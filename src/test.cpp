#include <exception>
#include <iostream>
#include <string>

#include "Aggregator.h"
#include "AnoException.h"
#include "AnoInterface.h"
#include "Environment.h"
#include "Line.h"
#include "Types.h"

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

inline void TEST_CASE_AGGREGATOR_INTERFACE() {
  std::cout << "\nStart Test: Aggregator Interface Integration.\n";

  Anorency::Environment env;
  Anorency::line_id_t id;
  env.introduce([&](Anorency::AnoInterface&& iface) {
    auto this_id = iface.make_line<int>();
    id = this_id;
    auto line = iface.get_line<int>(id);
    line->push(10);

    std::cout << "First Task: Value Pushed.\n";
  });

  env.introduce([&](Anorency::AnoInterface&& iface) {
    auto line = iface.get_line<int>(id);
    std::cout << line->peek() << "\n";
    line->pop();

    std::cout << "Second Task: Value Read.\n";
  });
}

int main() {
  TEST_CASE_ENVIRONMENT_INTRODUCE();
  TEST_CASE_EXCEPTION();
  TEST_CASE_AGGREGATOR();
  TEST_CASE_AGGREGATOR_INTERFACE();
  return 0;
}