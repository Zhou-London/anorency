#pragma once

#include "Anon/Dispatcher.h"

namespace Anon {

inline void SingleThreadDispatcher::add_actor(detail::Actor* actor) {
  {
    std::lock_guard lock(mtx_);
    actors_.push_back(actor);
  }
  cv_.notify_one();
}

inline void SingleThreadDispatcher::remove_actor(detail::Actor* actor) {
  std::lock_guard lock(mtx_);
  std::erase(actors_, actor);
}

inline void SingleThreadDispatcher::run() {
  while (!stopped_.load(std::memory_order_acquire)) {
    bool any_processed = false;

    // Process actors
    for (auto* actor : actors_) {
      if (actor->process_one()) any_processed = true;
    }

    // Erase actors that are pending terminate
    std::erase_if(actors_, [](detail::Actor* a) {
      return a->pending_terminate.load(std::memory_order_acquire);
    });

    // Standby when no actors are alive
    if (actors_.empty()) {
      std::unique_lock lock(mtx_);
      cv_.wait(lock, [this] {
        return stopped_.load(std::memory_order_acquire) ||
               !actors_.empty();
      });
      continue;
    }

    if (!any_processed) std::this_thread::yield();
  }
}

inline void SingleThreadDispatcher::stop() {
  stopped_.store(true, std::memory_order_release);
  cv_.notify_one();
}

}  // namespace Anon
