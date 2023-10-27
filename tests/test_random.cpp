#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "random.hpp"

#include <iostream>

TEST_CASE("test basic distributions") {

  std::mt19937 gen(0);

  SUBCASE("random_int [0,1]") {
    for (int i = 0; i < 100; ++i) {
      auto r = utils::random_int(gen);
      CHECK(r >= 0);
      CHECK(r <= 1);
    }
  }

  SUBCASE("random_int [0,10)") {
    for (int i = 0; i < 100; ++i) {
      auto r = utils::random_int(0, 10, gen);
      CHECK(r >= 0);
      CHECK(r <= 10);
    }
  }

  SUBCASE("random_double [0,1)") {
    for (int i = 0; i < 100; ++i) {
      auto r = utils::random_double(gen);
      CHECK(r >= 0);
      CHECK(r < 1);
    }
  }

  SUBCASE("random_double [0 10)") {
    for (int i = 0; i < 100; ++i) {
      auto r = utils::random_double(0, 10, gen);
      CHECK(r >= 0);
      CHECK(r < 10);
    }
  }
}

TEST_CASE("test shuffle") {

  std::mt19937 gen(0);

  std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};

  utils::random_shuffle(v, gen);

  CHECK(v == std::vector<int>({3, 2, 8, 7, 1, 6, 4, 5}));
}

TEST_CASE("test shuffled iota") {

  std::mt19937 gen(0);

  auto v = utils::shuffled_iota(10, gen);

  CHECK(v == std::vector<std::size_t>({8, 6, 7, 3, 9, 1, 2, 5, 0, 4}));
}

TEST_CASE("test shuffled mapping") {

  std::mt19937 gen(0);

  auto mapping = utils::shuffled_mapping(10, gen);

  CHECK(mapping == std::unordered_map<std::size_t, std::size_t>({{9, 4},
                                                                 {8, 0},
                                                                 {7, 5},
                                                                 {6, 2},
                                                                 {5, 1},
                                                                 {4, 9},
                                                                 {3, 3},
                                                                 {2, 7},
                                                                 {1, 6},
                                                                 {0, 8}}));
}
