# PhotonDB

PhotonDB is a lightweight vector database designed and optimized for efficiency on embedded devices, microcontrollers (MCU), and RTOS environments. Its architecture prioritizes performance, portability, and cross-platform compatibility through a clear separation of core logic and system abstractions.

## Key Features

- **Brute-force Search**: High-precision dot product similarity search.
- **Persistent Storage**: Save and load databases to `.pdb` files via platform hooks.
- **Portable**: Run anywhere from bare-metal MCUs to Linux systems.
- **Lightweight**: Minimal memory footprint, ideal for resource-constrained hardware.
- **Modular**: Clean separation between database logic and platform-dependent code.
- **Efficient**: Optimized for embedded performance.

## Architecture

PhotonDB is organized into three distinct layers:

| Layer | Responsibility |
| :--- | :--- |
| **Core** | Main database engine, vector storage, CRUD, and persistence. |
| **Hooks** | OS abstraction layer for memory, logging, and file I/O. |
| **Platform** | Build configurations and target-specific integrations. |

## Getting Started

### Initialization

Before using PhotonDB, you must provide platform-specific hooks for memory management, logging, and file I/O, then initialize the vector engine:

```c
#include "hooks.h"
#include "vector.h"

PhotonInitStruct hooks = {
    .alloc = my_malloc,
    .realloc = my_realloc,
    .free = my_free,
    .memcpy = memcpy,
    .memmove = memmove,
    .memset = memset,
    .log = my_log_function,
    // File I/O hooks for persistence
    .fopen = my_fopen,
    .fwrite = my_fwrite,
    .fread = my_fread,
    .fclose = my_fclose
};

photonInit(hooks);
photonVectorInit(); // Link hooks to the vector engine
```

### Supported Data Types

PhotonDB currently supports the following vector element types:
- `PHOTON_VECTOR_I8`: 8-bit signed integers.
- `PHOTON_VECTOR_F32`: 32-bit floating point numbers.

### Database Operations

Define your database configuration and perform CRUD operations:

```c
#include "vector.h"

PhotonDBConfig cfg = {
    .dim = 128,
    .dtype = PHOTON_VECTOR_F32
};

PhotonDB db;
photon_db_create(&cfg, &db);

// Insert a vector
float vector[128] = { ... };
int id = photon_db_insert(&db, vector);

// Persistence: Save the database
photon_db_save(&db, "vectors.pdb");

// Persistence: Load from file
PhotonDB loaded_db;
photon_db_load(&loaded_db, "vectors.pdb");

// Search for similar vectors
#include "search.h"
PhotonSearchResult results[5];
int count = photon_db_search_dot_product(&db, query_vector, 5, results);

// Clean up
photon_db_destroy(&db);
photon_db_destroy(&loaded_db);
```

## Project Structure

```text
.
├── core
│   └── src
│       ├── hooks.c/h   # Platform-dependent OS abstraction hooks
│       ├── vector.c/h  # Core vector database logic
│       └── search.c/h  # Vector search algorithms
├── platform
│   └── linux           # Linux build system (Zig)
├── test
│   └── src             # Test suite (C++)
└── GEMINI.md           # Internal project guide
```

## Building & Testing

PhotonDB uses the [Zig build system](https://ziglang.org/) for platform integrations.

### Build Library
```bash
cd platform/linux
zig build
```
This generates:
- `zig-out/lib/libPhotonCore.a`
- `zig-out/include/` (header files)

### Run Tests
```bash
cd platform/linux
zig build run_test
```
The test runner automatically links against the static library and uses headers from `zig-out/include/`.

## Design Philosophy

- **Multi-platform-first**: Instead of hardcoding OS APIs, PhotonDB uses a hook-based system to remain compatible with any environment.
- **Surgical Updates**: Designed for stability and maintainable codebases.
- **No Dependencies**: Focuses on standard C and explicit memory control.
