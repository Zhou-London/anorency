#include "AnoInterface.h"

#include <string>

namespace Anorency {

AnoInterface::AnoInterface(std::string& str, Aggregator& aggr)
    : str(str), aggr(aggr) {}

void AnoInterface::set_str(const std::string& new_str) { this->str = new_str; }

std::string AnoInterface::get_str() { return this->str; }

}  // namespace Anorency