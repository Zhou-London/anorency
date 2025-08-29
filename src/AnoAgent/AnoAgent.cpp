#include "AnoAgent.h"

#include <condition_variable>
#include <functional>
#include <mutex>

#include "Flags.h"


AnoAgent::AnoAgent()
    : stand_by_(true),

      mutex_(),

      cv_(),

      task_queue_() {
  thread_ = std::thread(&AnoAgent::thread_loop, this);
}

AnoAgent::~AnoAgent() { this->stop(); }

void AnoAgent::thread_loop() {
  while (true) {
    std::function<void(Flag::NewThread)> func;
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this] { return !stand_by_ || !task_queue_.empty(); });

    if (!stand_by_ && task_queue_.empty()) break;

    func = std::move(task_queue_.front());
    task_queue_.pop();

    func(Flag::NewThread());
  }
}

void AnoAgent::register_task(std::function<void(Flag::NewThread)> func) {
  task_queue_.emplace(std::move(func));

  cv_.notify_one();
}

void AnoAgent::stop() {
  stand_by_ = false;

  cv_.notify_one();

  thread_.join();
}