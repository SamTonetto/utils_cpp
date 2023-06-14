#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "compiletime.hpp"

#include <iostream>

TEST_CASE("test vec/map/umap") {

  int scalar = 1;
  std::vector<int> vector = {1, 2, 3};
  std::set<int> set = {1, 2, 3};
  std::unordered_set<int> uset = {1, 2, 3};
  std::map<int, int> map = {{1, 1}, {2, 2}, {3, 3}};
  std::unordered_map<int, int> umap = {{1, 1}, {2, 2}, {3, 3}};

  SUBCASE("is vec") {
    CHECK(utils::is_vector<decltype(scalar)>::value == false);
    CHECK(utils::is_vector<decltype(vector)>::value == true);
    CHECK(utils::is_vector<decltype(set)>::value == false);
    CHECK(utils::is_vector<decltype(uset)>::value == false);
    CHECK(utils::is_vector<decltype(map)>::value == false);
    CHECK(utils::is_vector<decltype(umap)>::value == false);
  }

  SUBCASE("is set") {
    CHECK(utils::is_set<decltype(scalar)>::value == false);
    CHECK(utils::is_set<decltype(vector)>::value == false);
    CHECK(utils::is_set<decltype(set)>::value == true);
    CHECK(utils::is_set<decltype(uset)>::value == false);
    CHECK(utils::is_set<decltype(map)>::value == false);
    CHECK(utils::is_set<decltype(umap)>::value == false);
  }

  SUBCASE("is uset") {
    CHECK(utils::is_unordered_set<decltype(scalar)>::value == false);
    CHECK(utils::is_unordered_set<decltype(vector)>::value == false);
    CHECK(utils::is_unordered_set<decltype(set)>::value == false);
    CHECK(utils::is_unordered_set<decltype(uset)>::value == true);
    CHECK(utils::is_unordered_set<decltype(map)>::value == false);
    CHECK(utils::is_unordered_set<decltype(umap)>::value == false);
  }

  SUBCASE("is map") {
    CHECK(utils::is_map<decltype(scalar)>::value == false);
    CHECK(utils::is_map<decltype(vector)>::value == false);
    CHECK(utils::is_map<decltype(set)>::value == false);
    CHECK(utils::is_map<decltype(uset)>::value == false);
    CHECK(utils::is_map<decltype(map)>::value == true);
    CHECK(utils::is_map<decltype(umap)>::value == false);
  }

  SUBCASE("is umap") {
    CHECK(utils::is_unordered_map<decltype(scalar)>::value == false);
    CHECK(utils::is_unordered_map<decltype(vector)>::value == false);
    CHECK(utils::is_unordered_map<decltype(set)>::value == false);
    CHECK(utils::is_unordered_map<decltype(uset)>::value == false);
    CHECK(utils::is_unordered_map<decltype(map)>::value == false);
    CHECK(utils::is_unordered_map<decltype(umap)>::value == true);
  }
}
