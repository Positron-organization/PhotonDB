#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hooks.h"
#include "vector.h"
#include "search.h"

// 1. Define platform-specific hooks
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
    // 2. Initialize PhotonDB with hooks
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

    // 3. Create a database for 4D floating-point vectors
    PhotonDBConfig cfg = {
        .dim = 4,
        .dtype = PHOTON_VECTOR_F32
    };

    PhotonDB db;
    photon_db_create(&cfg, &db);
    printf("Database created (dim: %zu)\n", cfg.dim);

    // 4. Insert some vectors
    float v1[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    float v2[4] = {0.0f, 1.0f, 0.0f, 0.0f};
    float v3[4] = {0.8f, 0.2f, 0.0f, 0.0f};

    int id1 = photon_db_insert(&db, v1);
    int id2 = photon_db_insert(&db, v2);
    int id3 = photon_db_insert(&db, v3);

    printf("Inserted 3 vectors with IDs: %d, %d, %d\n", id1, id2, id3);

    // 5. Perform a similarity search
    // Let's search for vectors similar to [0.9, 0.1, 0.0, 0.0]
    float query[4] = {0.9f, 0.1f, 0.0f, 0.0f};
    PhotonSearchResult results[2];
    int found = photon_db_search_dot_product(&db, query, 2, results);

    printf("Search results for query [0.9, 0.1, 0.0, 0.0]:\n");
    for (int i = 0; i < found; i++) {
        printf("  %d. ID: %zu, Score (Dot Product): %.4f\n", i + 1, results[i].id, results[i].score);
    }

    // 6. Save to disk
    const char* db_file = "example.pdb";
    if (photon_db_save(&db, db_file) == 0) {
        printf("Database saved to %s\n", db_file);
    }

    // 7. Cleanup
    photon_db_destroy(&db);
    printf("Database destroyed.\n");

    return 0;
}
