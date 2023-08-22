#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "random.hpp"

#include <iostream>

TEST_CASE("basic tests") {

  std::mt19937 gen(0);

  SUBCASE("random_int [0,1]") {
    for (int i = 0; i < 10; ++i) {
      auto r = utils::random_int(gen);
      CHECK(r >= 0);
      CHECK(r <= 1);
    }
  }

  SUBCASE("random_int [0,10)") {
    for (int i = 0; i < 10; ++i) {
      auto r = utils::random_int(gen, 0, 10);
      CHECK(r >= 0);
      CHECK(r <= 10);
    }
  }

  SUBCASE("random_double [0,1)") {
    for (int i = 0; i < 10; ++i) {
      auto r = utils::random_double(gen);
      CHECK(r >= 0);
      CHECK(r < 1);
    }
  }

  SUBCASE("random_double [0 10)") {
    for (int i = 0; i < 10; ++i) {
      auto r = utils::random_double(gen, 0, 10);
      CHECK(r >= 0);
      CHECK(r < 10);
    }
  }
}
