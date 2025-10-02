#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <queue>
#include <thread>

#include "AnoTaskInterface.h"

namespace Anorency {

class AnoTaskDefault : public AnoTaskInterface {
 public:
  AnoTaskDefault();
  ~AnoTaskDefault();

  uint32_t load(const std::function<void(void)>& task) override;
  uint32_t load(std::function<void(void)>&& task) override;
  uint32_t join() override;

 private:
  std::thread thread_;
  bool stand_by_;
  std::condition_variable cv;
  std::queue<std::function<void(void)>> task_queue_;
  std::mutex mutex_;

  void thread_loop();
};
}  // namespace Anorency