#include "AnoEnvironment.h"
#include "AnoType.h"
#include "Flags.h"
#include <functional>
#include <thread>

AnoEnvironment::AnoEnvironment() : threads_() {}

AnoEnvironment::~AnoEnvironment() { join_all_threads(); }

ThreadID AnoEnvironment::introduce(std::function<void(Flag::NewThread)> func) {

  const auto this_id = (ThreadID)threads_.size();
  auto j = std::thread(func, Flag::NewThread());
  threads_.emplace(this_id, std::move(j));

  return this_id;
}

ThreadID AnoEnvironment::introduce(std::function<void(Flag::ThreadPool)> func) {
  // TODO
  return 0;
}

void AnoEnvironment::join_all_threads() {

  for (auto it = threads_.begin(); it != threads_.end(); ++it) {
    if (it->second.joinable()) {
      it->second.join();
    }
  }
}