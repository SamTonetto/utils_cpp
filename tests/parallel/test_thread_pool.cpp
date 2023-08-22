#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "parallel/thread_pool.hpp"

using namespace utils;

TEST_CASE("Testing thread_pool functionality") {
  parallel::thread_pool pool;

  SUBCASE("Single task submission") {
    auto future = pool.submit([]() { return 42; });
    CHECK(future.get() == 42);
  }

  SUBCASE("Multiple tasks submission") {
    std::vector<std::future<int>> results;

    for (int i = 0; i < 100; ++i) {
      results.push_back(pool.submit([i]() { return i * i; }));
    }

    for (int i = 0; i < 100; ++i) {
      CHECK(results[i].get() == i * i);
    }
  }

  SUBCASE("Parallel tasks") {
    std::vector<std::future<int>> results;

    for (int i = 0; i < 100; ++i) {
      results.push_back(pool.submit([i]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return i * i;
      }));
    }

    for (int i = 0; i < 100; ++i) {
      CHECK(results[i].get() == i * i);
    }
  }
}

TEST_CASE("Testing threadsafe_queue functionality") {
  parallel::threadsafe_queue<int> queue;

  SUBCASE("Push and pop single value") {
    queue.push(42);
    auto value = queue.try_pop();
    CHECK(value);
    CHECK(*value == 42);
    CHECK(queue.empty());
  }

  SUBCASE("Push and pop multiple values") {
    for (int i = 0; i < 100; ++i) {
      queue.push(i);
    }

    for (int i = 0; i < 100; ++i) {
      auto value = queue.try_pop();
      CHECK(value);
      CHECK(*value == i);
    }

    CHECK(queue.empty());
  }
}
