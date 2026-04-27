#include <stddef.h>

void photonVectorInit();

// Database

typedef enum {
  PHOTON_VECTOR_I8,
  PHOTON_VECTOR_F32,
  PHOTON_VECTOR_NONE = 0,
} PhotonVectorType;

typedef struct {
  size_t dim;
  PhotonVectorType dtype;
} PhotonDBConfig;

typedef struct {
  void* data;
  size_t* list_of_id;
  size_t count;
  size_t capacity;
  size_t next_id;
  size_t dim;
  PhotonVectorType dtype;
} PhotonDB;

void photon_db_create(const PhotonDBConfig* cfg, PhotonDB* out_db);
void photon_db_destroy(PhotonDB* db);

// CRUD
int photon_db_insert(PhotonDB* db, const void* in_vector);
int photon_db_delete(PhotonDB* db, size_t id);
int photon_db_get(PhotonDB* db, size_t id, const void* out_vector);
int photon_db_upsert(PhotonDB* db, size_t id, const void* in_vector);
