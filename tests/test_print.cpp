#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/hash.hpp"
#include "utils_cpp/print.hpp"

#include <sstream>

using utils::operator<<;

TEST_CASE("print") {
  std::stringstream ss;

  SUBCASE("vector") {
    std::vector<int> v{1, 2, 3, 4, 5};

    ss << v;
    CHECK(ss.str() == "[1, 2, 3, 4, 5]");
  }

  SUBCASE("array") {
    std::array<int, 5> v{1, 2, 3, 4, 5};

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

  SUBCASE("pair") {
    std::pair<int, int> p{1, 2};
    ss << p;

    CHECK(ss.str() == "(1, 2)");
  }

  SUBCASE("vector of map") {
    std::vector<std::map<int, int>> v_m{{{1, 1}}, {{2, 2}}};
    ss << v_m;

    CHECK(ss.str() == "[{1: 1}, {2: 2}]");
  }

  SUBCASE("unordered_map of pair to double") {
    std::unordered_map<std::pair<int, int>, double,
                       utils::symmetric_pair_hash<int, int>>
        um_p_d;

    um_p_d[{1, 2}] = 1.0;
    um_p_d[{2, 1}] = 2.0;

    ss << um_p_d;

    bool is_correct = ss.str() == "{(1, 2): 1, (2, 1): 2}" ||
                      ss.str() == "{(2, 1): 2, (1, 2): 1}";

    CHECK(is_correct);
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

TEST_CASE("test_newline_io_manip") {

  std::vector<std::vector<std::set<int>>> v = {
      {{0, 1, 2, 3}, {0, 2, 3, 5}},
      {{0, 2}},
      {{1, 2, 3, 5}, {2, 3}, {1, 2, 3}},
      {{3}, {}},
      {}};

  SUBCASE("normal") {

    std::stringstream ss;
    ss << v;

    CHECK(ss.str() == "[[{0, 1, 2, 3}, {0, 2, 3, 5}], [{0, 2}], "
                      "[{1, 2, 3, 5}, {2, 3}, {1, 2, 3}], [{3}, {}], []]");
  }

  SUBCASE("newline") {
    std::stringstream ss;
    ss << utils::prettyprint(true) << v;

    CHECK(ss.str() ==
          "[\n  [\n    {0, 1, 2, 3},\n    {0, 2, 3, 5}\n  ],\n  [\n    "
          "{0, 2}\n  ],\n  [\n    {1, 2, 3, 5},\n    {2, 3},\n    {1, 2, 3}\n  "
          "],\n  "
          "[\n    {3},\n    "
          "{}\n  ],\n  []\n]");
  }
}

TEST_CASE("test_print_binary") {

  SUBCASE("uint8_t") {

    uint8_t x = static_cast<uint8_t>(1) << 3;
    uint8_t y = static_cast<uint8_t>(1) << 7;

    CHECK(utils::to_binary_string(x) == "00001000");
    CHECK(utils::to_binary_string(y) == "10000000");
  }

  SUBCASE("uint16_t") {

    uint16_t x = static_cast<uint16_t>(1) << 3;
    uint16_t y = static_cast<uint16_t>(1) << 15;

    CHECK(utils::to_binary_string(x) == "0000000000001000");
    CHECK(utils::to_binary_string(y) == "1000000000000000");
  }

  SUBCASE("uint32_t") {

    uint32_t x = static_cast<uint32_t>(1) << 3;
    uint32_t y = static_cast<uint32_t>(1) << 31;

    CHECK(utils::to_binary_string(x) == "00000000000000000000000000001000");
    CHECK(utils::to_binary_string(y) == "10000000000000000000000000000000");
  }

  SUBCASE("uint64_t") {

    uint64_t x = static_cast<uint64_t>(1) << 3;
    uint64_t y = static_cast<uint64_t>(1) << 63;

    CHECK(utils::to_binary_string(x) ==
          "0000000000000000000000000000000000000000000000000000000000001000");
    CHECK(utils::to_binary_string(y) ==
          "1000000000000000000000000000000000000000000000000000000000000000");
  }
}

TEST_CASE("escape special chars") {

  std::stringstream ss;
  ss << utils::escape_special_chars(true) << "hello\nworld" << '\t';
  ss << utils::escape_special_chars(false) << "hello\nworld" << '\t';

  CHECK(ss.str() == "hello\\nworld\\thello\nworld\t");
}

TEST_CASE("test print tuple") {

  std::tuple<int, std::string, std::vector<std::tuple<int, std::string>>> t = {
      1,
      "hello",
      {
          {1, "a"},
          {2, "b"},
          {3, "c"},
      }};

  std::stringstream ss;
  ss << t;

  CHECK(ss.str() == "(1, hello, [(1, a), (2, b), (3, c)])");
}

TEST_CASE("test enum") {

  SUBCASE("scoped enum") {

    enum class MyEnum { a, b, c };

    std::stringstream ss;

    ss << MyEnum::a << ' ' << MyEnum::b << ' ' << MyEnum::c;

    CHECK(ss.str() == "MyEnum::a MyEnum::b MyEnum::c");
  }
}
