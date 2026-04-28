#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>
#include <iomanip>

extern "C" {
    #include "hooks.h"
    #include "vector.h"
    #include "search.h"
    #include "photon_linux.h"
}

void run_basic_crud() {
    std::cout << "\n--- [Basic CRUD Test] ---" << std::endl;
    PhotonDBConfig cfg = { .dim = 4, .dtype = PHOTON_VECTOR_F32 };
    PhotonDB db;
    photon_db_create(&cfg, &db);

    float v1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    int id = photon_db_insert(&db, v1);
    std::cout << "Inserted vector with ID: " << id << std::endl;

    float v2[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    photon_db_upsert(&db, id, v2);
    std::cout << "Upserted ID " << id << " with new values." << std::endl;

    float out_v[4];
    photon_db_get(&db, id, out_v);
    std::cout << "Retrieved: [" << out_v[0] << ", " << out_v[1] << ", " << out_v[2] << ", " << out_v[3] << "]" << std::endl;

    photon_db_delete(&db, id);
    if (photon_db_get(&db, id, out_v) != 0) {
        std::cout << "Successfully deleted ID: " << id << std::endl;
    }

    photon_db_destroy(&db);
}

void run_persistence_test() {
    std::cout << "\n--- [Persistence Test] ---" << std::endl;
    PhotonDBConfig cfg = { .dim = 3, .dtype = PHOTON_VECTOR_F32 };
    PhotonDB db;
    photon_db_create(&cfg, &db);

    float data[3] = {1.1f, 2.2f, 3.3f};
    photon_db_insert(&db, data);
    
    const char* filename = "test_interactive.pdb";
    if (photon_db_save(&db, filename) == 0) {
        std::cout << "Saved DB to " << filename << std::endl;
    }

    PhotonDB loaded_db;
    if (photon_db_load(&loaded_db, filename) == 0) {
        std::cout << "Loaded DB from " << filename << std::endl;
        float out[3];
        photon_db_get(&loaded_db, 0, out);
        std::cout << "Verification: [" << out[0] << ", " << out[1] << ", " << out[2] << "]" << std::endl;
        photon_db_destroy(&loaded_db);
    }

    photon_db_destroy(&db);
}

void run_search_test() {
    std::cout << "\n--- [Search Test] ---" << std::endl;
    PhotonDBConfig cfg = { .dim = 2, .dtype = PHOTON_VECTOR_F32 };
    PhotonDB db;
    photon_db_create(&cfg, &db);

    float v1[2] = {1.0f, 0.0f};
    float v2[2] = {0.0f, 1.0f};
    float v3[2] = {0.707f, 0.707f};

    photon_db_insert(&db, v1);
    photon_db_insert(&db, v2);
    photon_db_insert(&db, v3);

    float query[2] = {1.0f, 0.1f};
    PhotonSearchResult results[3];
    int count = photon_db_search_dot_product(&db, query, 3, results);

    std::cout << "Search results for [1.0, 0.1]:" << std::endl;
    for (int i = 0; i < count; i++) {
        std::cout << i+1 << ". ID: " << results[i].id << " Score: " << results[i].score << std::endl;
    }

    photon_db_destroy(&db);
}

void run_benchmark() {
    std::cout << "\n--- [Benchmark] ---" << std::endl;
    const size_t DIM = 128;
    const size_t NUM_VECTORS = 5000;
    const size_t NUM_SEARCHES = 100;

    PhotonDBConfig cfg = { .dim = DIM, .dtype = PHOTON_VECTOR_F32 };
    PhotonDB db;
    photon_db_create(&cfg, &db);

    std::vector<float> vec(DIM, 0.5f);

    std::cout << "Inserting " << NUM_VECTORS << " vectors of dimension " << DIM << "..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < NUM_VECTORS; i++) {
        photon_db_insert(&db, vec.data());
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Insertion took: " << diff.count() << "s (" << NUM_VECTORS / diff.count() << " ops/s)" << std::endl;

    std::cout << "Performing " << NUM_SEARCHES << " searches (Top-10)..." << std::endl;
    PhotonSearchResult results[10];
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < NUM_SEARCHES; i++) {
        photon_db_search_dot_product(&db, vec.data(), 10, results);
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Search took: " << diff.count() << "s (" << NUM_SEARCHES / diff.count() << " searches/s)" << std::endl;

    photon_db_destroy(&db);
}

int main() {
    // Initialize with Linux defaults
    photonInit(photon_get_linux_init_struct());
    photonVectorInit();

    int choice = 0;
    while (true) {
        std::cout << "\n===============================" << std::endl;
        std::cout << "   PhotonDB Interactive Test   " << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "1. Basic CRUD Test" << std::endl;
        std::cout << "2. Persistence Test" << std::endl;
        std::cout << "3. Search Test" << std::endl;
        std::cout << "4. Benchmark" << std::endl;
        std::cout << "5. Run All Tests" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
            case 1: run_basic_crud(); break;
            case 2: run_persistence_test(); break;
            case 3: run_search_test(); break;
            case 4: run_benchmark(); break;
            case 5:
                run_basic_crud();
                run_persistence_test();
                run_search_test();
                run_benchmark();
                break;
            default: std::cout << "Invalid choice." << std::endl; break;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}
