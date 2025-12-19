#include "Stream.h"

#include <cstddef>

Stream::Stream() : head_(0), tail_(0) {};

void Stream::write(int num) {
  while (true) {
    if (tail_ - head_ < STREAM_BUFFER_CAPACITY) {
      buffer_[tail_ & (STREAM_BUFFER_CAPACITY - 1)] = num;
      ++tail_;
      return;
    }
  }
}

int Stream::read() {
  while (true) {
    if (tail_ > head_) {
      return buffer_[head_++ & (STREAM_BUFFER_CAPACITY - 1)];
    }
  }
}

size_t Stream::size() const noexcept { return tail_ - head_; }
