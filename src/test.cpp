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
  Anorency::Line<int> il;
  il.push(1);
  Anorency::Line<std::string> sl;
  sl.push("Hello");

  aggr.add(&il);
  aggr.add(&sl);

  auto v1 = aggr.get<int>(0);
  auto v2 = aggr.get<std::string>(1);

  std::cout << v1->peek() << "\n";
  std::cout << v2->peek() << "\n";
}

int main() {
  TEST_CASE_ENVIRONMENT_INTRODUCE();
  TEST_CASE_EXCEPTION();
  TEST_CASE_AGGREGATOR();
  return 0;
}