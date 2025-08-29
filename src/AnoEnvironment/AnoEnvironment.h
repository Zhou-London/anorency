#pragma once

#include "AnoAgent.h"
#include "Flags.h"
#include <functional>

#include "AnoType.h"

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