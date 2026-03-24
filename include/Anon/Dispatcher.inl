#pragma once

#include "Anon/Dispatcher.h"

namespace Anon {

inline void SingleThreadDispatcher::add_actor(detail::Actor* actor) {
  actors_.push_back(actor);
}

inline void SingleThreadDispatcher::remove_actor(detail::Actor* actor) {
  std::erase(actors_, actor);
}

inline void SingleThreadDispatcher::run() {
  while (!stopped_.load(std::memory_order_acquire)) {
    bool any_processed = false;
    for (auto* actor : actors_) {
      if (actor->process_one()) any_processed = true;
    }
    std::erase_if(actors_,
                  [](detail::Actor* a) { return a->pending_terminate; });
    if (!any_processed) std::this_thread::yield();
  }
}

inline void SingleThreadDispatcher::stop() {
  stopped_.store(true, std::memory_order_release);
}

}  // namespace Anon
