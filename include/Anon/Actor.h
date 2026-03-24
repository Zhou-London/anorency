#pragma once

#include <functional>
#include <unordered_map>

#include "Anon/Alias.h"
#include "Anon/Message.h"
#include "Anon/Stream.h"

namespace Anon {
namespace detail {

class Actor {
 public:
  using HandlerFn = std::function<void(void*)>;

  Stream<MessageS> inbox;
  std::unordered_map<type_id_t, HandlerFn> handlers;
  bool pending_terminate = false;

  // Process one message from inbox. Returns true if processed.
  bool process_one();
};

}  // namespace detail
}  // namespace Anon

#include "Anon/Actor.inl"
