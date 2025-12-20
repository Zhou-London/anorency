#include "Stream.h"

#include <cstddef>

Stream::Stream() : head_(0), tail_(0) {};

bool Stream::try_write(int x) {
  if (tail_ - head_ == STREAM_BUFFER_CAPACITY) return false;

  buffer_[tail_ & (STREAM_BUFFER_CAPACITY - 1)] = x;
  ++tail_;

  return true;
}

bool Stream::try_read(int& output) {
  if (tail_ == head_) return false;

  output = buffer_[head_++ & (STREAM_BUFFER_CAPACITY - 1)];

  return true;
}

size_t Stream::size() const noexcept { return tail_ - head_; }
