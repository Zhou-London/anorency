#pragma once

#include <cstddef>
#include <cstdlib>

namespace Anorency {

class MemPool {
 public:
  MemPool() = default;
  ~MemPool() = default;

  MemPool(const MemPool&) = delete;
  MemPool& operator=(const MemPool&) = delete;
  MemPool(MemPool&&) = delete;
  MemPool& operator=(MemPool&&) = delete;

  void* allocate(std::size_t size, std::size_t align) noexcept {
    if (size == 0) return nullptr;
    // aligned_alloc requires alignment >= sizeof(void*) on some platforms
    std::size_t min_align = sizeof(void*);
    std::size_t actual_align = align > min_align ? align : min_align;
    // aligned_alloc requires size to be multiple of alignment
    std::size_t adjusted_size = ((size + actual_align - 1) / actual_align) * actual_align;
    return std::aligned_alloc(actual_align, adjusted_size);
  }

  void deallocate(void* ptr) noexcept { std::free(ptr); }

 private:
};

}  // namespace Anorency
