#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>

namespace Anorency {

class MemPool {
 public:
  static constexpr std::size_t DEFAULT_POOL_SIZE = 1024 * 1024;
  static constexpr std::size_t NUM_BUCKETS = 7;
  static constexpr std::size_t HEADER_SIZE = 16;
  static constexpr std::size_t MAX_POOLED_SIZE = 1024;

  explicit MemPool(std::size_t pool_size = DEFAULT_POOL_SIZE) noexcept;
  ~MemPool() noexcept;

  MemPool(const MemPool&) = delete;
  MemPool& operator=(const MemPool&) = delete;
  MemPool(MemPool&&) = delete;
  MemPool& operator=(MemPool&&) = delete;

  void* allocate(std::size_t size, std::size_t align) noexcept;
  void deallocate(void* ptr) noexcept;

  // Type & Alias
 private:
  struct FreeBlock {
    FreeBlock* next;
  };

  using FreeBlockPtr = FreeBlock*;
  using BusyBlockPtr = std::byte*;
  using bucket_index_t = std::size_t;
  using offset_t = std::size_t;

  static FreeBlockPtr as_free_block(BusyBlockPtr ptr) noexcept {
    return reinterpret_cast<FreeBlockPtr>(ptr);
  }

  static BusyBlockPtr as_busy_block(FreeBlockPtr block) noexcept {
    return reinterpret_cast<BusyBlockPtr>(block);
  }

  static void fill_header(BusyBlockPtr block, bucket_index_t idx,
                          offset_t offset) noexcept {
    *reinterpret_cast<bucket_index_t*>(block) = idx;
    *reinterpret_cast<offset_t*>(block + offset - sizeof(offset_t)) = offset;
  }

 private:
  static constexpr std::size_t LARGE_ALLOC_MARKER = ~std::size_t(0);

  std::byte* pool_;
  std::size_t pool_size_;
  offset_t pool_offset_;
  std::array<FreeBlock*, NUM_BUCKETS> free_lists_;

  static constexpr bucket_index_t bucket_for_size(std::size_t size) noexcept {
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

inline MemPool::~MemPool() noexcept { std::free(pool_); }

inline void* MemPool::allocate(std::size_t size, std::size_t align) noexcept {
  if (size == 0) return nullptr;

  std::size_t data_offset = align > HEADER_SIZE ? align : HEADER_SIZE;

  std::size_t idx = bucket_for_size(size);
  bool use_pool = (idx < NUM_BUCKETS) && (data_offset == HEADER_SIZE);

  if (use_pool) {
    // Allocate from free list
    if (free_lists_[idx] != nullptr) {
      FreeBlock* block = free_lists_[idx];
      free_lists_[idx] = block->next;

      std::byte* block_ptr = reinterpret_cast<std::byte*>(block);
      *reinterpret_cast<std::size_t*>(block_ptr) = idx;
      *reinterpret_cast<std::size_t*>(block_ptr + data_offset -
                                      sizeof(std::size_t)) = data_offset;

      return block_ptr + data_offset;
    }

    // Allocate from pool
    std::size_t block_size = bucket_block_size(idx);
    std::size_t aligned_offset =
        (pool_offset_ + HEADER_SIZE - 1) & ~(HEADER_SIZE - 1);

    if (pool_ && aligned_offset + block_size <= pool_size_) {
      std::byte* block_ptr = pool_ + aligned_offset;
      pool_offset_ = aligned_offset + block_size;

      *reinterpret_cast<std::size_t*>(block_ptr) = idx;
      *reinterpret_cast<std::size_t*>(block_ptr + data_offset -
                                      sizeof(std::size_t)) = data_offset;

      return block_ptr + data_offset;
    }
  }

  // Allocate from system.
  std::size_t actual_align = align > sizeof(void*) ? align : sizeof(void*);
  if (actual_align < HEADER_SIZE) actual_align = HEADER_SIZE;

  std::size_t total_size = data_offset + size;
  total_size = (total_size + actual_align - 1) & ~(actual_align - 1);

  void* block = std::aligned_alloc(actual_align, total_size);
  if (!block) return nullptr;

  std::byte* block_ptr = static_cast<std::byte*>(block);

  *reinterpret_cast<std::size_t*>(block_ptr) = LARGE_ALLOC_MARKER;
  *reinterpret_cast<std::size_t*>(block_ptr + data_offset -
                                  sizeof(std::size_t)) = data_offset;

  return block_ptr + data_offset;
}

inline void MemPool::deallocate(void* ptr) noexcept {
  if (!ptr) return;

  std::byte* data = static_cast<std::byte*>(ptr);

  std::size_t data_offset = *reinterpret_cast<std::size_t*>(data - 8);
  std::byte* block = data - data_offset;
  std::size_t idx = *reinterpret_cast<std::size_t*>(block);

  if (idx == LARGE_ALLOC_MARKER) {
    std::free(block);
    return;
  }

  if (pool_ && block >= pool_ && block < pool_ + pool_size_) {
    FreeBlock* fb = reinterpret_cast<FreeBlock*>(block);
    fb->next = free_lists_[idx];
    free_lists_[idx] = fb;
  } else {
    // External fallback allocation
    std::free(block);
  }
}

}  // namespace Anorency
