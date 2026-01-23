#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>

namespace Anorency {

/**
 * Memory pool with pre-allocated memory and size-class buckets.
 *
 * Design:
 * - Pre-allocates a contiguous memory block upfront
 * - Uses size-class buckets (16, 32, 64, 128, 256, 512, 1024 bytes)
 * - Each bucket maintains a free list for O(1) reuse
 * - Falls back to system allocator when pool is exhausted or size/align too large
 *
 * Block layout:
 * [bucket_idx: 8 bytes][data_offset: 8 bytes][padding...][DATA]
 *                                                        ^returned pointer
 */
class MemPool {
 public:
  static constexpr std::size_t DEFAULT_POOL_SIZE = 1024 * 1024;  // 1 MB
  static constexpr std::size_t NUM_BUCKETS = 7;
  static constexpr std::size_t HEADER_SIZE = 16;       // bucket_idx + data_offset
  static constexpr std::size_t MAX_POOLED_SIZE = 1024; // Largest bucket usable size

  explicit MemPool(std::size_t pool_size = DEFAULT_POOL_SIZE) noexcept;
  ~MemPool() noexcept;

  MemPool(const MemPool&) = delete;
  MemPool& operator=(const MemPool&) = delete;
  MemPool(MemPool&&) = delete;
  MemPool& operator=(MemPool&&) = delete;

  void* allocate(std::size_t size, std::size_t align) noexcept;
  void deallocate(void* ptr) noexcept;

 private:
  struct FreeBlock {
    FreeBlock* next;
  };

  static constexpr std::size_t LARGE_ALLOC_MARKER = ~std::size_t(0);

  std::byte* pool_;
  std::size_t pool_size_;
  std::size_t pool_offset_;
  std::array<FreeBlock*, NUM_BUCKETS> free_lists_;

  // Bucket usable sizes: 16, 32, 64, 128, 256, 512, 1024
  static constexpr std::size_t bucket_for_size(std::size_t size) noexcept {
    if (size <= 16) return 0;
    if (size <= 32) return 1;
    if (size <= 64) return 2;
    if (size <= 128) return 3;
    if (size <= 256) return 4;
    if (size <= 512) return 5;
    if (size <= 1024) return 6;
    return NUM_BUCKETS;
  }

  static constexpr std::size_t bucket_usable_size(std::size_t idx) noexcept {
    constexpr std::size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024};
    return sizes[idx];
  }

  // Total block size = header + usable
  static constexpr std::size_t bucket_block_size(std::size_t idx) noexcept {
    return HEADER_SIZE + bucket_usable_size(idx);
  }
};

// === Implementation ===

inline MemPool::MemPool(std::size_t pool_size) noexcept
    : pool_(nullptr), pool_size_(pool_size), pool_offset_(0) {
  free_lists_.fill(nullptr);
  if (pool_size > 0) {
    pool_ = static_cast<std::byte*>(std::aligned_alloc(HEADER_SIZE, pool_size));
  }
}

inline MemPool::~MemPool() noexcept {
  std::free(pool_);
}

inline void* MemPool::allocate(std::size_t size, std::size_t align) noexcept {
  if (size == 0) return nullptr;

  // Calculate data offset: must fit header (16 bytes) and align data
  std::size_t data_offset = HEADER_SIZE;
  if (align > HEADER_SIZE) {
    data_offset = align;  // e.g., align=32 -> offset=32
  }

  std::size_t idx = bucket_for_size(size);

  // Use pool only for standard alignment (offset=16) and size within buckets
  bool use_pool = (idx < NUM_BUCKETS) && (data_offset == HEADER_SIZE);

  if (use_pool) {
    // Try free list first (O(1) reuse)
    if (free_lists_[idx] != nullptr) {
      FreeBlock* block = free_lists_[idx];
      free_lists_[idx] = block->next;

      std::byte* block_ptr = reinterpret_cast<std::byte*>(block);
      std::size_t* header = reinterpret_cast<std::size_t*>(block_ptr);
      header[0] = idx;          // bucket index
      header[1] = data_offset;  // offset to data

      return block_ptr + data_offset;
    }

    // Allocate from pool
    std::size_t block_size = bucket_block_size(idx);
    std::size_t aligned_offset =
        (pool_offset_ + HEADER_SIZE - 1) & ~(HEADER_SIZE - 1);

    if (pool_ && aligned_offset + block_size <= pool_size_) {
      std::byte* block_ptr = pool_ + aligned_offset;
      pool_offset_ = aligned_offset + block_size;

      std::size_t* header = reinterpret_cast<std::size_t*>(block_ptr);
      header[0] = idx;
      header[1] = data_offset;

      return block_ptr + data_offset;
    }
  }

  // Fallback: system allocator for large/high-align allocations
  std::size_t actual_align = align > sizeof(void*) ? align : sizeof(void*);
  if (actual_align < HEADER_SIZE) actual_align = HEADER_SIZE;

  std::size_t total_size = data_offset + size;
  total_size = (total_size + actual_align - 1) & ~(actual_align - 1);

  void* block = std::aligned_alloc(actual_align, total_size);
  if (!block) return nullptr;

  std::size_t* header = reinterpret_cast<std::size_t*>(block);
  header[0] = LARGE_ALLOC_MARKER;
  header[1] = data_offset;

  return static_cast<std::byte*>(block) + data_offset;
}

inline void MemPool::deallocate(void* ptr) noexcept {
  if (!ptr) return;

  std::byte* data = static_cast<std::byte*>(ptr);

  // Read data_offset from right before data (at offset -8 from data, which is header[1])
  // Actually header[1] is at block + 8, and data is at block + data_offset
  // So we read from (data - data_offset + 8) which requires knowing data_offset first
  // Solution: For standard offset (16), header[1] is at data - 8

  // First assume standard offset to read the actual offset
  std::size_t data_offset = *reinterpret_cast<std::size_t*>(data - 8);
  std::byte* block = data - data_offset;
  std::size_t idx = *reinterpret_cast<std::size_t*>(block);

  if (idx == LARGE_ALLOC_MARKER) {
    std::free(block);
    return;
  }

  // Check if block is within our pool
  if (pool_ && block >= pool_ && block < pool_ + pool_size_) {
    // Return to free list for reuse
    FreeBlock* fb = reinterpret_cast<FreeBlock*>(block);
    fb->next = free_lists_[idx];
    free_lists_[idx] = fb;
  } else {
    // External fallback allocation
    std::free(block);
  }
}

}  // namespace Anorency
