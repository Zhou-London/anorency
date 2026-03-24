#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "Anon/Actor.h"
#include "Anon/Address.h"
#include "Anon/Dispatcher.h"
#include "Anon/Interface.h"

namespace Anon {

template <typename D>
class Stage {
  static_assert(dispatcher<D>,
                "D must satisfy the dispatcher concept");

 public:
  Stage() = default;

  template <class SetupFn, class InitFn>
  Address introduce(SetupFn&& setup, InitFn&& init);

  void run();
  void stop();

 private:
  struct ActorSlot {
    std::unique_ptr<detail::Actor> actor;
    std::unique_ptr<Interface> iface;

    // Used to check expired
    uint32_t generation = 0;
    bool alive = false;
  };

  // Init function
  struct PendingInit {
    uint32_t index;
    std::function<void(Interface&)> fn;
  };

  std::vector<ActorSlot> slots_;
  std::vector<PendingInit> pending_inits_;
  D dispatcher_;

  // Deliver the published message
  bool deliver(Address target, MessageS&& msg);
};

}  // namespace Anon

#include "Anon/Stage.inl"
