#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "print.hpp"

#include <sstream>

TEST_CASE("print") {
  std::stringstream ss;

  SUBCASE("vector") {
    std::vector<int> v{1, 2, 3, 4, 5};

    ss << v;
    CHECK(ss.str() == "[1, 2, 3, 4, 5]");
  }

  SUBCASE("set") {
    std::set<int> s{1, 2, 3, 4, 5};
    ss << s;
    CHECK(ss.str() == "{1, 2, 3, 4, 5}");
  }

  SUBCASE("map") {
    std::map<int, int> m{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}};
    ss << m;
    CHECK(ss.str() == "{1: 1, 2: 2, 3: 3, 4: 4, 5: 5}");
  }

  SUBCASE("unordered_map") {
    std::unordered_map<int, int> um{{1, 1}, {2, 2}};
    ss << um;

    bool is_correct = ss.str() == "{1: 1, 2: 2}" || ss.str() == "{2: 2, 1: 1}";
    CHECK(is_correct);
  }

  SUBCASE("unordered_set") {
    std::unordered_set<int> us{1, 2};
    ss << us;

    bool is_correct = ss.str() == "{1, 2}" || ss.str() == "{2, 1}";
    CHECK(is_correct);
  }

  SUBCASE("vector of map") {
    std::vector<std::map<int, int>> v_m{{{1, 1}}, {{2, 2}}};
    ss << v_m;

    CHECK(ss.str() == "[{1: 1}, {2: 2}]");
  }

  SUBCASE("queue") {
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    ss << q;

    CHECK(ss.str() == "[1, 2, 3]");
  }

  SUBCASE("stack") {
    std::stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    ss << s;

    CHECK(ss.str() == "[3, 2, 1]");
  }

  SUBCASE("priority_queue") {
    std::priority_queue<int> pq;
    pq.push(1);
    pq.push(2);
    pq.push(3);
    ss << pq;

    CHECK(ss.str() == "[3, 2, 1]");
  }
}
