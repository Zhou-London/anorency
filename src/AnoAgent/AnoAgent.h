#pragma once

#include "Flags.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
class AnoAgent {
public:
  AnoAgent();
  ~AnoAgent();

  void register_task(std::function<void(Flag::NewThread)>);
  void stop();

private:
  bool stand_by_;
  std::mutex mutex_;
  std::condition_variable cv_;

  std::thread thread_;
  std::queue<std::function<void(Flag::NewThread)>> task_queue_;

  void thread_loop();
};