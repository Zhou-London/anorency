#pragma once

#include <cstdint>

namespace Anorency {

class AnoEnvironmentInterface {
 public:
  virtual uint32_t introduce_task() = 0;
  virtual uint32_t join_all_tasks() = 0;
};

}  // namespace Anorency