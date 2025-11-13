#include "Aggregator.h"

#include <cstddef>
#include <typeindex>

namespace Anorency {
Aggregator::Aggregator() { this->storage_.reserve(128); }

Aggregator::Aggregator(uint32_t size) { this->storage_.reserve(size); }

Aggregator::~Aggregator() {
  // ! Probably do more
}

std::size_t Aggregator::size() const noexcept { return storage_.size(); }

std::type_index Aggregator::type_at(std::size_t index) const {
  if (index >= storage_.size())
    throw AnoException("Aggregator: Index out of range.");

  return storage_[index]->type();
}
}  // namespace Anorency