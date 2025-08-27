#pragma once

#include <sys/types.h>

#include <cstdint>
#include <functional>

#include "AnoInterface.h"

template <typename T>
class AnoThread {
 public:
  AnoThread<T>(std::function<T(AnoInterface&)>&& func, uint32_t id)
      : func_(func), id_(id) {}
  ~AnoThread() = default;

  void execute(AnoInterface& interface) { func_(interface); }
  const uint32_t id() const { return id_; }

 private:
  std::function<T(AnoInterface&)> func_;
  uint32_t id_;
};