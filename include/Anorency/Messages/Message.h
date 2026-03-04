#pragma once

#include <cstddef>
#include <type_traits>

#include "alias.h"
#include "mem_pool_wrapper.h"

#define DEFAULT_MSG_SIZE 64

namespace Anorency {

struct MessageView {
  type_id_t type;
  const void* data;
};

struct MessageStorage {
  void* ptr;
  void* pool;  // type-erased pool pointer (nullptr if not pool-allocated)
  std::size_t size;
  std::size_t align;

  void (*deallocate)(void* pool, void* ptr) noexcept;
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
    requires std::is_nothrow_move_constructible_v<T>
  const T* try_get() const noexcept;

  template <class T, class... Args>
  static Message make(Args&&... args);

  template <class T, class Pool, class... Args>
    requires std::is_nothrow_move_constructible_v<T> &&
             interfaces::mem_pool_wrapper<Pool>
  static Message make(Pool& pool, Args&&... args);

 private:
  union {
    alignas(Align) std::byte storage_[N];
    MessageStorage external_;
  };

  const MsgOps* ops_ = nullptr;
  bool use_external_storage_ = false;

  void reset() noexcept;

  void move_from(Message& other) noexcept;

  template <class T>
  static const MsgOps& ops_for() noexcept;
};

}  // namespace Anorency

#include "Message.inl"
