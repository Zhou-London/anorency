#pragma once

#include <cstdint>
#include <thread>
#include <unordered_map>

using ThreadsMap = std::unordered_map<uint32_t, std::thread>;

using ThreadID = uint32_t;