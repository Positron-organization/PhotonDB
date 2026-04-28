# Contributing to PhotonDB

Thank you for your interest in contributing to PhotonDB! We welcome contributions from the community to help make PhotonDB the best embedded vector database.

## How to Contribute

1. **Fork the repository** and create your branch from `main`.
2. **Implement your changes**. Ensure your code adheres to our [Coding Standards](#coding-standards).
3. **Add tests** for any new features or bug fixes.
4. **Verify your changes** by running the existing test suite and your new tests.
5. **Submit a Pull Request** with a clear description of your changes and why they are needed.

## Coding Standards

- **Language**: Core logic must be written in standard C99.
- **Portability**: Avoid using OS-specific APIs directly in the `core/` directory. Use the hook system in `hooks.h` for memory, I/O, and logging.
- **Naming**: Use `photon_` prefix for public functions and `Photon` prefix for public types. Use `snake_case` for functions and `PascalCase` for types.
- **Documentation**: Provide clear comments for all public functions in header files.
- **Simplicity**: Favor simple, readable implementations over complex optimizations unless they provide significant performance benefits.

## Build & Test

We use the Zig build system for development on POSIX-compliant systems (Linux, macOS, etc.).

```bash
cd platform/posix
# Build the library
zig build
# Run tests
zig build run_test
# Run the example
zig build run_example
```

## Reporting Issues

If you find a bug or have a feature request, please open an issue on GitHub. Provide as much detail as possible, including steps to reproduce the issue and your environment details.

## License

By contributing to PhotonDB, you agree that your contributions will be licensed under the project's [LICENSE](LICENSE).
