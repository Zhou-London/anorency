#include <benchmark/benchmark.h>

#include <ranges>
#include <thread>

#include "Stream.h"

static void BM_Stream_FIFO(benchmark::State& state) {
  for (auto _ : state) {
    Stream stream;

    int N = 1 << 12;
    int r = 0;
    {
      std::jthread write([&]() {
        for (int i : std::ranges::views::iota(0, N))
          while (!stream.try_write(i)) __asm__ volatile("yield");
      });

      std::jthread read([&]() {
        for (int i : std::ranges::views::iota(0, N))
          while (!stream.try_read(r)) __asm__ volatile("yield");
      });
    }
  }
}

BENCHMARK(BM_Stream_FIFO);
BENCHMARK_MAIN();
