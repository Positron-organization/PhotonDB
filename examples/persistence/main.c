#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hooks.h"
#include "vector.h"

// Platform-specific hooks
void* my_alloc(size_t size) { return malloc(size); }
void* my_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
void my_free(void* ptr) { free(ptr); }
void my_log(const char* msg) { printf("[PhotonDB] %s\n", msg); }
void wrap_memcpy(void* dst, const void* src, size_t n) { memcpy(dst, src, n); }
void wrap_memmove(void* dst, const void* src, size_t n) { memmove(dst, src, n); }
void wrap_memset(void* s, int c, size_t n) { memset(s, c, n); }

void* my_fopen(const char* filename, const char* mode) { return fopen(filename, mode); }
size_t my_fwrite(const void* ptr, size_t size, size_t nmemb, void* stream) { return fwrite(ptr, size, nmemb, (FILE*)stream); }
size_t my_fread(void* ptr, size_t size, size_t nmemb, void* stream) { return fread(ptr, size, nmemb, (FILE*)stream); }
int my_fclose(void* stream) { return fclose((FILE*)stream); }

int main() {
    PhotonInitStruct hooks = {
        .alloc = my_alloc,
        .realloc = my_realloc,
        .free = my_free,
        .memcpy = wrap_memcpy,
        .memmove = wrap_memmove,
        .memset = wrap_memset,
        .log = my_log,
        .fopen = my_fopen,
        .fwrite = my_fwrite,
        .fread = my_fread,
        .fclose = my_fclose
    };

    photonInit(hooks);
    photonVectorInit();

    const char* db_file = "persistence_demo.pdb";

    // 1. Create and save a database
    {
        PhotonDBConfig cfg = { .dim = 3, .dtype = PHOTON_VECTOR_F32 };
        PhotonDB db;
        photon_db_create(&cfg, &db);

        float v1[3] = {1.1f, 2.2f, 3.3f};
        float v2[3] = {4.4f, 5.5f, 6.6f};
        photon_db_insert(&db, v1);
        photon_db_insert(&db, v2);

        printf("Saving database with 2 vectors to %s...\n", db_file);
        photon_db_save(&db, db_file);
        photon_db_destroy(&db);
    }

    // 2. Load the database
    {
        printf("Loading database from %s...\n", db_file);
        PhotonDB db;
        photon_db_create(NULL, &db); // Initialize to zero
        if (photon_db_load(&db, db_file) == 0) {
            printf("Database loaded successfully.\n");
            printf("Dimension: %zu, Count: %zu\n", db.dim, db.count);

            float out[3];
            for (size_t i = 0; i < db.count; i++) {
                size_t id = db.list_of_id[i];
                if (photon_db_get(&db, id, out) == 0) {
                    printf("Vector ID %zu: [%.1f, %.1f, %.1f]\n", id, out[0], out[1], out[2]);
                }
            }
            photon_db_destroy(&db);
        } else {
            printf("Failed to load database.\n");
        }
    }

    return 0;
}
