#include <gtest/gtest.h>

#include <ranges>
#include <thread>

#include "Stream.h"
#include "version.h"

TEST(Info, info) { EXPECT_EQ(version(), "v0.1"); }

TEST(Streams, StreamFIFOTest) {
  auto stream = Stream<int, 2048>();

  int total_n = 1024, written_n = 0, read_n = 0;

  int r;
  {
    std::jthread write([&]() {
      for (int i : std::ranges::views::iota(0, total_n)) {
        while (!stream.try_write(i)) {
        }

        ++written_n;
      }
    });

    std::jthread read([&]() {
      for (int i : std::ranges::views::iota(0, total_n)) {
        while (!stream.try_read(r)) {
        }

        ++read_n;
      }
    });
  }

  EXPECT_EQ(written_n, read_n);
  EXPECT_EQ(written_n, 1024);
  EXPECT_EQ(read_n, 1024);
  EXPECT_EQ(r, 1023);
}
