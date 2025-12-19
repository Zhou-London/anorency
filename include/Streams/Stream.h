#pragma once
#include <cstddef>
#define STREAM_BUFFER_CAPACITY 128

#include <array>
#include <cstdint>

class Stream {
 public:
  Stream();
  ~Stream() = default;

  void write(int num);
  int read();

  size_t size() const noexcept;

 private:
  uint32_t head_;
  uint32_t tail_;

  std::array<int, STREAM_BUFFER_CAPACITY> buffer_;
};
