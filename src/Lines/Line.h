#pragma once

#include <algorithm>
#include <exception>
#include <queue>

#include "AnoException.h"
namespace Anorency {

template <typename T>
class Line {
 public:
  Line() = default;
  ~Line() = default;

  void push(const T& var) { queue_.push(var); }

  void push(T&& var) { queue_.emplace(std::move(var)); }

  T peek() {
    try {
      return queue_.front();
    } catch (std::exception& e) {
      throw AnoException(e.what());
    }
  }

  void pop() {
    try {
      queue_.pop();
    } catch (std::exception& e) {
      throw AnoException(e.what());
    }
  }

  size_t size() const noexcept { return queue_.size(); }

 private:
  std::queue<T> queue_;
};

}  // namespace Anorency