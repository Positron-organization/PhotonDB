#include <stdio.h>
#include "hooks.h"
#include "vector.h"
#include "photon_linux.h"

int main() {
    // Initialize PhotonDB with Linux default hooks
    PhotonInitStruct hooks = photon_get_linux_init_struct();
    photonInit(hooks);
    photonVectorInit();

    printf("PhotonDB initialized with Linux default config!\n");

    // Create a database
    PhotonDBConfig cfg = {
        .dim = 4,
        .dtype = PHOTON_VECTOR_F32
    };

    PhotonDB db;
    photon_db_create(&cfg, &db);
    printf("Database created (dim: %zu)\n", cfg.dim);

    // Insert a vector
    float v1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    int id1 = photon_db_insert(&db, v1);
    printf("Inserted vector with ID: %d\n", id1);

    // Save the database
    const char* filename = "linux_default_demo.pdb";
    if (photon_db_save(&db, filename) == 0) {
        printf("Database saved to %s\n", filename);
    } else {
        printf("Failed to save database\n");
    }

    // Cleanup
    photon_db_destroy(&db);
    printf("Database destroyed\n");

    return 0;
}
