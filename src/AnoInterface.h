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
  line_id_t make_line() {
    return aggr.add<T>();
  }

  template <typename T>
  line_id_t make_line(Line<T>* p) {
    return aggr.add<T>(p);
  }

  template <typename T>
  Line<T>* get_line(size_t index) {
    return aggr.get<T>(index);
  }

 private:
  std::string& str;
  Aggregator& aggr;
};

}  // namespace Anorency