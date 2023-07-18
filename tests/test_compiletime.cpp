#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "compiletime.hpp"

#include <iostream>

TEST_CASE("test vec/map/umap") {

  int scalar = 1;
  std::vector<int> vector = {1, 2, 3};
  std::set<int> set = {1, 2, 3};
  std::pair<int, int> pair = {1, 2};
  std::unordered_set<int> uset = {1, 2, 3};
  std::map<int, int> map = {{1, 1}, {2, 2}, {3, 3}};
  std::unordered_map<int, int> umap = {{1, 1}, {2, 2}, {3, 3}};

  SUBCASE("is pair") {

    CHECK(utils::is_same_container_v<std::pair, decltype(scalar)> == false);
    CHECK(utils::is_same_container_v<std::pair, decltype(vector)> == false);
    CHECK(utils::is_same_container_v<std::pair, decltype(pair)> == true);
    CHECK(utils::is_same_container_v<std::pair, decltype(set)> == false);
    CHECK(utils::is_same_container_v<std::pair, decltype(uset)> == false);
    CHECK(utils::is_same_container_v<std::pair, decltype(map)> == false);
    CHECK(utils::is_same_container_v<std::pair, decltype(umap)> == false);
  }
}
