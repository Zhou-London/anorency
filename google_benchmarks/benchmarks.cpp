#include <benchmark/benchmark.h>

#include <algorithm>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

#include "Stream.h"

static void BM_Stream_FIFO(benchmark::State& state) {
  for (auto _ : state) {
    Stream stream;
    int total_n = 1 << 12;
    {
      std::jthread write([&]() {
        for (int i : std::ranges::views::iota(0, total_n)) stream.write(i);
      });

      std::jthread read([&]() {
        for (int i : std::ranges::views::iota(0, total_n)) stream.read();
      });
    }
  }
}

BENCHMARK(BM_Stream_FIFO);
BENCHMARK_MAIN();
