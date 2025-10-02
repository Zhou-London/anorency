
#include "AnoTaskDefault.h"

#include <cstdint>
#include <functional>
#include <mutex>

namespace Anorency {

AnoTaskDefault::AnoTaskDefault()
    : thread_(), stand_by_(true), cv(), task_queue_(), mutex_() {
  thread_ = std::thread(&AnoTaskDefault::thread_loop, this);
}

AnoTaskDefault::~AnoTaskDefault() {}

uint32_t AnoTaskDefault::load(const std::function<void(void)>& task) {
  if (!stand_by_) return 1;

  task_queue_.push(task);
  cv.notify_one();
  return 0;
}

uint32_t AnoTaskDefault::load(std::function<void(void)>&& task) {
  if (!stand_by_) return 1;

  task_queue_.emplace(std::move(task));
  cv.notify_one();
  return 0;
}

uint32_t AnoTaskDefault::join() {
  stand_by_ = false;
  cv.notify_one();
  thread_.join();

  return 0;
}

void AnoTaskDefault::thread_loop() {
  while (true) {
    auto task = std::function<void(void)>();
    auto lock = std::unique_lock<std::mutex>(this->mutex_);

    cv.wait(lock, [this]() { return !stand_by_ || !task_queue_.empty(); });

    if (!stand_by_ && task_queue_.empty()) break;

    task = std::move(task_queue_.front());
    task_queue_.pop();

    task();
  }
}

}  // namespace Anorency