

#include <chrono>
#include <iostream>
#include <thread>

#include "AnoEnvironment.h"
#include "AnoInterface.h"
#include "Flags.h"

int main() {
  AnoEnvironment env;

  auto ano_thread = env.introduce<NewThread>([&](AnoInterface& interface) {
    auto id = interface.get_this_id();
    std::cout << "My id is: " << (int)id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    return NewThread();
  });

  //env.join(ano_thread->id());

  return 0;
}