#include <stdint.h>
#include "hooks.h"
#include "vector.h"

static PhotonInitStruct* hooks = NULL;

void photonVectorInit() {
  hooks = photonGetInitStruct();
}

static size_t get_element_size(PhotonVectorType dtype) {
  switch (dtype) {
    case PHOTON_VECTOR_I8: return sizeof(int8_t);
    case PHOTON_VECTOR_F32: return sizeof(float);
    default: return 0;
  }
}

static int is_hooks_valid() {
  return (hooks != NULL && 
          hooks->alloc != NULL && 
          hooks->free != NULL && 
          hooks->realloc != NULL &&
          hooks->memcpy != NULL &&
          hooks->memmove != NULL &&
          hooks->memset != NULL);
}

static int are_file_hooks_valid() {
  return (hooks != NULL &&
          hooks->fopen != NULL &&
          hooks->fwrite != NULL &&
          hooks->fread != NULL &&
          hooks->fclose != NULL);
}

static int ensure_capacity(PhotonDB *db, size_t min_capacity) {
  if (db == NULL || !is_hooks_valid()) return 0;
  if (db->capacity >= min_capacity) return 1;

  size_t new_capacity = db->capacity == 0 ? 8 : db->capacity * 2;
  if (new_capacity < db->capacity) return 0; // Overflow
  while (new_capacity < min_capacity) {
    size_t prev = new_capacity;
    new_capacity *= 2;
    if (new_capacity < prev) return 0; // Overflow
  }

  size_t element_size = get_element_size(db->dtype);
  if (element_size == 0) return 0;

  size_t vector_bytes = db->dim * element_size;
  void *new_data = hooks->realloc(db->data, new_capacity * vector_bytes);
  if (new_data == NULL) return 0;
  db->data = new_data;

  size_t *new_ids = (size_t *)hooks->realloc(db->list_of_id, new_capacity * sizeof(size_t));
  if (new_ids == NULL) return 0;
  db->list_of_id = new_ids;

  db->capacity = new_capacity;
  return 1;
}

typedef struct {
  char magic[4];
  uint32_t version;
  uint32_t dim;
  uint32_t dtype;
  uint64_t count;
  uint64_t next_id;
} PDBHeader;

int photon_db_save(PhotonDB *db, const char *filename) {
  if (db == NULL || filename == NULL || !are_file_hooks_valid()) return -1;

  void *f = hooks->fopen(filename, "wb");
  if (f == NULL) return -1;

  PDBHeader header;
  hooks->memcpy(header.magic, "PDB", 4);
  header.version = 1;
  header.dim = (uint32_t)db->dim;
  header.dtype = (uint32_t)db->dtype;
  header.count = (uint64_t)db->count;
  header.next_id = (uint64_t)db->next_id;

  if (hooks->fwrite(&header, sizeof(PDBHeader), 1, f) != 1) {
    hooks->fclose(f);
    return -1;
  }

  if (db->count > 0) {
    // Save list of IDs
    for (size_t i = 0; i < db->count; i++) {
      uint64_t id = (uint64_t)db->list_of_id[i];
      if (hooks->fwrite(&id, sizeof(uint64_t), 1, f) != 1) {
        hooks->fclose(f);
        return -1;
      }
    }

    // Save vector data
    size_t element_size = get_element_size(db->dtype);
    size_t vector_bytes = db->dim * element_size;
    if (hooks->fwrite(db->data, vector_bytes, db->count, f) != db->count) {
      hooks->fclose(f);
      return -1;
    }
  }

  hooks->fclose(f);
  return 0;
}

int photon_db_load(PhotonDB *db, const char *filename) {
  if (db == NULL || filename == NULL || !are_file_hooks_valid() || !is_hooks_valid()) return -1;

  void *f = hooks->fopen(filename, "rb");
  if (f == NULL) return -1;

  PDBHeader header;
  if (hooks->fread(&header, sizeof(PDBHeader), 1, f) != 1) {
    hooks->fclose(f);
    return -1;
  }

  // Check magic
  const char* magic = "PDB";
  for (int i = 0; i < 3; i++) {
    if (header.magic[i] != magic[i]) {
      hooks->fclose(f);
      return -1;
    }
  }

  // Check version
  if (header.version != 1) {
    hooks->fclose(f);
    return -1;
  }

  // Clear existing db
  photon_db_destroy(db);

  db->dim = (size_t)header.dim;
  db->dtype = (PhotonVectorType)header.dtype;
  db->next_id = (size_t)header.next_id;
  
  if (header.count > 0) {
    if (!ensure_capacity(db, (size_t)header.count)) {
      hooks->fclose(f);
      return -1;
    }

    // Load IDs
    for (size_t i = 0; i < (size_t)header.count; i++) {
      uint64_t id;
      if (hooks->fread(&id, sizeof(uint64_t), 1, f) != 1) {
        hooks->fclose(f);
        return -1;
      }
      db->list_of_id[i] = (size_t)id;
    }

    // Load vector data
    size_t element_size = get_element_size(db->dtype);
    size_t vector_bytes = db->dim * element_size;
    if (hooks->fread(db->data, vector_bytes, (size_t)header.count, f) != (size_t)header.count) {
      hooks->fclose(f);
      return -1;
    }
    db->count = (size_t)header.count;
  }

  hooks->fclose(f);
  return 0;
}

