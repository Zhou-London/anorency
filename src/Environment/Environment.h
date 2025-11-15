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

  template <typename S, typename F>
    requires std::invocable<F, AnoInterface<S>&&>
  stream_id_t introduce(F&& f) {
    using uniq_stream_t = S;
    auto uniq_id = aggr.add<uniq_stream_t>();

    auto wrap = [&]() {
      f(std::move(AnoInterface<S>(str, aggr, uniq_id)));
    };

    wrap();

    return uniq_id;
  }

 private:
  std::string str;

  Aggregator aggr;
};

}  // namespace Anorency