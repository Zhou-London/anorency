#pragma once

#include <concepts>
#include <string>

#include "AnoInterface.h"
namespace Anorency {

class Environment {
 public:
  Environment() = default;
  ~Environment() = default;

  template <typename F>
    requires std::invocable<F, AnoInterface&&>
  void introduce(F&& f) {
    f(std::move(AnoInterface(str)));
  }

 private:
  std::string str;
};

}  // namespace Anorency