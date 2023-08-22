#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "bitvector.hpp"

TEST_CASE("create, set and iterate through bitvector") {

  utils::BitVector bv(8);

  bv[0] = 1;
  bv[2] = 1;
  bv[7] = 1;

  SUBCASE("iterate only ones") {
    std::stringstream ss;
    for (std::size_t val : bv) {
      ss << val << ' ';
    }

    CHECK(ss.str() == "0 2 7 ");
  }

  SUBCASE("iterate all") {
    std::stringstream ss;
    for (std::size_t i = 0; i < bv.size(); ++i) {
      ss << bv[i] << ' ';
    }
    CHECK(ss.str() == "1 0 1 0 0 0 0 1 ");
  }
}

TEST_CASE("first bit not set, last bit not set") {

  utils::BitVector bv(8);

  bv[1] = 1;
  bv[5] = 1;

  std::stringstream ss;
  for (std::size_t val : bv) {
    ss << val << ' ';
  }

  CHECK(ss.str() == "1 5 ");
}

TEST_CASE("over size 64") {

  utils::BitVector bv(72);

  SUBCASE("first and last bit not set") {

    bv[1] = 1;
    bv[68] = 1;

    std::stringstream ss;
    for (std::size_t val : bv) {
      ss << val << ' ';
    }
    CHECK(ss.str() == "1 68 ");
  }

  SUBCASE("first and last bit set") {

    bv[0] = 1;
    bv[50] = 1;
    bv[71] = 1;

    std::stringstream ss;
    for (std::size_t val : bv) {
      ss << val << ' ';
    }
    CHECK(ss.str() == "0 50 71 ");
  }
}
