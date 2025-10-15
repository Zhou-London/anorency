#include <exception>
#include <iostream>
#include <string>

#include "Aggregator.h"
#include "AnoException.h"
#include "Environment.h"
#include "Line.h"

inline void TEST_CASE_EXCEPTION() {
  std::cout << "\nStart Test: Exception.\n";

  try {
    throw Anorency::AnoException("Unknown Error.");
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
  }
}

inline void TEST_CASE_ENVIRONMENT_INTRODUCE() {
  std::cout << "\nStart Test: Environment_Introduce.\n";

  Anorency::Environment env;
  env.introduce([&](Anorency::AnoInterface&& iface) {
    iface.set_str("First set.");
    std::cout << iface.get_str() << "\n";

    iface.set_str("Second set.");
    std::cout << iface.get_str() << "\n";
  });
}

inline void TEST_CASE_AGGREGATOR() {
  std::cout << "\nStart Test: Aggregator.\n";

  Anorency::Aggregator aggr;
  auto it = aggr.add<int>();

  auto line = aggr.get<int>(it);
  line->push(10);
  line->push(20);
  line->push(30);
  line->push(40);
  line->push(50);
  std::cout << line->peek() << "\n";
  line->pop();
  std::cout << line->peek() << "\n";
  line->pop();
  std::cout << line->peek() << "\n";
  line->pop();
  std::cout << line->peek() << "\n";
  line->pop();
  std::cout << line->peek() << "\n";
  line->pop();
}

int main() {
  TEST_CASE_ENVIRONMENT_INTRODUCE();
  TEST_CASE_EXCEPTION();
  TEST_CASE_AGGREGATOR();
  return 0;
}