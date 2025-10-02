#pragma once

#include <functional>

namespace Anorency {
class AnoTaskInterface {
 public:
  virtual void load(const std::function<void(void)>& task) = 0;
  virtual void load(std::function<void(void)>&& task) = 0;

  virtual void join() = 0;
};
}  // namespace Anorency