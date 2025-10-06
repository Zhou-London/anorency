#pragma once

#include "IAnoEnvironment.h"

namespace Anorency {
class AnoEnvironment : public IAnoEnvironment {
 public:
  AnoEnvironment();
  ~AnoEnvironment();

  void introduce_task() override;
  void join_all_tasks() override;
};
}  // namespace Anorency