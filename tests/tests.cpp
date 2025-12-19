#include <gtest/gtest.h>

#include <ranges>
#include <thread>

#include "Stream.h"
#include "version.h"

TEST(Info, info) { EXPECT_EQ(version(), "v0.1"); }

TEST(Streams, StreamFIFOTest) {
  Stream stream;

  int total_n = 1024, written_n = 0, read_n = 0;

  std::thread write([&]() {
    for (int i = 0; i < total_n; ++i) {
      stream.write(i);
      ++written_n;
    }

    return;
  });

  std::thread read([&]() {
    while (read_n < total_n) {
      stream.read();
      ++read_n;
    }

    return;
  });

  write.join();
  read.join();

  EXPECT_EQ(written_n, read_n);
  EXPECT_EQ(written_n, 1024);
  EXPECT_EQ(read_n, 1024);
}
