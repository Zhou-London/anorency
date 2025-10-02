#pragma once

namespace Anorency {

class AnoEnvironmentInterface {
 public:
  virtual void introduce_task() = 0;
  virtual void join_all_tasks() = 0;
};

}  // namespace Anorency