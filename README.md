# ⚛️ PhotonDB

**PhotonDB** is a ultra-lightweight vector database designed for the edge. Optimized for **embedded devices**, **microcontrollers (MCU)**, and **RTOS environments**, it brings high-performance vector search to resource-constrained hardware.

[![License](https://img.shields.io/badge/license-AGPL--3.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20MCU%20%7C%20Bare--Metal-orange)](https://github.com/yourusername/PhotonDB)
[![Language](https://img.shields.io/badge/language-C99-green)](https://en.wikipedia.org/wiki/C99)

---

## 🤔 Why PhotonDB?

Modern AI applications often require vector similarity search, but existing solutions are often too heavy for embedded hardware. **PhotonDB** bridges this gap by providing:

*   ✅ **Efficiency**: Runs on devices with as little as 64KB of RAM.
*   ✅ **Speed**: Optimized brute-force search for small to medium datasets.
*   ✅ **Safety**: No hidden allocations; you control the memory through hooks.
*   ✅ **Simplicity**: A single-header-inspired design that is easy to integrate.

---

## 🚀 Key Features

*   🎯 **Brute-force Precision**: High-accuracy dot product similarity search.
*   💾 **Persistent Storage**: Robust Save/Load functionality using custom `.pdb` format.
*   🌍 **Universal Portability**: Runs on everything from 8-bit MCUs to high-end Linux servers.
*   🪶 **Minimal Footprint**: Extremely low memory overhead, tailored for silicon-constrained environments.
*   🧩 **Modular Hooks**: Complete abstraction of OS-specific tasks (I/O, Memory, Logging).
*   ⚡ **Embedded Efficiency**: Written in pure C99 with zero external dependencies.

---

## 🏗️ Architecture

PhotonDB is built with a clean three-layer separation of concerns:

| Layer | Responsibility | Icon |
| :--- | :--- | :---: |
| **Core** | Vector engine, CRUD operations, and binary persistence logic. | 🧠 |
| **Hooks** | OS-independent abstraction for I/O, memory, and logging. | 🔌 |
| **Platform** | Build orchestration (Zig) and target-specific glue code. | 🏗️ |

---

## 🛠️ Getting Started

### 1️⃣ Initialization

Configure your platform hooks and wake up the engine:

```c
#include "hooks.h"
#include "vector.h"

PhotonInitStruct hooks = {
    .alloc   = my_malloc,   // Use your RTOS or custom heap
    .realloc = my_realloc,
    .free    = my_free,
    .memcpy  = memcpy,
    .memmove = memmove,
    .memset  = memset,
    .log     = my_log_function,
    // File I/O hooks for .pdb persistence
    .fopen   = my_fopen,
    .fwrite  = my_fwrite,
    .fread   = my_fread,
    .fclose  = my_fclose
};

photonInit(hooks);
photonVectorInit();
```

### 2️⃣ Core Operations

```c
#include "vector.h"
#include "search.h"

// 🛠️ Create a DB
PhotonDBConfig cfg = { .dim = 128, .dtype = PHOTON_VECTOR_F32 };
PhotonDB db;
photon_db_create(&cfg, &db);

// 📥 Insert
float vector[128] = { ... };
int id = photon_db_insert(&db, vector);

// 💾 Save to Disk
photon_db_save(&db, "vectors.pdb");

// 🔍 Search
PhotonSearchResult results[5];
photon_db_search_dot_product(&db, query_vector, 5, results);

// 🧹 Cleanup
photon_db_destroy(&db);
```

---

## 📂 Project Structure

```text
.
├── core/src/
│   ├── hooks.c/h   # 🔌 System abstraction layer
│   ├── vector.c/h  # 🧠 Main database logic
│   └── search.c/h  # 🔍 Search algorithms
├── platform/
│   └── linux/      # 🏗️ Build targets (Zig)
├── test/src/       # 🧪 C++ test suite
└── examples/       # 💡 Integration examples
```

---

## 🔨 Building & Testing

We use the [Zig build system](https://ziglang.org/) for modern, cross-platform compilation.

### 📦 Build Library
```bash
cd platform/linux
zig build
```

### 🧪 Run Tests
```bash
zig build run_test
```

### 💡 Quick Start Example
```bash
zig build run_example
```

---

## 🎯 Roadmap

- [ ] **Advanced Indexing**: HNSW & IVF support for sub-linear search.
- [ ] **Expanded Metrics**: L2 Distance and Cosine Similarity.
- [ ] **Vector Quantization**: PQ & BQ for 10x memory reduction.
- [ ] **SIMD Acceleration**: NEON (ARM) & AVX (x86) optimizations.
- [ ] **Metadata Support**: Attribute-based filtering.
- [ ] **MCU Drivers**: Official support for ESP32, STM32, and Nordic.
- [ ] **Concurrency**: Thread-safe handles for multi-core systems.

---

## ⚖️ License

Distributed under the AGPL-3.0 License. See `LICENSE` for more information.

---

## 🤝 Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 🆘 Support

If you encounter any issues or have questions, feel free to:
*   Open an [Issue](https://github.com/yourusername/PhotonDB/issues)
*   Check the [GEMINI.md](GEMINI.md) for internal architecture details.
*   Reach out to the maintainers.

---

<p align="center">Built with ❤️ for the Embedded Community</p>