void photon_db_create(const PhotonDBConfig *cfg, PhotonDB *out_db) {
  if (out_db == NULL) return;
  
  if (hooks != NULL && hooks->memset != NULL) {
    hooks->memset(out_db, 0, sizeof(PhotonDB));
  } else {
    // Fallback if hooks aren't ready yet, though user should call photonVectorInit first
    uint8_t* ptr = (uint8_t*)out_db;
    for (size_t i = 0; i < sizeof(PhotonDB); i++) ptr[i] = 0;
  }
  
  if (cfg == NULL || cfg->dim == 0) return;
  
  out_db->dim = cfg->dim;
  out_db->dtype = cfg->dtype;
}

void photon_db_destroy(PhotonDB *db) {
  if (db == NULL || !is_hooks_valid()) return;

  if (db->data != NULL) {
    hooks->free(db->data);
  }
  if (db->list_of_id != NULL) {
    hooks->free(db->list_of_id);
  }

  hooks->memset(db, 0, sizeof(PhotonDB));
  db->dtype = PHOTON_VECTOR_NONE;
}

int photon_db_insert(PhotonDB *db, const void *in_vector) {
  if (db == NULL || in_vector == NULL || !is_hooks_valid()) return -1;
  if (!ensure_capacity(db, db->count + 1)) return -1;

  size_t element_size = get_element_size(db->dtype);
  size_t vector_bytes = db->dim * element_size;
  
  hooks->memcpy((uint8_t*)db->data + (db->count * vector_bytes), in_vector, vector_bytes);
  
  size_t new_id = db->next_id++;
  db->list_of_id[db->count] = new_id;
  db->count++;
  
  return (int)new_id;
}

int photon_db_get(PhotonDB *db, size_t id, const void *out_vector) {
  if (db == NULL || out_vector == NULL || db->data == NULL || !is_hooks_valid()) return -1;

  for (size_t i = 0; i < db->count; i++) {
    if (db->list_of_id[i] == id) {
      size_t element_size = get_element_size(db->dtype);
      size_t vector_bytes = db->dim * element_size;
      hooks->memcpy((void*)out_vector, (uint8_t*)db->data + (i * vector_bytes), vector_bytes);
      return 0;
    }
  }
  return -1;
}

int photon_db_delete(PhotonDB *db, size_t id) {
  if (db == NULL || db->data == NULL || !is_hooks_valid()) return -1;

  for (size_t i = 0; i < db->count; i++) {
    if (db->list_of_id[i] == id) {
      size_t element_size = get_element_size(db->dtype);
      size_t vector_bytes = db->dim * element_size;
      
      if (i < db->count - 1) {
        hooks->memmove((uint8_t*)db->data + (i * vector_bytes), 
                       (uint8_t*)db->data + ((i + 1) * vector_bytes), 
                       (db->count - 1 - i) * vector_bytes);
        hooks->memmove(&db->list_of_id[i], &db->list_of_id[i+1], (db->count - 1 - i) * sizeof(size_t));
      }
      db->count--;
      return 0;
    }
  }
  return -1;
}

int photon_db_upsert(PhotonDB *db, size_t id, const void *in_vector) {
  if (db == NULL || in_vector == NULL || !is_hooks_valid()) return -1;

  for (size_t i = 0; i < db->count; i++) {
    if (db->list_of_id[i] == id) {
      size_t element_size = get_element_size(db->dtype);
      size_t vector_bytes = db->dim * element_size;
      hooks->memcpy((uint8_t*)db->data + (i * vector_bytes), in_vector, vector_bytes);
      return 0;
    }
  }

  if (!ensure_capacity(db, db->count + 1)) return -1;

  size_t element_size = get_element_size(db->dtype);
  size_t vector_bytes = db->dim * element_size;
  
  hooks->memcpy((uint8_t*)db->data + (db->count * vector_bytes), in_vector, vector_bytes);
  db->list_of_id[db->count] = id;
  db->count++;
  
  if (id >= db->next_id) {
    db->next_id = id + 1;
  }
  
  return 0;
}
