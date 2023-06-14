#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "hash.hpp"

#include <iostream>

TEST_CASE("test basic hashes") {

  // If this even compiles at all, it basically "passes" the test.
  SUBCASE("in an unordered_map") {
    std::unordered_map<std::pair<int, int>, int, utils::pair_hash<int, int>> x1;
    std::unordered_map<std::vector<int>, int, utils::vector_hash<int>> x2;
    std::unordered_map<std::set<int>, int, utils::set_hash<int>> x3;
    std::unordered_map<std::unordered_set<int>, int,
                       utils::unordered_set_hash<int>>
        x4;
    std::unordered_map<std::map<int, int>, int, utils::map_hash<int, int>> x5;

    std::unordered_map<std::unordered_map<int, int>, int,
                       utils::unordered_map_hash<int, int>>
        x6;
  }

  SUBCASE("in a unordered_set") {
    std::unordered_set<std::pair<int, int>, utils::pair_hash<int, int>> x1;
    std::unordered_set<std::vector<int>, utils::vector_hash<int>> x2;
    std::unordered_set<std::set<int>, utils::set_hash<int>> x3;
    std::unordered_set<std::unordered_set<int>, utils::unordered_set_hash<int>>
        x4;
    std::unordered_set<std::map<int, int>, utils::map_hash<int, int>> x5;

    std::unordered_set<std::unordered_map<int, int>,
                       utils::unordered_map_hash<int, int>>
        x6;
  }
}
