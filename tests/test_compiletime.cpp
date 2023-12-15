#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "compiletime.hpp"

#include <iostream>

struct test_struct1 {};

template <typename T>
struct test_struct2 {};

TEST_CASE("Testing is_instantiation") {
  SUBCASE("Same container types") {
    std::vector<int> v;
    std::unordered_map<int, std::string> m;

    CHECK(utils::is_instantiation<std::vector, decltype(v)>() == true);
    CHECK(utils::is_instantiation<std::unordered_map, decltype(m)>() == true);
  }

  SUBCASE("Different container types") {
    std::vector<int> v;
    std::list<int> l;

    CHECK_FALSE(utils::is_instantiation<std::vector, decltype(l)>());
    CHECK_FALSE(utils::is_instantiation<std::list, decltype(v)>());
  }

  SUBCASE("Non-container types") {
    int i;
    std::string s;

    CHECK_FALSE(utils::is_instantiation<std::vector, decltype(i)>());
    CHECK_FALSE(utils::is_instantiation<std::list, decltype(s)>());
  }

  SUBCASE("test custom container") {
    CHECK_FALSE(utils::is_instantiation<std::vector, test_struct1>());
    CHECK_FALSE(utils::is_instantiation<std::list, test_struct2<int>>());
    CHECK_FALSE(utils::is_instantiation<std::queue, test_struct2<int>>());
  }

  SUBCASE("pair") {
    std::pair<int, int> p;
    CHECK(utils::is_instantiation<std::pair, decltype(p)>());
    CHECK(utils::is_instantiation<std::pair, std::pair<int, int>>());
    CHECK(utils::is_instantiation<std::pair, std::pair<int, std::string>>());

    CHECK_FALSE(utils::is_instantiation<std::pair, int>());
  }
}

TEST_CASE("is stl container") {

  CHECK(utils::is_stl_container<std::array<int, 2>>());
  CHECK(utils::is_stl_container<std::bitset<5>>());
  CHECK(utils::is_stl_container<std::deque<int>>());
  CHECK(utils::is_stl_container<std::forward_list<int>>());
  CHECK(utils::is_stl_container<std::list<int>>());
  CHECK(utils::is_stl_container<std::map<int, int>>());
  CHECK(utils::is_stl_container<std::multimap<int, int>>());
  CHECK(utils::is_stl_container<std::multiset<int>>());
  CHECK(utils::is_stl_container<std::pair<int, int>>());
  CHECK(utils::is_stl_container<std::priority_queue<int>>());
  CHECK(utils::is_stl_container<std::queue<int>>());
  CHECK(utils::is_stl_container<std::set<int>>());
  CHECK(utils::is_stl_container<std::span<int>>());
  CHECK(utils::is_stl_container<std::stack<int>>());
  CHECK(utils::is_stl_container<std::tuple<int>>());
  CHECK(utils::is_stl_container<std::unordered_map<int, int>>());
  CHECK(utils::is_stl_container<std::unordered_multimap<int, int>>());
  CHECK(utils::is_stl_container<std::unordered_multiset<int>>());
  CHECK(utils::is_stl_container<std::unordered_set<int>>());
  CHECK(utils::is_stl_container<std::valarray<int>>());
  CHECK(utils::is_stl_container<std::vector<int>>());
  CHECK(utils::is_stl_container<std::variant<int, std::vector<double>>>());

  CHECK(utils::is_stl_container<test_struct1>() == false);
  CHECK(utils::is_stl_container<test_struct2<int>>() == false);
}
