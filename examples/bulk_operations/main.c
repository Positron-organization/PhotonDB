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
        .alloc = my_alloc, .realloc = my_realloc, .free = my_free,
        .memcpy = wrap_memcpy, .memmove = wrap_memmove, .memset = wrap_memset,
        .log = my_log, .fopen = my_fopen, .fwrite = my_fwrite, .fread = my_fread, .fclose = my_fclose
    };

    photonInit(hooks);
    photonVectorInit();

    PhotonDBConfig cfg = { .dim = 2, .dtype = PHOTON_VECTOR_F32 };
    PhotonDB db;
    photon_db_create(&cfg, &db);

    printf("Bulk inserting 100 vectors...\n");
    for (int i = 0; i < 100; i++) {
        float v[2] = {(float)i, (float)(i * 2)};
        photon_db_insert(&db, v);
    }
    printf("Current count: %zu, Capacity: %zu\n", db.count, db.capacity);

    printf("Deleting even-indexed vectors...\n");
    for (int i = 2; i <= 100; i += 2) {
        photon_db_delete(&db, i);
    }
    printf("Current count after deletions: %zu\n", db.count);

    float v_check[2];
    if (photon_db_get(&db, 1, v_check) == 0) {
        printf("Vector 1 still exists: [%.1f, %.1f]\n", v_check[0], v_check[1]);
    }
    if (photon_db_get(&db, 2, v_check) != 0) {
        printf("Vector 2 successfully deleted.\n");
    }

    photon_db_destroy(&db);
    return 0;
}
