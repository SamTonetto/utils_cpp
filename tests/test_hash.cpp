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

TEST_CASE("test with equality") {

  std::unordered_set<std::pair<int, int>, utils::symmetric_pair_hash<int, int>,
                     utils::symmetric_pair_equal<int, int>>
      uset;

  uset.insert({0, 1});

  CHECK(uset.contains({0, 1}));
  CHECK(uset.contains({1, 0}));

  std::unordered_map<std::pair<int, int>, int,
                     utils::symmetric_pair_hash<int, int>,
                     utils::symmetric_pair_equal<int, int>>
      umap;

  umap.insert({{0, 1}, 1});

  CHECK(uset.contains({0, 1}));
  CHECK(uset.contains({1, 0}));

  std::unordered_set<std::vector<int>, utils::symmetric_vector_hash<int>,
                     utils::symmetric_vector_equal<int>>
      vec_uset;

  vec_uset.insert({0, 1, 2});

  CHECK(vec_uset.contains({0, 1, 2}));
  CHECK(vec_uset.contains({0, 2, 1}));
  CHECK(vec_uset.contains({1, 0, 2}));
  CHECK(vec_uset.contains({1, 2, 0}));
  CHECK(vec_uset.contains({2, 0, 1}));
  CHECK(vec_uset.contains({2, 1, 0}));
  CHECK(vec_uset.contains({2, 1}) == false);
  CHECK(vec_uset.contains({0, 1, 2, 3}) == false);

  std::unordered_set<std::array<int, 3>, utils::symmetric_array_hash<int, 3>,
                     utils::symmetric_array_equal<int, 3>>
      array_uset;

  array_uset.insert(std::array<int, 3>({0, 1, 2}));

  CHECK(array_uset.contains(std::array<int, 3>({0, 1, 2})));
  CHECK(array_uset.contains(std::array<int, 3>({0, 2, 1})));
  CHECK(array_uset.contains(std::array<int, 3>({1, 0, 2})));
  CHECK(array_uset.contains(std::array<int, 3>({1, 2, 0})));
  CHECK(array_uset.contains(std::array<int, 3>({2, 0, 1})));
  CHECK(array_uset.contains(std::array<int, 3>({2, 1, 0})));
  CHECK(array_uset.contains(std::array<int, 3>({0, 2, 3})) == false);
}
