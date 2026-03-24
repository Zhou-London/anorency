#pragma once

#include <cstdint>

namespace Anon {

struct Address {
  uint32_t index;
  uint32_t generation;

  bool operator==(const Address&) const = default;
};

}  // namespace Anon
