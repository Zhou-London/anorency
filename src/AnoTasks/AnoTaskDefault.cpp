
#include "AnoTaskDefault.h"

#include <functional>
#include <iostream>
#include <mutex>

#include "AnoException.h"

namespace Anorency {

AnoTaskDefault::AnoTaskDefault()
    : thread_(), stand_by_(true), cv(), task_queue_(), mutex_() {
  thread_ = std::thread(&AnoTaskDefault::thread_loop, this);
}

AnoTaskDefault::~AnoTaskDefault() {
  if (this->thread_.joinable()) {
    this->join();
  }
}

void AnoTaskDefault::load(const std::function<void(void)>& task) {
  if (!stand_by_) throw AnoException("Thread is not stand by.");

  task_queue_.push(task);
  cv.notify_one();
}

void AnoTaskDefault::load(std::function<void(void)>&& task) {
  if (!stand_by_) throw AnoException("Thread is not stand by.");

  task_queue_.emplace(std::move(task));
  cv.notify_one();
}

void AnoTaskDefault::join() {
  if (!stand_by_) throw AnoException("Thread is joined more than once.");

  stand_by_ = false;
  cv.notify_one();
  thread_.join();
}

void AnoTaskDefault::thread_loop() {
  try {
    while (true) {
      auto task = std::function<void(void)>();
      auto lock = std::unique_lock<std::mutex>(this->mutex_);

      cv.wait(lock, [this]() { return !stand_by_ || !task_queue_.empty(); });

      if (!stand_by_ && task_queue_.empty()) break;

      task = std::move(task_queue_.front());
      task_queue_.pop();

      task();
    }
  } catch (const AnoException& ae){
    std::cerr << "AnoException caught: "<<ae.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "std::exception caught: "<<e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }

}

}  // namespace Anorency