#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>

namespace Anon {

template <typename T>
concept mem_pool_wrapper =
    requires(T s, std::size_t size, std::size_t align, void* ptr) {
      { s.allocate(size, align) } noexcept -> std::same_as<void*>;
      { s.deallocate(ptr) } noexcept -> std::same_as<void>;
    };

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

  static void fill_header(BusyBlockPtr block, bucket_index_t index,
                          offset_t offset) noexcept {
    *reinterpret_cast<bucket_index_t*>(block) = index;
    *reinterpret_cast<offset_t*>(block + offset - sizeof(offset_t)) = offset;
  }

 private:
  static constexpr std::size_t LARGE_ALLOC_MARKER = ~std::size_t(0);

  std::byte* pool_;
  std::size_t pool_size_;
  offset_t pool_offset_;
  std::array<FreeBlockPtr, NUM_BUCKETS> free_lists_;

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

}  // namespace Anon

#include "Anon/MemPool.inl"
