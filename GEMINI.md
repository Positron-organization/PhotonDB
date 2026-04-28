# PhotonDB Project Guide for Gemini CLI

## What is PhotonDB?

**PhotonDB** is a lightweight vector database designed and optimized to run efficiently on **embedded devices**.
Its architecture focuses on **performance**, **portability**, and **cross-platform compatibility**.

---

## Project Structure

```text
.
├── core
│   └── src
│       ├── detect.c
│       ├── detect.h
│       ├── hooks.c
│       ├── hooks.h
│       ├── search.c
│       ├── search.h
│       ├── simd.c
│       ├── simd.h
│       ├── vector.c
│       └── vector.h
├── examples
│   ├── basic_usage
│   ├── persistence
│   ├── bulk_operations
│   └── posix_default
├── platform
│   └── posix
│       ├── build.zig
│       ├── photon_posix.c
│       └── photon_posix.h
├── test
│   └── src
│       └── main.cpp
├── GEMINI.md
└── README.md
```

---

## Directory Overview

## `core/src/detect.h` / `detect.c`

This module provides **compile-time and runtime environment detection**. It identifies:
*   **Operating System**: Linux, Windows, macOS, FreeRTOS, etc.
*   **Architecture**: x86, x64, ARM, RISC-V.
*   **Device Class**: Detects if the target is an embedded MCU.
*   **SIMD Support**: Identifies availability of SSE, AVX, NEON, and RVV.

---

## `core/src/hooks.h` / `hooks.c`

This module contains a **struct of function pointers** for platform-dependent operations such as memory allocation, memory management, logging, and **file I/O**.

### Why does this exist?

PhotonDB is built with a **multi-platform-first mindset**, abstracting OS-specific APIs (including filesystem access) so it can run on everything from bare-metal MCUs to full Linux/POSIX systems.

---

## `core/src/vector.h` / `vector.c`

This is the **core engine** of PhotonDB. It contains the main database logic, including:

* CRUD operations
* internal database management
* vector storage
* **Persistence (Save/Load to .pdb files)**

---

## `core/src/search.h` / `search.c`

This module implements **vector search algorithms**. It currently supports:

* **Dot Product**: Direct similarity calculation.
* **Euclidean (L2) Distance**: Distance-based similarity (lower is better).
* **Cosine Similarity**: Angular similarity between vectors.
* **Top-K filtering**: Efficient result ranking for all metrics.

---

## `core/src/simd.h` / `simd.c`

This module provides **SIMD-accelerated** implementations for core mathematical operations used in search.
*   **x86 Support**: SSE2, AVX, AVX2.
*   **ARM Support**: NEON.
*   **RISC-V Support**: RVV (Vector Extension).
*   **Automatic Fallback**: Gracefully falls back to scalar implementations if SIMD is not supported or detected.

---

## `test/src/main.cpp`

This is an **interactive C++ test runner**. It provides a CLI menu to execute various test scenarios:
*   **Basic CRUD**: Validates insertion, retrieval, and deletion.
*   **Persistence**: Verifies saving and loading databases to `.pdb` files.
*   **Search**: Tests the accuracy of the vector search algorithms.
*   **Benchmark**: Measures performance for high-volume operations.

Run it using:
```bash
zig build run_test
```

---

## `platform/<platform>/`

This directory contains the **build system and platform integration** for each supported target.

### POSIX Implementation (`photon_posix.h` / `photon_posix.c`)

For Linux/POSIX systems, a default `PhotonInitStruct` is provided that uses standard C library functions (`malloc`, `free`, `fopen`, etc.). This allows for quick integration on standard operating systems.

Building the project (e.g., via `zig build`) generates artifacts in `zig-out/`:
* `zig-out/lib/libPhotonCore.a`: The static library.
* `zig-out/include/*.h`: The public header files.

### Running Examples

Examples can be run using the Zig build system from the `platform/posix` directory:

```bash
zig build run_example -Dexample=<example_folder_name>
```

Available examples:
- `basic_usage`: General CRUD and search operations.
- `persistence`: Demonstrates saving and loading databases.
- `bulk_operations`: Demonstrates handling larger datasets and deletions.
- `posix_default`: Demonstrates using the built-in POSIX default configuration.

---

## Design Philosophy

PhotonDB is built around these principles:

* **Portable** → Run anywhere
* **Lightweight** → Suitable for embedded hardware
* **Modular** → Clear separation between core and platform code
* **Efficient** → Optimized for constrained systems
* **Maintainable** → Easy to extend for new platforms

---

## Summary

PhotonDB separates:

| Layer    | Responsibility             |
| -------- | -------------------------- |
| Core     | Database logic & Search    |
| Hooks    | OS abstraction             |
| Platform | Build + target integration |

This allows PhotonDB to scale from tiny MCUs to full POSIX systems with minimal changes.
