#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/parallel/threadsafe_queue.hpp"

using namespace utils;

TEST_CASE("Test thread safe queue push and try_pop") {
  parallel::threadsafe_queue<int> queue;

  SUBCASE("Single thread push and try_pop") {
    queue.push(1);
    queue.push(2);
    queue.push(3);

    CHECK(*queue.try_pop() == 1);
    CHECK(*queue.try_pop() == 2);
    CHECK(*queue.try_pop() == 3);
  }

  SUBCASE("Multi thread push") {
    std::thread t1([&queue] {
      for (int i = 0; i < 100; ++i) {
        queue.push(i);
      }
    });

    std::thread t2([&queue] {
      for (int i = 100; i < 200; ++i) {
        queue.push(i);
      }
    });

    t1.join();
    t2.join();

    int count = 0;
    while (auto val = queue.try_pop()) {
      ++count;
    }
    CHECK(count == 200);
  }

  SUBCASE("Multi thread push and try_pop") {
    parallel::threadsafe_queue<int> queue;
    std::atomic<int> sum(0);

    std::thread producer([&queue] {
      for (int i = 1; i <= 100; ++i) {
        queue.push(i);
      }
    });

    producer.join();

    std::thread consumer([&queue, &sum] {
      for (int i = 0; i < 100; ++i) {
        while (auto val = queue.try_pop()) {
          sum += *val;
        }
      }
    });

    consumer.join();

    CHECK(sum == 5050); // Sum of first 100 natural numbers
  }
}
