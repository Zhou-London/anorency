#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "Message.h"
#include "types.h"

namespace Anorency {

template <std::size_t N, std::size_t Align>
Message<N, Align>::Message(Message<N, Align>&& other) noexcept {
  move_from(other);
}

template <std::size_t N, std::size_t Align>
Message<N, Align>& Message<N, Align>::operator=(
    Message<N, Align>&& other) noexcept {
  if (this != &other) {
    reset();
    move_from(other);
  }

  return *this;
}

template <std::size_t N, std::size_t Align>
Message<N, Align>::~Message() noexcept {
  reset();
}

template <std::size_t N, std::size_t Align>
bool Message<N, Align>::empty() const noexcept {
  return ops_ == nullptr;
}

template <std::size_t N, std::size_t Align>
type_id_t Message<N, Align>::type_id() const noexcept {
  return ops_ ? ops_->type() : nullptr;
}

template <std::size_t N, std::size_t Align>
MessageView Message<N, Align>::view() const noexcept {
  return MessageView{ops_ ? ops_->type() : nullptr,
                     ops_ ? static_cast<const void*>(storage_) : nullptr};
}

template <std::size_t N, std::size_t Align>
template <class T>
const T* Message<N, Align>::try_get() const noexcept {
  if (!ops_ || ops_->type() != Anorency::types::type_id<T>()) return nullptr;
  return std::launder(reinterpret_cast<const T*>(storage_));
}

template <std::size_t N, std::size_t Align>
template <class T, class... Args>
Message<N, Align> Message<N, Align>::make(Args&&... args) {
  static_assert(sizeof(T) <= N, "payload too large");
  static_assert(alignof(T) <= Align, "payload alignment too large");
  static_assert(std::is_nothrow_constructible_v<T>, "payload lack of CTRs");
  static_assert(std::is_nothrow_destructible_v<T>, "payload lack of DTRs");

  // WARN: new
  Message m;
  new (m.storage_) T(std::forward<Args>(args)...);

  m.ops_ = &ops_for<T>();
  return m;
}

template <std::size_t N, std::size_t Align>
void Message<N, Align>::reset() noexcept {
  if (ops_) {
    ops_->destroy(storage_);
    ops_ = nullptr;
  }
}

template <std::size_t N, std::size_t Align>
void Message<N, Align>::move_from(Message<N, Align>& other) noexcept {
  if (!other.ops_) {
    ops_ = nullptr;
    return;
  }
  other.ops_->move_to(other.storage_, storage_);
  ops_ = other.ops_;
  other.ops_ = nullptr;
}

template <std::size_t N, std::size_t Align>
template <class T>
const Message<N, Align>::MsgOps& Message<N, Align>::ops_for() noexcept {
  static const MsgOps ops{
      [](void* data_to_destroy) noexcept {
        std::launder(reinterpret_cast<T*>(data_to_destroy))->~T();
      },
      // WARN: new
      [](void* move_src, void* move_dst) noexcept {
        T* s = std::launder(reinterpret_cast<T*>(move_src));
        new (move_dst) T(std::move(*s));
        s->~T();
      },
      []() noexcept -> type_id_t { return Anorency::types::type_id<T>(); }};

  return ops;
}

}  // namespace Anorency
