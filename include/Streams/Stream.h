#pragma once
#include <atomic>
#include <concepts>
#include <cstddef>
#define DEFAULT_STREAM_CAPACITY 2048

#include <array>
#include <cstdint>

namespace Anorency {
template <typename T, size_t Capacity = DEFAULT_STREAM_CAPACITY>
class Stream {
 public:
  Stream();
  ~Stream() = default;

  template <typename U>
    requires std::constructible_from<T, U&&>
  bool try_write(U&& x);

  bool try_read(T& output);

  size_t size() const noexcept;

 private:
  alignas(64) std::atomic<uint32_t> head_;
  alignas(64) std::atomic<uint32_t> tail_;

  alignas(64) std::array<T, Capacity> buffer_;
};
}  // namespace Anorency
#include "Stream.inl"
