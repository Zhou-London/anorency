#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Aggregator.h"
#include "AnoException.h"
#include "AnoInterface.h"
#include "Environment.h"
#include "Line.h"
#include "Types.h"

#define TEST_PASS "PASS"
#define TEST_FAIL "FAIL"

uint16_t PASS_C = 0;
uint16_t FAIL_C = 0;

inline void TEST_CASE_EXCEPTION() {
  std::cout << "\nStart Test: Exception.\n";

  try {
    throw Anorency::AnoException("Unknown Error.");
  } catch (Anorency::AnoException& e) {
    std::cout << TEST_PASS << "\n";
    ++PASS_C;
  } catch (std::exception& e) {
    std::cout << TEST_FAIL << "\n";
    ++FAIL_C;
  } catch (...) {
    std::cout << TEST_FAIL << "\n";
    ++FAIL_C;
  }
}

inline void TEST_CASE_ENVIRONMENT_INTRODUCE() {
  std::cout << "\nStart Test: Environment_Introduce.\n";

  Anorency::Environment env;
  env.introduce([&](Anorency::AnoInterface&& iface) {
    iface.set_str("abc");

    if (iface.get_str() == "abc") {
      std::cout << TEST_PASS << "\n";
      ++PASS_C;
      return;
    }
    std::cout << TEST_FAIL << "\n";
    ++FAIL_C;
  });
}

inline void TEST_CASE_AGGREGATOR() {
  std::cout << "\nStart Test: Aggregator.\n";

  Anorency::Aggregator aggr;
  auto it = aggr.add<int>();

  auto line = aggr.get<int>(it);
  auto vec = std::vector<int>{1, 2, 3, 4, 5};

  for (const auto i : vec) line->push(i);

  auto res = std::vector<int>(vec.size(), 0);
  for (size_t i = 0; i != vec.size(); ++i) {
    res[i] = line->peek();
    line->pop();
  }

  if (res == vec && line->size() == 0) {
    std::cout << TEST_PASS << "\n";
    ++PASS_C;
    return;
  }
  std::cout << TEST_FAIL << "\n";
  ++FAIL_C;
}

inline void TEST_CASE_AGGREGATOR_INTERFACE() {
  std::cout << "\nStart Test: Aggregator Interface Integration.\n";

  Anorency::Environment env;
  Anorency::line_id_t id;
  int v = 10;
  int res = 0;

  env.introduce([&](Anorency::AnoInterface&& iface) {
    id = iface.make_line<int>();
    auto line = iface.get_line<int>(id);
    line->push(v);
  });

  env.introduce([&](Anorency::AnoInterface&& iface) {
    auto line = iface.get_line<int>(id);
    res = line->peek();
  });

  if (res == v) {
    std::cout << TEST_PASS << "\n";
    ++PASS_C;
    return;
  }
  std::cout << TEST_FAIL << "\n";
  ++FAIL_C;
}

int main() {
  TEST_CASE_ENVIRONMENT_INTRODUCE();
  TEST_CASE_EXCEPTION();
  TEST_CASE_AGGREGATOR();
  TEST_CASE_AGGREGATOR_INTERFACE();

  auto pass_rate = (double)PASS_C / (PASS_C + FAIL_C) * 100;
  std::cout << "\n" // 
  << PASS_C + FAIL_C << " test cases executed.\n"
  << PASS_C << " passed.\n"
  << FAIL_C << " failed.\n"
  << "Pass rate: " << pass_rate << "%\n";

  return 0;
}