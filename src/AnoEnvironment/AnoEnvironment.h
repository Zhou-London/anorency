#pragma once

#include "Flags.h"
#include <functional>

#include "AnoType.h"

class AnoEnvironment {

public:
  AnoEnvironment();
  ~AnoEnvironment();

  ThreadID introduce(std::function<void(Flag::NewThread)>);

  ThreadID introduce(std::function<void(Flag::ThreadPool)>);

  void join_all_threads();

private:
  ThreadsMap threads_;
};