#pragma once

#include "AnoEnvironmentInterface.h"

namespace Anorency {
class AnoEnvironment : public AnoEnvironmentInterface {
 public:
  AnoEnvironment();
  ~AnoEnvironment();

  void introduce_task() override;
  void join_all_tasks() override;
};
}  // namespace Anorency