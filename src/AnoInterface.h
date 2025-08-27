#pragma once

#include <cstdint>

class AnoInterface{
public:
  AnoInterface(uint32_t this_id);

  uint32_t get_this_id();

private:
  uint32_t this_id_;

};