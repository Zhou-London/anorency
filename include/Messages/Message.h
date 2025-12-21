#pragma once

#include <cstddef>

#include "alias.h"

#define DEFAULT_MSG_SIZE 64

namespace Anorency {

struct MessageView {
  type_id_t type;
  const void* data;
};

template <std::size_t N = DEFAULT_MSG_SIZE,
          std::size_t Align = alignof(std::max_align_t)>
class Message {
 public:
  Message() noexcept = default;
  Message(Message&& other) noexcept;
  Message& operator=(Message&& other) noexcept;

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;

  ~Message() noexcept;

  struct MsgOps {
    void (*destroy)(void*) noexcept;
    void (*move_to)(void* src, void* dst) noexcept;
    type_id_t (*type)() noexcept;
  };

  bool empty() const noexcept;

  type_id_t type_id() const noexcept;

  MessageView view() const noexcept;

  template <class T>
  const T* try_get() const noexcept;

  template <class T, class... Args>
  static Message make(Args&&... args);

 private:
  alignas(Align) std::byte storage_[N];
  const MsgOps* ops_ = nullptr;

  void reset() noexcept;

  void move_from(Message& other) noexcept;

  template <class T>
  static const MsgOps& ops_for() noexcept;
};

}  // namespace Anorency

#include "Message.inl"
