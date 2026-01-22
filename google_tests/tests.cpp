#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <ranges>
#include <string>
#include <thread>
#include <vector>

#include "MemPool.h"
#include "Message.h"
#include "MessageWrappers.h"
#include "Stream.h"
#include "mem_pool_wrapper.h"
#include "types.h"
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

TEST(MemPool, BasicAllocation) {
  Anorency::MemPool pool;

  void* ptr = pool.allocate(64, 8);
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % 8, 0);

  pool.deallocate(ptr);
}

TEST(MemPool, AlignedAllocation) {
  Anorency::MemPool pool;

  for (std::size_t align : {8, 16, 32, 64, 128}) {
    void* ptr = pool.allocate(100, align);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % align, 0);
    pool.deallocate(ptr);
  }
}

TEST(MemPool, ZeroSizeAllocation) {
  Anorency::MemPool pool;

  void* ptr = pool.allocate(0, 8);
  EXPECT_EQ(ptr, nullptr);
}

TEST(Messages, PoolBasedMessageConceptCheck) {
  // Verify MemPool satisfies the concept
  static_assert(Anorency::interfaces::mem_pool_wrapper<Anorency::MemPool>,
                "MemPool must satisfy mem_pool_wrapper concept");

  Anorency::MemPool pool;

  auto msg = MessageS::make<int>(pool, 42);

  EXPECT_FALSE(msg.empty());
  EXPECT_NE(msg.type_id(), nullptr);
  EXPECT_EQ(msg.type_id(), Anorency::types::type_id<int>());

  auto v = msg.view();
  EXPECT_NE(v.type, nullptr);
  EXPECT_NE(v.data, nullptr);
}

TEST(Messages, PoolBasedMessageCreation) {
  Anorency::MemPool pool;

  auto msg = MessageS::make<int>(pool, 42);
  ASSERT_NE(msg.try_get<int>(), nullptr);
  EXPECT_EQ(*msg.try_get<int>(), 42);
}

TEST(Messages, PoolBasedMessageTypeCheck) {
  Anorency::MemPool pool;

  auto msg = MessageS::make<int>(pool, 42);
  EXPECT_EQ(msg.try_get<float>(), nullptr);
  EXPECT_EQ(msg.try_get<double>(), nullptr);
  ASSERT_NE(msg.try_get<int>(), nullptr);
}

TEST(Messages, PoolBasedMessageDestruction) {
  Anorency::MemPool pool;
  int destruction_count = 0;

  struct Trackable {
    int* counter;

    Trackable() noexcept : counter(nullptr) {}
    Trackable(int& c) noexcept : counter(&c) {}
    Trackable(Trackable&& other) noexcept : counter(other.counter) {
      other.counter = nullptr;
    }
    ~Trackable() {
      if (counter) ++(*counter);
    }
  };

  {
    auto msg = MessageS::make<Trackable>(pool, destruction_count);
    EXPECT_EQ(destruction_count, 0);
  }

  EXPECT_EQ(destruction_count, 1);
}

TEST(Messages, PoolBasedMessageMove) {
  Anorency::MemPool pool;

  auto msg1 = MessageS::make<int>(pool, 100);
  ASSERT_NE(msg1.try_get<int>(), nullptr);
  EXPECT_EQ(*msg1.try_get<int>(), 100);

  auto msg2 = std::move(msg1);

  EXPECT_TRUE(msg1.empty());
  ASSERT_NE(msg2.try_get<int>(), nullptr);
  EXPECT_EQ(*msg2.try_get<int>(), 100);
}

TEST(Messages, PoolBasedMessageMoveAssignment) {
  Anorency::MemPool pool;

  auto msg1 = MessageS::make<int>(pool, 100);
  auto msg2 = MessageS::make<int>(pool, 200);

  msg1 = std::move(msg2);

  EXPECT_TRUE(msg2.empty());
  ASSERT_NE(msg1.try_get<int>(), nullptr);
  EXPECT_EQ(*msg1.try_get<int>(), 200);
}

TEST(Messages, PoolBasedLargePayload) {
  Anorency::MemPool pool;

  struct LargePayload {
    std::array<char, 256> data;

    LargePayload() noexcept = default;
    LargePayload(char fill) noexcept { data.fill(fill); }
    LargePayload(LargePayload&&) noexcept = default;
  };

  auto msg = MessageS::make<LargePayload>(pool, 'X');
  ASSERT_NE(msg.try_get<LargePayload>(), nullptr);
  EXPECT_EQ(msg.try_get<LargePayload>()->data[0], 'X');
  EXPECT_EQ(msg.try_get<LargePayload>()->data[255], 'X');
}

TEST(Messages, PoolBasedMessageConsistency) {
  Anorency::MemPool pool;
  constexpr int N = 1024;

  std::vector<MessageS> v;
  v.reserve(N);

  for (int i : std::ranges::views::iota(0, N)) {
    v.emplace_back(std::move(MessageS::make<int>(pool, i * i)));
  }

  for (int i = 0; i < N; ++i) {
    const auto msg = v[i].try_get<int>();
    ASSERT_NE(msg, nullptr);
    EXPECT_EQ(*msg, i * i);
  }
}
