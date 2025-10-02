
#include "AnoTaskDefault.h"

#include <functional>
#include <mutex>

namespace Anorency {

AnoTaskDefault::AnoTaskDefault()
    : thread_(), stand_by_(true), cv(), task_queue_(), mutex_() {
  thread_ = std::thread(&AnoTaskDefault::thread_loop, this);
}

AnoTaskDefault::~AnoTaskDefault() {}

void AnoTaskDefault::load(const std::function<void(void)>& task) {

  task_queue_.push(task);
  cv.notify_one();
}

void AnoTaskDefault::load(std::function<void(void)>&& task) {

  task_queue_.emplace(std::move(task));
  cv.notify_one();
}

void AnoTaskDefault::join() {
  stand_by_ = false;
  cv.notify_one();
  thread_.join();
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