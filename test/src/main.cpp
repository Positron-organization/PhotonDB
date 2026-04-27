#include <iostream>
#include <vector>
#include <cstring>

extern "C" {
    #include "hooks.h"
    #include "vector.h"
}

// Basic implementation of hooks for testing
void* my_alloc(size_t size) { return malloc(size); }
void* my_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
void my_free(void* ptr) { free(ptr); }
void my_log(const char* msg) { std::cout << "[Photon Log] " << msg << std::endl; }

// Wrappers for memory functions to match 'void' return type
void wrap_memcpy(void* dst, const void* src, size_t n) { memcpy(dst, src, n); }
void wrap_memmove(void* dst, const void* src, size_t n) { memmove(dst, src, n); }
void wrap_memset(void* s, int c, size_t n) { memset(s, c, n); }

// File I/O hooks
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

    PhotonDBConfig cfg = {
        .dim = 4,
        .dtype = PHOTON_VECTOR_F32
    };

    PhotonDB db;
    photon_db_create(&cfg, &db);

    float v1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    int id1 = photon_db_insert(&db, v1);
    
    std::cout << "Inserted vector with ID: " << id1 << std::endl;

    if (photon_db_save(&db, "test.pdb") == 0) {
        std::cout << "Database saved successfully to test.pdb" << std::endl;
    } else {
        std::cerr << "Failed to save database" << std::endl;
        return 1;
    }

    PhotonDB loaded_db;
    memset(&loaded_db, 0, sizeof(PhotonDB));
    if (photon_db_load(&loaded_db, "test.pdb") == 0) {
        std::cout << "Database loaded successfully from test.pdb" << std::endl;
        
        float out_v[4];
        if (photon_db_get(&loaded_db, (size_t)id1, out_v) == 0) {
            std::cout << "Retrieved vector: " << out_v[0] << ", " << out_v[1] << ", " << out_v[2] << ", " << out_v[3] << std::endl;
            if (memcmp(v1, out_v, sizeof(v1)) == 0) {
                std::cout << "Data integrity verified!" << std::endl;
            } else {
                std::cerr << "Data mismatch!" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Failed to retrieve vector from loaded DB" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Failed to load database" << std::endl;
        return 1;
    }

    photon_db_destroy(&db);
    photon_db_destroy(&loaded_db);

    std::cout << "Tests passed successfully!" << std::endl;
    return 0;
}
