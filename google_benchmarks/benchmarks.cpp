#include <benchmark/benchmark.h>

#include <thread>
#include "Stream.h"

using namespace Anorency;

static void BM_Stream_FIFO(benchmark::State& state) {
  constexpr int N = 1 << 20;
  Stream<int> stream;
  
  for (auto _ : state) {
    int r = 0;

    std::jthread writer([&]() {
      for (int i = 0; i < N; ++i)
        while (!stream.try_write(i))
          std::this_thread::yield();
    });

    for (int i = 0; i < N; ++i)
      while (!stream.try_read(r))
        std::this_thread::yield();

    benchmark::DoNotOptimize(r);
  }
}

BENCHMARK(BM_Stream_FIFO);
BENCHMARK_MAIN();
