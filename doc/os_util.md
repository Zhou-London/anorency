# OS Utilities

Low-level CPU and thread utilities with platform-specific implementations.

## First Principles

### Why These Exist

**Spin-loops waste energy.** When code waits for something (like a lock), it typically spins in a tight loop. Modern CPUs can do better if you tell them "I'm waiting."

**Thread placement matters for performance.** Running related threads on nearby cores improves cache sharing. Running them on different cores reduces contention.

### What Each Utility Does

#### `cpu_relax()`

Tells the CPU: "I'm in a spin-loop, don't run at full speed."

**How it works:**
- **x86**: `pause` instruction - delays ~10 cycles, prevents memory reorder violations
- **ARM64**: `yield` instruction - hints to switch to another hardware thread

**When to use:**
```cpp
while (!ready.load()) {
    cpu_relax();  // Don't burn the CPU while waiting
}
```

#### `set_thread_affinity(core)`

Asks the OS: "Run this thread on a specific CPU core."

**Platform differences:**

| Platform | Behavior | Guarantee |
|----------|----------|-----------|
| Linux    | `pthread_setaffinity_np()` | Strict - thread will only run on that core |
| macOS    | `thread_policy_set()` with affinity tag | Hint only - scheduler may ignore it |

**Why macOS is different:** Apple controls the P-core/E-core scheduling on Apple Silicon. They don't expose true core pinning to applications.

**When to use:**
```cpp
// Pin producer and consumer to different cores
std::jthread producer([&]() {
    set_thread_affinity(0);  // Core 0
    // produce items...
});

std::jthread consumer([&]() {
    set_thread_affinity(1);  // Core 1
    // consume items...
});
```

## Compile-Time Platform Detection

The code auto-detects your platform at compile time:

```cpp
// Architecture (mutually exclusive)
#define ANORENCY_ARCH_X86_64 1  // Intel/AMD 64-bit
#define ANORENCY_ARCH_ARM64 1   // ARM 64-bit (Apple Silicon, AWS Graviton)

// Operating System (mutually exclusive)
#define ANORENCY_OS_LINUX 1     // Linux
#define ANORENCY_OS_MACOS 1     // macOS
```

**Unsupported platforms cause a compile error** - you'll know immediately rather than getting runtime failures.

## Implementation Details

### cpu_relax() - Inline in Header

```cpp
inline void cpu_relax() {
#if ANORENCY_ARCH_X86_64
    asm volatile("pause" ::: "memory");
#elif ANORENCY_ARCH_ARM64
    asm volatile("yield" ::: "memory");
#endif
}
```

The `"memory"` clobber tells the compiler: "Memory may have changed - don't reorder reads/writes across this."

### set_thread_affinity() - In Source File

Linux and macOS need different headers and APIs, so implementation stays in `os_util.cpp`.

**Linux:**
```cpp
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(core, &cpuset);
pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
```

**macOS:**
```cpp
thread_affinity_policy_data_t policy;
policy.affinity_tag = core + 1;  // 0 means "no affinity"
thread_policy_set(pthread_mach_thread_np(pthread_self()),
                  THREAD_AFFINITY_POLICY, ...);
```

## Testing

Run the OS utility tests:
```bash
./build/test/google_tests/ano_tests --gtest_filter="OsUtil.*"
```

Tests verify:
- Platform macros are mutually exclusive
- `cpu_relax()` doesn't crash (called 1000+ times)
- `cpu_relax()` works in actual spin-loop scenarios
- `set_thread_affinity()` doesn't crash for multiple cores
- Thread affinity works from spawned threads
