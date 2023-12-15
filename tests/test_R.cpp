#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "R.hpp"

using namespace utils;

TEST_CASE("Testing seq function") {
  SUBCASE("Normal ascending sequence") {
    auto result = seq(1, 5);
    CHECK(result == std::vector{1, 2, 3, 4, 5});
  }

  SUBCASE("Descending sequence") {
    auto result = seq(5, 1, -1);
    CHECK(result == std::vector{5, 4, 3, 2, 1});
  }

  SUBCASE("Floating point sequence") {
    auto result = seq(1.0, 2.0, 0.5);
    CHECK(result == std::vector{1.0, 1.5, 2.0});
  }

  SUBCASE("Invalid step size") {
    CHECK_THROWS_AS(seq(1, 5, 0), std::invalid_argument);
  }

  SUBCASE("Large range") {
    auto result = seq(1, 1000, 100);
    CHECK(result.size() == 10);
    CHECK(result.front() == 1);
    CHECK(result.back() == 901);
  }

  SUBCASE("Negative step size") {
    auto result = seq(10, 1, -2);
    CHECK(result == std::vector{10, 8, 6, 4, 2});
  }

  SUBCASE("Step size larger than range") {
    auto result = seq(1, 5, 10);
    CHECK(result == std::vector{1});
  }

  SUBCASE("Start and end are equal") {
    auto result = seq(5, 5);
    CHECK(result == std::vector{5});
  }

  SUBCASE("Empty range") { CHECK_THROWS(seq(5, 4)); }
}

TEST_CASE("Testing rep function") {
  SUBCASE("Repeat scalar value") {
    auto result = rep(42, 3, 2);
    CHECK(result == std::vector{42, 42, 42, 42, 42, 42});
  }

  SUBCASE("Repeat vector") {
    auto result = rep(std::vector{1, 2}, 2, 3);
    CHECK(result == std::vector{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
  }

  SUBCASE("Repeat empty vector") {
    auto result = rep(std::vector<int>{}, 2, 3);
    CHECK(result.empty());
  }

  SUBCASE("Zero times") {
    auto result = rep(5, 0, 2);
    CHECK(result.empty());
  }

  SUBCASE("Zero each") {
    auto result = rep(5, 2, 0);
    CHECK(result.empty());
  }

  SUBCASE("Large times and each") {
    auto result = rep(1, 1000, 1000);
    CHECK(result.size() == 1000000);
    CHECK(result.front() == 1);
    CHECK(result.back() == 1);
  }

  SUBCASE("Non-integer types") {
    auto result = rep(3.14, 3, 2);
    CHECK(result == std::vector{3.14, 3.14, 3.14, 3.14, 3.14, 3.14});
  }
}

TEST_CASE("Testing fapply") {

  auto func = [](int x) -> int { return 2 * x; };

  SUBCASE("vector") {

    std::vector<std::vector<int>> v = {{1, 2}, {3, 4}};

    auto result = fapply(v, func);

    CHECK(result == std::vector<std::vector<int>>{{2, 4}, {6, 8}});
  }
  SUBCASE("pair") {

    std::pair<int, int> p = {1, 2};

    auto result = fapply(p, func);

    CHECK(result == std::pair<int, int>{2, 4});
  }

  SUBCASE("nested vectors and pairs") {
    std::vector<std::pair<std::vector<int>, std::pair<int, int>>> v = {
        {{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};

    auto result = fapply(v, func);

    CHECK(result ==
          std::vector<std::pair<std::vector<int>, std::pair<int, int>>>{
              {{2, 4}, {6, 8}}, {{10, 12}, {14, 16}}});
  }
}
