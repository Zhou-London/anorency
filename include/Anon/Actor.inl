#pragma once

#include "Anon/Actor.h"

namespace Anon {
namespace detail {

inline bool Actor::process_one() {
  MessageS msg;
  if (!inbox.try_read(msg)) return false;
  if (msg.empty()) return true;

  auto it = handlers.find(msg.type_id());
  if (it != handlers.end()) {
    auto view = msg.view();
    it->second(const_cast<void*>(view.data));
  }
  return true;
}

}  // namespace detail
}  // namespace Anon
