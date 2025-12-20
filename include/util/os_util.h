#pragma once

namespace Anorency {
void cpu_relax();

void set_thread_affinity(int core);
}  // namespace Anorency
