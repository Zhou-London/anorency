#include "Aggregator.h"

namespace Anorency {
Aggregator::Aggregator() { this->storage_.reserve(128); }

Aggregator::Aggregator(uint32_t size) { this->storage_.reserve(size); }

Aggregator::~Aggregator() {
  // ! Probably do more
}

}  // namespace Anorency