#pragma once

#include <cstdint>
#include <functional>

namespace Anorency {
class AnoTaskInterface {
 public:
  virtual uint32_t load(const std::function<void(void)>& task) = 0;
  virtual uint32_t load(std::function<void(void)>&& task) = 0;

  virtual uint32_t join() = 0;
};
}  // namespace Anorency