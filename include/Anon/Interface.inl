#pragma once

#include "Anon/Interface.h"

namespace Anon {

inline Interface::Interface(
    detail::Actor* actor,
    std::function<bool(Address, MessageS&&)> pub,
    std::function<void()> term)
    : actor_(actor),
      publish_fn_(std::move(pub)),
      terminate_fn_(std::move(term)) {}

inline void Interface::terminate() { terminate_fn_(); }

template <class T, class F>
void Interface::subscribe(F&& handler) {
  auto type = types::type_id<T>();

  // Bind the handler function to the type (message).
  actor_->handlers[type] =
      [h = std::forward<F>(handler)](void* data) mutable {
        h(*static_cast<T*>(data));
      };
}

template <class T, class... Args>
bool Interface::publish(Address target, Args&&... args) {
  auto msg = MessageS::make<T>(std::forward<Args>(args)...);
  return publish_fn_(target, std::move(msg));
}

}  // namespace Anon
