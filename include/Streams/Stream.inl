#pragma once

#include <atomic>
#include <cstddef>

#include "Stream.h"
template <typename T, size_t Capacity>
Stream<T, Capacity>::Stream() : head_(0), tail_(0){};

template <typename T, size_t Capacity>
template <typename U>
  requires std::constructible_from<T, U&&>
bool Stream<T, Capacity>::try_write(U&& x) {
  if (tail_ - head_ == Capacity) return false;

  buffer_[tail_ & (Capacity - 1)] = x;
  ++tail_;

  return true;
}

template <typename T, size_t Capacity>
bool Stream<T, Capacity>::try_read(T& output) {
  if (tail_ == head_) return false;

  output = buffer_[head_ & (Capacity - 1)];
  ++head_;

  return true;
}

template <typename T, size_t Capacity>
size_t Stream<T, Capacity>::size() const noexcept {
  return tail_ - head_;
}
