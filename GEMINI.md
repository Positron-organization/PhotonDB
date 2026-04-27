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
│       ├── vector.c
│       └── vector.h
├── GEMINI.md
├── platform
│   └── linux
│       └── build.zig
└── README.md
```

---

## Directory Overview

## `core/src/hooks.h` / `hooks.c`

This module contains a **struct of function pointers** for platform-dependent operations such as:

* memory allocation (`alloc`)
* memory reallocation (`realloc`)
* memory free (`free`)
* logging
* other low-level system hooks

### Why does this exist?

PhotonDB is built with a **multi-platform-first mindset**.

Instead of hardcoding OS-specific APIs, PhotonDB abstracts them through hooks so it can run on:

* Linux systems
* Microcontrollers (MCU)
* RTOS environments
* Bare-metal systems
* Other custom platforms

---

## `core/src/vector.h` / `vector.c`

This is the **core engine** of PhotonDB.

It contains the main database logic, such as:

* CRUD operations
* vector storage
* indexing
* search logic
* internal database management

This is where PhotonDB’s main functionality lives.

---

## `platform/<platform>/`

This directory contains the **build system and platform integration** for each supported target.

Each platform can have its own build configuration.

### Example

```text
platform/linux/build.zig
```

Used to build PhotonDB for Linux with Zig build system.

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
| Core     | Database logic             |
| Hooks    | OS abstraction             |
| Platform | Build + target integration |

This allows PhotonDB to scale from tiny MCUs to full Linux systems with minimal changes.

