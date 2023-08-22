#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "numpy.hpp"

#include <vector>

using namespace utils;

TEST_CASE("test arange") {

  SUBCASE("one param") {

    auto range = arange{10};
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  SUBCASE("two params") {

    auto range = arange(5, 10);
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>({5, 6, 7, 8, 9}));
  }

  SUBCASE("three params") {

    auto range = arange(5, 10, 2);
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>({5, 7, 9}));
  }

  SUBCASE("three params, negative step") {

    auto range = arange(10, 5, -2);
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>({10, 8, 6}));
  }

  SUBCASE("last value not included") {

    auto range = arange(6, 10, 2);
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>({6, 8}));
  }

  SUBCASE("last value not included - double") {

    auto range = arange(6.0, 10.0, 2.0);
    std::vector<double> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<double>({6.0, 8.0}));
  }

  SUBCASE("empty range") {

    auto range = arange(10, 5, 2);
    std::vector<int> v;
    for (auto i : range) {
      v.push_back(i);
    }
    CHECK(v == std::vector<int>());
  }
}
