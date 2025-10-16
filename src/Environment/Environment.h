#pragma once

#include <concepts>
#include <string>

#include "Aggregator.h"
#include "AnoInterface.h"
namespace Anorency {

class Environment {
 public:
  Environment() = default;
  Environment(std::size_t aggr_size);
  ~Environment() = default;

  template <typename F>
    requires std::invocable<F, AnoInterface&&>
  void introduce(F&& f) {
    f(std::move(AnoInterface(str, aggr)));
  }

 private:
  std::string str;

  Aggregator aggr;
};

}  // namespace Anorency