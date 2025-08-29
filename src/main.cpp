#include <chrono>
#include <iostream>
#include <thread>

#include "AnoEnvironment.h"
#include "AnoType.h"
#include "Flags.h"

int main() {
  AnoEnvironment env;

  auto t1 = env.introduce([&](Flag::NewThread) {
    std::cout << "Hello\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  });

  env[t1].register_task([&](Flag::NewThread) {
    std::cout << "World\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  });

  return 0;
}