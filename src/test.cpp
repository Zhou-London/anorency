

#include <iostream>

#include "AnoTaskDefault.h"

int main(int argc, char **argv) {
  auto task = Anorency::AnoTaskDefault();

  task.load([&]() { 
    std::cout << "Hello World\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  });

  return 0;
}