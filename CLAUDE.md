# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Anon is a header-heavy C++23 static library providing an actor model framework with supporting lock-free data structures and a memory pool allocator.

## Build & Test Commands

```bash
# Debug build (for tests)
cmake --preset test && cmake --build --preset test

# Run all tests
ctest --preset test

# Release build (for benchmarks)
cmake --preset release && cmake --build --preset release

# Run benchmarks
./build/release/google_benchmarks/ano_benchmarks
```

Build uses Ninja generator. Dependencies (GoogleTest 1.14.0, Google Benchmark 1.8.3) are fetched automatically via CMake FetchContent.

## Architecture

### Core Components (all under `include/Anon/`)

- **Message** (`Message.h`, `Message.inl`) — Type-erased, move-only message container with dual storage: inline (small buffer optimization up to N bytes) or external (pool-allocated). Uses operation tables (`MsgOps`) for polymorphism instead of virtual functions. Predefined sizes: `MessageS` (64B), `MessageM` (128B), `MessageL` (256B).

- **Stream** (`Stream.h`, `Stream.inl`) — Lock-free SPSC FIFO circular buffer. Capacity must be power of 2. Head/tail are cache-aligned (64-byte) atomics. Wait-free for single producer/single reader.

- **MemPool** (`MemPool.h`) — Fixed-size pool allocator with 7 bucket size classes (16–1024 bytes). Uses free lists for reuse, arena allocation for new blocks, and falls back to `malloc`/`free` for allocations >1024 bytes. Each allocation has a 16-byte header.

- **Type ID** (`Types.h`) — RTTI-free type identification using static variable addresses as unique `type_id_t` values.

### Actor Model (designed, partially implemented — see `doc/actor-system-design.md`)

Stage → Dispatcher → Actors (with Address handles and Stream inboxes). Dispatcher is concept-based (compile-time polymorphism, no virtual overhead).

### Key Design Decisions

- C++23 required (concepts, `std::max_align_t`, placement new with `std::launder`)
- No RTTI, no virtual functions — uses type erasure and concepts throughout
- Move-only semantics for messages (no copies)
- All public headers aggregated in `src/All.cpp`

## Code Style

- `.clang-format`: 80-column limit, 2-space indent, attached braces (K&R)
- Pointer alignment: left (`int* p`)
