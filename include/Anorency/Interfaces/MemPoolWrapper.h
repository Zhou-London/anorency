#pragma once

#include <concepts>
#include <cstddef>
namespace Anorency {
namespace interfaces {

template <typename T>
concept mem_pool_wrapper =
    requires(T s, std::size_t size, std::size_t align, void* ptr) {
      { s.allocate(size, align) } noexcept -> std::same_as<void*>;
      { s.deallocate(ptr) } noexcept -> std::same_as<void>;
    };

}  // namespace interfaces
}  // namespace Anorency
