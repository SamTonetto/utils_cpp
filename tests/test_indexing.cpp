#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/indexing.hpp"

using namespace utils;

TEST_CASE("testing apply_index_mapping") {
  std::unordered_map<std::size_t, std::size_t> mapping = {
      {1, 10}, {2, 20}, {3, 30}, {4, 40}};

  std::vector<std::size_t> vec1 = {1, 2, 3, 4};
  auto mappedVec1 = apply_index_mapping(vec1, mapping);
  CHECK(mappedVec1 == std::vector<std::size_t>{10, 20, 30, 40});

  std::vector<std::vector<std::size_t>> vec2 = {{1, 2}, {3, 4}};
  auto mappedVec2 = apply_index_mapping(vec2, mapping);
  CHECK(mappedVec2 ==
        std::vector<std::vector<std::size_t>>{{10, 20}, {30, 40}});
}
