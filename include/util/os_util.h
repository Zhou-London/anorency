#pragma once

// ============================================================================
// Platform Detection
// ============================================================================

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
#define ANORENCY_ARCH_X86_64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ANORENCY_ARCH_ARM64 1
#else
#error "Unsupported architecture: only x86_64 and ARM64 are supported"
#endif

// Operating system detection
#if defined(__linux__)
#define ANORENCY_OS_LINUX 1
#elif defined(__APPLE__)
#define ANORENCY_OS_MACOS 1
#else
#error "Unsupported OS: only Linux and macOS are supported"
#endif

namespace Anorency {

// ============================================================================
// CPU Relaxation
// ============================================================================

/// Hints to the CPU that we're in a spin-loop.
/// Reduces power consumption and improves performance on SMT/hyperthreaded CPUs.
inline void cpu_relax() {
#if ANORENCY_ARCH_X86_64
  // x86 PAUSE instruction: ~10 cycles delay, prevents memory order violation
  asm volatile("pause" ::: "memory");
#elif ANORENCY_ARCH_ARM64
  // ARM64 YIELD instruction: hints to switch to another thread
  asm volatile("yield" ::: "memory");
#endif
}

// ============================================================================
// Thread Affinity
// ============================================================================

/// Attempts to pin the current thread to a specific CPU core.
///
/// Behavior varies by platform:
/// - Linux: True core pinning via pthread_setaffinity_np()
/// - macOS: Affinity tag hint via thread_policy_set() (not guaranteed)
///
/// @param core The CPU core index (0-based)
void set_thread_affinity(int core);

}  // namespace Anorency
