#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "print.hpp"

#include <sstream>

TEST_CASE("print") {
  std::vector<int> v{1, 2, 3, 4, 5};
  std::set<int> s{1, 2, 3, 4, 5};
  std::map<int, int> m{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}};
  std::unordered_map<int, int> um{{1, 1}, {2, 2}};
  std::unordered_set<int> us{1, 2};

  std::vector<std::map<int, int>> v_m{{{1, 1}}, {{2, 2}}};

  SUBCASE("vector") {
    std::stringstream ss;
    ss << v;
    CHECK(ss.str() == "[1, 2, 3, 4, 5]");
  }

  SUBCASE("set") {
    std::stringstream ss;
    ss << s;
    CHECK(ss.str() == "{1, 2, 3, 4, 5}");
  }

  SUBCASE("map") {
    std::stringstream ss;
    ss << m;
    CHECK(ss.str() == "{1: 1, 2: 2, 3: 3, 4: 4, 5: 5}");
  }

  SUBCASE("unordered_map") {
    std::stringstream ss;
    ss << um;

    bool is_correct = ss.str() == "{1: 1, 2: 2}" || ss.str() == "{2: 2, 1: 1}";
    CHECK(is_correct);
  }

  SUBCASE("unordered_set") {
    std::stringstream ss;
    ss << us;

    bool is_correct = ss.str() == "{1, 2}" || ss.str() == "{2, 1}";
    CHECK(is_correct);
  }

  SUBCASE("vector of map") {
    std::stringstream ss;
    ss << v_m;

    CHECK(ss.str() == "[{1: 1}, {2: 2}]");
  }
}
