#pragma once

#include <functional>

#include "AnoAgent.h"
#include "AnoType.h"
#include "Flags.h"

class AnoEnvironment {
 public:
  AnoEnvironment();
  ~AnoEnvironment();

  AgentID introduce(TaskNewThread);

  AgentID introduce(std::function<void(Flag::ThreadPool)>);

  AnoAgent& operator[](AgentID id);

 private:
  AgentsMap agents_;
};