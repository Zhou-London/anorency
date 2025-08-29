#include "AnoEnvironment.h"
#include "AnoType.h"
#include "Flags.h"
#include <functional>

AnoEnvironment::AnoEnvironment() : agents_() {}

AnoEnvironment::~AnoEnvironment() {

  // * RAII Destructs agents_map
  // * RAII Desutrcts each agent
}

AgentID AnoEnvironment::introduce(TaskNewThread func) {

  const auto this_id = (AgentID)agents_.size();

  agents_[this_id].register_task(func);

  return this_id;
}

AgentID AnoEnvironment::introduce(std::function<void(Flag::ThreadPool)> func) {
  // TODO
  return 0;
}

AnoAgent &AnoEnvironment::operator[](AgentID id) { return agents_.at(id); }