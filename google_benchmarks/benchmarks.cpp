#include <benchmark/benchmark.h>
#include <pthread/qos.h>
#include <sys/qos.h>

#include <atomic>
#include <thread>

#include "Stream.h"
#include "os_util.h"

static void BM_Stream_FIFO(benchmark::State& state) {
  constexpr int N = 1 << 22;
  auto stream = Stream<int>();
  std::atomic<bool> start{false};

  int r = 0;

  std::jthread writer([&]() {
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    while (!start.load(std::memory_order_acquire)) {
    }

    for (int i = 0; i < N; ++i)
      while (!stream.try_write(i)) {
        cpu_relax();
      }
  });

  std::jthread reader([&]() {
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    while (!start.load(std::memory_order_acquire)) {
    }

    for (int i = 0; i < N; ++i)
      while (!stream.try_read(r)) {
        cpu_relax();
      }
  });

  for (auto _ : state) {
    start.store(false, std::memory_order_relaxed);

    benchmark::DoNotOptimize(r);

    start.store(true, std::memory_order_release);

    while (stream.size() != 0) {
      cpu_relax();
    }
  }
}

BENCHMARK(BM_Stream_FIFO);
BENCHMARK_MAIN();
