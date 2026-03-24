#pragma once

#include "Anon/MemPool.h"

namespace Anon {

inline MemPool::MemPool(std::size_t pool_size) noexcept
    : pool_(nullptr), pool_size_(pool_size), pool_offset_(0) {
  free_lists_.fill(nullptr);
  if (pool_size > 0) {
    pool_ = static_cast<std::byte*>(std::aligned_alloc(HEADER_SIZE, pool_size));
  }
}

inline MemPool::~MemPool() noexcept { std::free(pool_); }

inline void* MemPool::allocate(std::size_t size,
                               std::size_t align) noexcept {
  if (size == 0) return nullptr;

  offset_t data_offset = align > HEADER_SIZE ? align : HEADER_SIZE;

  auto idx = bucket_for_size(size);
  bool use_pool = (idx < NUM_BUCKETS) && (data_offset == HEADER_SIZE);

  if (use_pool) {
    // Allocate from free list
    if (free_lists_[idx] != nullptr) {
      auto block = free_lists_[idx];
      free_lists_[idx] = block->next;

      fill_header(as_busy_block(block), idx, data_offset);

      return as_busy_block(block) + data_offset;
    }

    // Allocate from pool
    auto block_size = bucket_block_size(idx);
    offset_t aligned_offset =
        (pool_offset_ + HEADER_SIZE - 1) & ~(HEADER_SIZE - 1);

    if (pool_ && aligned_offset + block_size <= pool_size_) {
      BusyBlockPtr block_ptr = pool_ + aligned_offset;
      pool_offset_ = aligned_offset + block_size;

      fill_header(block_ptr, idx, data_offset);

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

  auto block_ptr = static_cast<BusyBlockPtr>(block);

  fill_header(block_ptr, LARGE_ALLOC_MARKER, data_offset);

  return block_ptr + data_offset;
}

inline void MemPool::deallocate(void* ptr) noexcept {
  if (!ptr) return;

  auto data = static_cast<BusyBlockPtr>(ptr);

  auto data_offset = *reinterpret_cast<offset_t*>(data - sizeof(offset_t));
  BusyBlockPtr block = data - data_offset;
  auto idx = *reinterpret_cast<bucket_index_t*>(block);

  if (idx == LARGE_ALLOC_MARKER) {
    std::free(block);
    return;
  }

  if (pool_ && block >= pool_ && block < pool_ + pool_size_) {
    auto fb = as_free_block(block);
    fb->next = free_lists_[idx];
    free_lists_[idx] = fb;
  } else {
    // External fallback allocation
    std::free(block);
  }
}

}  // namespace Anon
