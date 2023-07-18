#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "print.hpp"

TEST_CASE("test_vector_print") {

  std::vector<std::vector<std::set<int>>> v = {
      {{0, 1, 2, 3}, {0, 2, 3, 5}},
      {{0, 2}},
      {{1, 2, 3, 5}, {2, 3}, {1, 2, 3}},
      {{3}, {}},
      {}};

  SUBCASE("normal") {

    std::stringstream ss;
    ss << v;

    std::cout << ss.str() << std::endl;

    CHECK(ss.str() == "[[{0, 1, 2, 3}, {0, 2, 3, 5}], [{0, 2}], "
                      "[{1, 2, 3, 5}, {2, 3}, {1, 2, 3}], [{3}, {}], []]");
  }

  SUBCASE("newline") {
    std::stringstream ss;
    ss << utils::prettyprint << v;

    std::cout << ss.str() << std::endl;

    CHECK(ss.str() ==
          "[\n  [\n    {0, 1, 2, 3},\n    {0, 2, 3, 5}\n  ],\n  [\n    "
          "{0, 2}\n  ],\n  [\n    {1, 2, 3, 5},\n    {2, 3},\n    {1, 2, 3}\n  "
          "],\n  "
          "[\n    {3},\n    "
          "{}\n  ],\n  []\n]");
  }
}
