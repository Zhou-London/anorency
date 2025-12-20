#pragma once

#include "Alias.h"

namespace Anorency {
struct MessageView {
  type_id_t type;
  const void* data;
};
}  // namespace Anorency
