#pragma once

#include <algorithm>
#include <atomic>
#include <concepts>
#include <thread>
#include <vector>

#include "Anon/Actor.h"

namespace Anon {

template <typename D>
concept dispatcher = requires(D d, detail::Actor* a) {
  { d.add_actor(a) };
  { d.remove_actor(a) };
  { d.run() };
  { d.stop() };
};

class SingleThreadDispatcher {
 public:
  void add_actor(detail::Actor* actor);
  void remove_actor(detail::Actor* actor);
  void run();
  void stop();

 private:
  std::vector<detail::Actor*> actors_;
  std::atomic<bool> stopped_{false};
};

}  // namespace Anon

#include "Anon/Dispatcher.inl"
