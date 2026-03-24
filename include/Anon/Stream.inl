#pragma once

#include <atomic>
#include <cstddef>

#include "Anon/Stream.h"

namespace Anon {
template <typename T, size_t Capacity>
Stream<T, Capacity>::Stream() : head_(0), tail_(0){};

template <typename T, size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U&&>
bool Stream<T, Capacity>::try_write(U&& x) {
  if (tail_ - head_ == Capacity) return false;

  buffer_[tail_ & (Capacity - 1)] = std::forward<U>(x);
  ++tail_;

  return true;
}

template <typename T, size_t Capacity>
bool Stream<T, Capacity>::try_read(T& output) {
  if (tail_ == head_) return false;

  output = std::move(buffer_[head_ & (Capacity - 1)]);
  ++head_;

  return true;
}

template <typename T, size_t Capacity>
size_t Stream<T, Capacity>::size() const noexcept {
  return tail_ - head_;
}
}  // namespace Anon
