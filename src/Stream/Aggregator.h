#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <typeindex>
#include <vector>

#include "AnoException.h"
#include "Stream.h"
#include "Types.h"
namespace Anorency {

class Aggregator {
 public:
  Aggregator();
  Aggregator(uint32_t size);
  ~Aggregator();

  template <typename T>
  stream_id_t add(Stream<T>* p) {
    {
      std::unique_lock<std::mutex> m;
      storage_.emplace_back(std::make_unique<Model<T>>(p));
      return storage_.size() - 1;
    }
  }

  template <typename T>
  stream_id_t add() {
    {
      std::unique_lock<std::mutex> m;
      storage_.emplace_back(std::make_unique<Model<T>>(new Stream<T>()));
      return storage_.size() - 1;
    }
  }

  template <typename T>
  Stream<T>* get(std::size_t index) const {
    if (index >= storage_.size())
      throw AnoException("Aggregator: Index out of range.");

    const auto& item = storage_[index];

    if (item->type() == typeid(Stream<T>))
      return static_cast<Stream<T>*>(item->raw_ptr());
    else if (item == nullptr)
      throw AnoException("Aggregator: Pointer destructed.");
    else
      throw AnoException("Aggregator: Type mismatch.");
  }

  std::type_index type_at(std::size_t index) const;

  std::size_t size() const noexcept;

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void* raw_ptr() = 0;
    virtual std::type_index type() const = 0;
  };

  template <typename T>
  struct Model : Concept {
    Stream<T>* ptr;
    Model(Stream<T>* ptr) : ptr(ptr) {}
    void* raw_ptr() override { return ptr; }
    std::type_index type() const override { return typeid(Stream<T>); }
  };

  std::mutex mutex_;
  std::vector<std::unique_ptr<Concept>> storage_;
};

}  // namespace Anorency