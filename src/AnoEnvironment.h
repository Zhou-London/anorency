#pragma once

#include <cstdint>

#include "AnoEnvironmentInterface.h"

namespace Anorency {
class AnoEnvironment : public AnoEnvironmentInterface {
 public:
  AnoEnvironment();
  ~AnoEnvironment();

  uint32_t introduce_task() override;
  uint32_t join_all_tasks() override;
};
}  // namespace Anorency