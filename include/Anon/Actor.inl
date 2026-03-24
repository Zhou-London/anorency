#pragma once

#include "Anon/Actor.h"

namespace Anon {
namespace detail {

inline bool Actor::process_one() {

  // Read inbox
  MessageS msg;
  if (!inbox.try_read(msg)) return false;
  if (msg.empty()) return true;

  // Call handlers
  auto it = handlers.find(msg.type_id());
  if (it != handlers.end()) {
    void* data = const_cast<void*>(msg.view().data);
    it->second(data);
  }
  return true;
}

}  // namespace detail
}  // namespace Anon
