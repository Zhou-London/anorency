#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <typeindex>
#include <vector>

#include "AnoException.h"
#include "Line.h"
namespace Anorency {

class Aggregator {
 public:
  Aggregator() { storage_.reserve(128); }
  Aggregator(uint32_t size) { storage_.reserve(size); }
  ~Aggregator() = default;

  template <typename T>
  std::size_t add(Line<T>* p) {
    {
      std::unique_lock<std::mutex> m;
      storage_.emplace_back(std::make_unique<Model<T>>(p));
      return storage_.size() - 1;
    }
  }

  template <typename T>
  std::size_t add() {
    {
      std::unique_lock<std::mutex> m;
      storage_.emplace_back(std::make_unique<Model<T>>(new Line<T>()));
      return storage_.size() - 1;
    }
  }

  template <typename T>
  Line<T>* get(std::size_t index) const {
    if (index >= storage_.size())
      throw AnoException("Aggregator: Index out of range.");

    const auto& item = storage_[index];

    if (item->type() == typeid(Line<T>))
      return static_cast<Line<T>*>(item->raw_ptr());
    else
      throw AnoException("Aggregator: Type mismatch.");
  }

  std::type_index type_at(std::size_t index) const {
    if (index >= storage_.size())
      throw AnoException("Aggregator: Index out of range.");

    return storage_[index]->type();
  }

  std::size_t size() const noexcept { return storage_.size(); }

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void* raw_ptr() = 0;
    virtual std::type_index type() const = 0;
  };

  template <typename T>
  struct Model : Concept {
    Line<T>* ptr;
    Model(Line<T>* ptr) : ptr(ptr) {}
    void* raw_ptr() override { return ptr; }
    std::type_index type() const override { return typeid(Line<T>); }
  };

  std::mutex mutex_;
  std::vector<std::unique_ptr<Concept>> storage_;
};

}  // namespace Anorency