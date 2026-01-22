#pragma once

#include <cstddef>
namespace Anorency {
class MemPool {
 public:
  MemPool() = default;
  ~MemPool() = default;

  void* allocate(std::size_t size, std::size_t align) noexcept;
  void deallocate(void* ptr) noexcept;

 private:
};

}  // namespace Anorency
