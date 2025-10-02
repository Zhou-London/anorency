
#include "AnoEnvironment.h"

#include <iostream>

namespace Anorency {

AnoEnvironment::AnoEnvironment() = default;
AnoEnvironment::~AnoEnvironment() = default;

void AnoEnvironment::introduce_task() { std::cout << "Introduce Task\n"; }

void AnoEnvironment::join_all_tasks() { std::cout << "Join All Tasks\n"; }

}  // namespace Anorency