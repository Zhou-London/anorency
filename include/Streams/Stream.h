#pragma once
#include <atomic>
#include <cstddef>
#define STREAM_BUFFER_CAPACITY 128

#include <array>
#include <cstdint>

class Stream {
 public:
  Stream();
  ~Stream() = default;

  bool try_write(int x);
  bool try_read(int& output);

  size_t size() const noexcept;

 private:
  alignas(64) std::atomic<uint32_t> head_;
  alignas(64) std::atomic<uint32_t> tail_;

  alignas(64) std::array<int, STREAM_BUFFER_CAPACITY> buffer_;
};
