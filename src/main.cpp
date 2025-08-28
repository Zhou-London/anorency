#include "AnoEnvironment.h"
#include "Flags.h"
#include <iostream>

int main() {

  AnoEnvironment env;

  auto id = env.introduce([](Flag::NewThread) { std::cout << "Hello 1\n"; });

  auto id2 = env.introduce([](Flag::NewThread) { std::cout << "Hello 2\n"; });

  auto id3 = env.introduce([](Flag::NewThread) { std::cout << "Hello 3\n"; });

  return 0;
}