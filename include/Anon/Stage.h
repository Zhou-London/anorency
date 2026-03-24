#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
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

  // Take an actor offline by its address.
  void retire(Address addr);

  // Launch the stage on a dedicated thread.
  // Returns std::jthread; call stop() to terminate.
  static std::jthread launch(Stage& stage);

 private:
  struct ActorSlot {
    std::unique_ptr<detail::Actor> actor;
    std::unique_ptr<Interface> iface;

    // Used to check expired (atomic for cross-thread retire)
    std::atomic<uint32_t> generation{0};
    std::atomic<bool> alive{false};

    ActorSlot() = default;
    ActorSlot(ActorSlot&& o) noexcept
        : actor(std::move(o.actor)),
          iface(std::move(o.iface)),
          generation(o.generation.load()),
          alive(o.alive.load()) {}
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
