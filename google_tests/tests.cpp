#include <gtest/gtest.h>

#include <array>
#include <ranges>
#include <string>
#include <thread>
#include <vector>

#include "Message.h"
#include "MessageWrappers.h"
#include "Stream.h"
#include "version.h"

using namespace Anorency;

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

TEST(Messages, MessageConsistencyTest) {
  constexpr int N = 4096;

  auto testing = std::array<int, N>();
  for (int i : std::ranges::views::iota(0, N)) {
    testing[i] = i * i;
  }

  std::vector<MessageS> v;
  v.reserve(N);

  for (int i : std::ranges::views::iota(0, N)) {
    v.emplace_back(std::move(MessageS::make<int>(i * i)));
  }

  for (int i = 0; i < N; ++i) {
    const auto msg = v[i].try_get<int>();
    if (msg) {
      EXPECT_EQ(*msg, testing[i]);
    }
  }
}

TEST(Messages, MessageTypesTest) {
  {
    auto msg = MessageS::make<std::string>("Hello world.");

    EXPECT_EQ(msg.try_get<int>(), nullptr);
    EXPECT_EQ(msg.try_get<float>(), nullptr);
    EXPECT_EQ(*msg.try_get<std::string>(), "Hello world.");
  }

  int x = 10;
  {
    struct Content {
      int* ptr;

      Content() noexcept = default;
      Content(int& x) noexcept : ptr(&x) {}

      ~Content() {
        *ptr = 5;
        ptr = nullptr;
      }
    };

    auto msg = MessageS::make<Content>(x);
    EXPECT_EQ(msg.try_get<Content>()->ptr, &x);
  }
  EXPECT_EQ(x, 5);
}
