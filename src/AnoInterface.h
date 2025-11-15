#pragma once

#include <algorithm>
#include <string>

#include "Aggregator.h"
#include "Types.h"
namespace Anorency {

template <typename S>
class AnoInterface {
 public:
  AnoInterface(std::string& str, Aggregator& aggr, stream_id_t uniq_stream_id)
      : str(str), aggr(aggr), uniq_stream_id(uniq_stream_id) {
        uniq_stream = aggr.get<S>(uniq_stream_id);
      }
  ~AnoInterface() = default;

  void set_str(const std::string& new_str) { this->str = new_str; }
  std::string get_str() const noexcept { return this->str; }

  stream_id_t get_uniq_stream_id() const noexcept {
    return this->uniq_stream_id;
  }

  Stream<S>* get_uniq_stream() const noexcept { return this->uniq_stream; }

  S read_uniq_stream() { return this->uniq_stream->read(); }

  void write_uniq_stream(S&& val) { this->uniq_stream->write(std::move(val)); }

  template <typename T>
  stream_id_t make_stream() {
    return aggr.add<T>();
  }

  template <typename T>
  stream_id_t make_stream(Stream<T>* p) {
    return aggr.add<T>(p);
  }

  template <typename T>
  Stream<T>* get_stream(stream_id_t index) {
    return aggr.get<T>(index);
  }

 private:
  using uniq_stream_t = S;

  std::string& str;
  Aggregator& aggr;
  stream_id_t uniq_stream_id;
  Stream<S>* uniq_stream;
};

}  // namespace Anorency