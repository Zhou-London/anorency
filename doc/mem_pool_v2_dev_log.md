# Memory Pool v2 - Bucket-Based Design

**Date:** 2026-01-22

## Problem

The original MemPool called `malloc` on every allocation. No actual pooling.

## First Principles

**What is a memory pool?**
1. Pre-allocate memory once
2. Hand out chunks from that memory
3. Recycle returned chunks

**What makes allocation slow?**
- System calls to OS
- Fragmentation searching

**Solution: Size-class buckets**
- Group allocations by size
- Each bucket: free list of same-sized blocks
- Allocation = pop from list (O(1))
- Deallocation = push to list (O(1))

## Design

```
Pool (1MB default)
├── Bucket 0 (16 bytes)  → free list
├── Bucket 1 (32 bytes)  → free list
├── Bucket 2 (64 bytes)  → free list
├── Bucket 3 (128 bytes) → free list
├── Bucket 4 (256 bytes) → free list
├── Bucket 5 (512 bytes) → free list
└── Bucket 6 (1024 bytes)→ free list
```

**Block layout:**
```
[bucket_idx: 8B][data_offset: 8B][DATA...]
                                 ^returned
```

Header stores:
- Which bucket (for dealloc routing)
- Data offset (for pointer math)

## Key Decisions

| Decision | Why |
|----------|-----|
| 16-byte header | Fits metadata, maintains 16-byte alignment |
| Power-of-2 buckets | Fast bucket lookup, good size coverage |
| Free list per bucket | O(1) alloc/dealloc, no fragmentation in bucket |
| System fallback | Handle edge cases (large size, high alignment) |

## API (unchanged)

```cpp
MemPool pool;                    // 1MB default
MemPool pool(4 * 1024 * 1024);   // 4MB custom

void* p = pool.allocate(size, align);
pool.deallocate(p);
```

## Test Results

All 15 tests pass. No API changes required.
