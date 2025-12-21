#pragma once

#include "alias.h"

namespace Anorency {
namespace types {

template <class T>
type_id_t type_id() noexcept {
  static int anchor;
  return &anchor;
}
}  // namespace types

}  // namespace Anorency
