#pragma once

#include "AnoAgent/AnoAgent.h"
#include "Flags.h"
#include <cstdint>
#include <functional>
#include <unordered_map>

using AgentsMap = std::unordered_map<uint32_t, AnoAgent>;

using AgentID = uint32_t;

using TaskNewThread = std::function<void(Flag::NewThread)>;