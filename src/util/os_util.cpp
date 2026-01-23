#include "os_util.h"

#if ANORENCY_OS_LINUX
#include <pthread.h>
#include <sched.h>
#elif ANORENCY_OS_MACOS
#include <mach/mach.h>
#include <mach/thread_policy.h>
#include <pthread.h>
#endif

namespace Anorency {

void set_thread_affinity(int core) {
#if ANORENCY_OS_LINUX
  // Linux: True core pinning using pthread_setaffinity_np
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core, &cpuset);

  pthread_t thread = pthread_self();
  pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

#elif ANORENCY_OS_MACOS
  // macOS: Affinity tags as scheduling hints (not guaranteed pinning)
  // Threads with the same affinity tag are scheduled to run together.
  // We use core+1 as tag since 0 means "no affinity".
  thread_affinity_policy_data_t policy;
  policy.affinity_tag = core + 1;

  thread_port_t mach_thread = pthread_mach_thread_np(pthread_self());
  thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                    reinterpret_cast<thread_policy_t>(&policy),
                    THREAD_AFFINITY_POLICY_COUNT);
#endif
}

}  // namespace Anorency
