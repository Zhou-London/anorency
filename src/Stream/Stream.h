#pragma once

#include <algorithm>
#include <exception>
#include <list>

#include "AnoException.h"
namespace Anorency {

template <typename T>
class Stream {
 public:
  Stream() = default;
  ~Stream() = default;

  // ! Deprecated function
  void push(const T& var) { list_.push_back(var); }

  void push(T&& var) { list_.emplace_back(std::move(var)); }

  T peek() const {
    try {
      return list_.front();
    } catch (std::exception& e) {
      throw AnoException(e.what());
    }
  }

  void pop() {
    try {
      list_.pop_front();
    } catch (std::exception& e) {
      throw AnoException(e.what());
    }
  }
  // !

  void write(const T& var) { list_.push_back(var); }

  void write(T&& var) { list_.emplace_back(std::move(var)); }

  T read() {
    try {
      auto it = list_.front();
      list_.pop_front();
      return it;
    } catch (std::exception& e) {
      throw AnoException(e.what());
    }
  }

  size_t size() const noexcept { return list_.size(); }
  bool empty() const noexcept { return list_.empty(); }

  auto begin() const noexcept { return list_.begin(); }
  auto end() const noexcept { return list_.end(); }

 private:
  std::list<T> list_;
};

}  // namespace Anorency