#include "AnoInterface.h"
#include <cstdint>

AnoInterface::AnoInterface(uint32_t this_id): this_id_(this_id) {}

uint32_t AnoInterface::get_this_id(){

    return this_id_;

}