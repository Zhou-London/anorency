
#include "AnoEnvironment.h"

#include <cstdint>
#include <iostream>

namespace Anorency {

AnoEnvironment::AnoEnvironment() = default;
AnoEnvironment::~AnoEnvironment() = default;

uint32_t AnoEnvironment::introduce_task() {
  std::cout << "Introduce Task\n";
  return 0;
}

uint32_t AnoEnvironment::join_all_tasks() {
  std::cout << "Join All Tasks\n";
  return 0;
}

}  // namespace Anorency