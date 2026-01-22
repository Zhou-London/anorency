#include "os_util.h"

#include <pthread.h>
#include <sched.h>

namespace Anorency {

void cpu_relax() { asm volatile("yield" ::: "memory"); }

void set_thread_affinity(int core) {
  // TODO:
}
}  // namespace Anorency
