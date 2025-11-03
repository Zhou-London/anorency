#pragma once

#include <string>

#include "Aggregator.h"
#include "Types.h"
namespace Anorency {

class AnoInterface {
 public:
  AnoInterface(std::string& str, Aggregator& aggr);
  ~AnoInterface() = default;

  void set_str(const std::string& new_str);
  std::string get_str();

  template <typename T>
  stream_id_t make_stream() {
    return aggr.add<T>();
  }

  template <typename T>
  stream_id_t make_stream(Stream<T>* p) {
    return aggr.add<T>(p);
  }

  template <typename T>
  Stream<T>* get_stream(size_t index) {
    return aggr.get<T>(index);
  }

 private:
  std::string& str;
  Aggregator& aggr;
};

}  // namespace Anorency