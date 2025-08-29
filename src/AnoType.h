#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "AnoAgent/AnoAgent.h"
#include "Flags.h"


using AgentsMap = std::unordered_map<uint32_t, AnoAgent>;

using AgentID = uint32_t;

using TaskNewThread = std::function<void(Flag::NewThread)>;