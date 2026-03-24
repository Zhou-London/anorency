#pragma once

#include "Anon/Stage.h"

namespace Anon {

template <typename D>
template <class SetupFn, class InitFn>
Address Stage<D>::introduce(SetupFn&& setup, InitFn&& init) {
  auto index = static_cast<uint32_t>(slots_.size());

  // Append an Actor Slot
  auto& slot = slots_.emplace_back();
  slot.actor = std::make_unique<detail::Actor>();
  slot.generation.store(1);
  slot.alive.store(true);
  slot.iface.reset(new Interface(
      slot.actor.get(),

      // Publish func
      [this](Address target, MessageS&& msg) {
        return deliver(target, std::move(msg));
      },
      // Terminate func
      [this, index]() {
        auto& slot = slots_[index];
        if (slot.alive.load()) {
          slot.alive.store(false);
          slot.generation.fetch_add(1);
          slot.actor->pending_terminate.store(true);
        }
      }));

  // Call setup func & Append init func
  setup(*slot.iface);
  pending_inits_.push_back(
      {index, std::forward<InitFn>(init)});

  dispatcher_.add_actor(slot.actor.get());
  return Address{index, slot.generation.load()};
}

template <typename D>
void Stage<D>::run() {
  for (auto& pi : pending_inits_) {
    if (slots_[pi.index].alive) {
      pi.fn(*slots_[pi.index].iface);
    }
  }
  pending_inits_.clear();

  dispatcher_.run();
}

template <typename D>
void Stage<D>::stop() {
  dispatcher_.stop();
}

template <typename D>
void Stage<D>::retire(Address addr) {
  if (addr.index >= slots_.size()) return;
  auto& slot = slots_[addr.index];
  if (slot.generation.load() != addr.generation) return;
  slot.alive.store(false);
  slot.generation.fetch_add(1);
  slot.actor->pending_terminate.store(true);
}

template <typename D>
std::jthread Stage<D>::launch(Stage& stage) {
  return std::jthread([&stage]() { stage.run(); });
}

template <typename D>
bool Stage<D>::deliver(Address target, MessageS&& msg) {
  if (target.index >= slots_.size()) return false;
  auto& slot = slots_[target.index];
  if (!slot.alive.load() ||
      slot.generation.load() != target.generation)
    return false;
  return slot.actor->inbox.try_write(std::move(msg));
}

}  // namespace Anon
