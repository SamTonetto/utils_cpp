#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "compiletime.hpp"

#include <iostream>

TEST_CASE("Testing is_same_container") {
  SUBCASE("Same container types") {
    std::vector<int> v;
    std::unordered_map<int, std::string> m;

    CHECK(utils::is_same_container_v<std::vector, decltype(v)>);
    CHECK(utils::is_same_container_v<std::unordered_map, decltype(m)>);
  }

  SUBCASE("Different container types") {
    std::vector<int> v;
    std::list<int> l;

    CHECK_FALSE(utils::is_same_container_v<std::vector, decltype(l)>);
    CHECK_FALSE(utils::is_same_container_v<std::list, decltype(v)>);
  }

  SUBCASE("Non-container types") {
    int i;
    std::string s;

    CHECK_FALSE(utils::is_same_container_v<std::vector, decltype(i)>);
    CHECK_FALSE(utils::is_same_container_v<std::list, decltype(s)>);
  }
}
