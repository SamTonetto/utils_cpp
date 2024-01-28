#include <optional>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/timing.hpp"

#include <iostream>

void wait_1us() { std::this_thread::sleep_for(std::chrono::microseconds(1)); }

std::pair<int, int> wait_1us_and_increment(int x) {
  wait_1us();
  return {x, x + 1};
}

int test_increment(int x) { return x + 1; }

TEST_CASE("test timing a void function") {

  auto timed_function = utils::time(wait_1us);

  auto [result, duration] = timed_function();

  CHECK(!result.has_value());
  CHECK(duration > 0);
}

TEST_CASE("test timing a function with return value") {

  auto timed_function = utils::time(wait_1us_and_increment);

  int x = 1;
  auto [result, duration] = timed_function(x);

  CHECK(result.has_value());
  CHECK(result.value() == std::make_pair(x, x + 1));
  CHECK(duration > 0);
}
