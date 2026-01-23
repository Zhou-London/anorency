# Memory Pool Implementation Dev Log

**Date:** 2026-01-22

## Summary

Implemented memory pool allocation support for the `Message` class, enabling payloads to be allocated from a `MemPool` instead of the inline SBO (Small Buffer Optimization) buffer.

## Changes Made

### 1. MemPool Class (`include/Pools/MemPool.h`)

Implemented the `MemPool` class with `allocate` and `deallocate` methods using `std::aligned_alloc` and `std::free`.

**Key implementation detail:** On 64-bit platforms (macOS, Linux), `std::aligned_alloc` requires alignment to be at least `sizeof(void*)` (8 bytes). The implementation enforces this minimum alignment to ensure cross-platform compatibility.

```cpp
void* allocate(std::size_t size, std::size_t align) noexcept {
  if (size == 0) return nullptr;
  std::size_t min_align = sizeof(void*);
  std::size_t actual_align = align > min_align ? align : min_align;
  std::size_t adjusted_size = ((size + actual_align - 1) / actual_align) * actual_align;
  return std::aligned_alloc(actual_align, adjusted_size);
}
```

### 2. Pool-based Message::make() (`include/Messages/Message.inl`)

Implemented the pool-based `make()` method that:
1. Allocates memory from the pool
2. Constructs the object in-place using placement new
3. Stores the pointer and deallocator in `external_` storage
4. Sets `use_external_storage_ = true`

### 3. Bug Fixes

**move_from() bug:** The original implementation checked `this->use_external_storage_` instead of `other.use_external_storage_`. This caused incorrect behavior when moving pool-allocated messages.

**try_get() fix:** Modified to return the correct pointer based on `use_external_storage_`:
```cpp
const void* obj = use_external_storage_ ? external_.ptr : static_cast<const void*>(storage_);
```

**view() fix:** Similarly updated to use the correct data pointer for external storage.

## New Tests Added

10 new tests in `google_tests/tests.cpp`:

| Test Name | Purpose |
|-----------|---------|
| `MemPool.BasicAllocation` | Basic allocation and alignment |
| `MemPool.AlignedAllocation` | Various alignment values |
| `MemPool.ZeroSizeAllocation` | Edge case: size=0 |
| `Messages.PoolBasedMessageConceptCheck` | Verify concept satisfaction |
| `Messages.PoolBasedMessageCreation` | Basic pool message creation |
| `Messages.PoolBasedMessageTypeCheck` | Type safety verification |
| `Messages.PoolBasedMessageDestruction` | Destructor invocation |
| `Messages.PoolBasedMessageMove` | Move constructor |
| `Messages.PoolBasedMessageMoveAssignment` | Move assignment |
| `Messages.PoolBasedLargePayload` | Payloads larger than SBO buffer |
| `Messages.PoolBasedMessageConsistency` | Many messages test |

## Design Decisions

**Why simple aligned_alloc wrapper?**
- Clean, correct baseline implementation
- Matches existing `MessageStorage::deallocate` signature (takes only `void*`)
- Can be upgraded to arena/free-list allocator later without API changes

**Why static deallocator lambda?**
- The `MessageStorage::deallocate` function pointer only takes `void*`, no pool reference
- Since we use `aligned_alloc`/`free`, the deallocator doesn't need the pool reference

## Files Modified

- `include/Pools/MemPool.h` - MemPool implementation
- `include/Messages/Message.inl` - Pool make(), bug fixes for move_from(), try_get(), view()
- `google_tests/tests.cpp` - New tests

## Test Results

All 15 tests pass:
- 4 original tests
- 11 new tests (3 MemPool + 8 Messages pool-based)
