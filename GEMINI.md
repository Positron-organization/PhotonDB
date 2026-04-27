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
│       ├── hooks.c
│       ├── hooks.h
│       ├── search.c
│       ├── search.h
│       ├── vector.c
│       └── vector.h
├── platform
│   └── linux
│       └── build.zig
├── test
│   └── src
│       └── main.cpp
├── GEMINI.md
└── README.md
```

---

## Directory Overview

## `core/src/hooks.h` / `hooks.c`

This module contains a **struct of function pointers** for platform-dependent operations such as memory allocation, memory management, logging, and **file I/O**.

### Why does this exist?

PhotonDB is built with a **multi-platform-first mindset**, abstracting OS-specific APIs (including filesystem access) so it can run on everything from bare-metal MCUs to full Linux systems.

---

## `core/src/vector.h` / `vector.c`

This is the **core engine** of PhotonDB. It contains the main database logic, including:

* CRUD operations
* internal database management
* vector storage
* **Persistence (Save/Load to .pdb files)**

---

## `core/src/search.h` / `search.c`

This module implements **vector search algorithms**. Currently, it provides:

* Brute-force dot product search
* Top-K result filtering

---

## `platform/<platform>/`

This directory contains the **build system and platform integration** for each supported target.
Building the project (e.g., via `zig build`) generates artifacts in `zig-out/`:
* `zig-out/lib/libPhotonCore.a`: The static library.
* `zig-out/include/*.h`: The public header files.

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

This allows PhotonDB to scale from tiny MCUs to full Linux systems with minimal changes.
