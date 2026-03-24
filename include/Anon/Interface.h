#pragma once

#include <functional>

#include "Anon/Actor.h"
#include "Anon/Address.h"
#include "Anon/Message.h"
#include "Anon/Types.h"

namespace Anon {

class Interface {
 public:
  Interface();
  ~Interface() = default;

  // * T = Message Type, F = Handler function
  template <class T, class F>
  void subscribe(F&& handler);

  template <class T, class... Args>
  bool publish(Address target, Args&&... args);

  void terminate();

 private:
  template <typename D>
  friend class Stage;

  detail::Actor* actor_;
  std::function<bool(Address, MessageS&&)> publish_fn_;
  std::function<void()> terminate_fn_;

  Interface(detail::Actor* actor, std::function<bool(Address, MessageS&&)> pub,
            std::function<void()> term);
};

}  // namespace Anon

#include "Anon/Interface.inl"
