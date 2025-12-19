# Tests & Benchmarks

Driven by Google Test and Google Benchmarks.

Super fan of Google?

## Commands

Build for debug.

```bash
$ cmake --preset test
$ cmake --build --preset test
```

Build for release.

```bash
$ cmake --preset release
$ cmake --build --preset release
```

Run gtest.

```bash
$ ctest --preset test
```

Run gbench.

```bash
$ ./build/release/google_benchmarks/ano_benchmarks
```

## Configuartion

See CMakePresets.json
