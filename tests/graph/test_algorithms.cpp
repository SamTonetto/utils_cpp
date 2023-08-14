#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "graph/algorithms.hpp"
#include "graph/library.hpp"

#include <boost/graph/graphviz.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <iostream>
#include <sstream>
#include <string>

using namespace utils;

TEST_CASE("floyd warshall undirected") {

  auto gb = gl::grid(2, 2);
  auto &g = gb.graph;

  SUBCASE("int distance") {
    auto distances = gl::floyd_warshall(g);

    std::vector<std::vector<int>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }

  SUBCASE("double distances") {
    auto distances = gl::floyd_warshall<double>(g);

    std::vector<std::vector<double>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }

  SUBCASE("std::size_t distances") {
    auto distances = gl::floyd_warshall<std::size_t>(g);

    std::vector<std::vector<std::size_t>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }
}

TEST_CASE("floyd warshall directed") {

  gl::DiGraph g;

  boost::add_edge(0, 1, g);
  boost::add_edge(0, 2, g);
  boost::add_edge(1, 2, g);
  boost::add_edge(1, 3, g);
  boost::add_edge(1, 4, g);
  boost::add_edge(2, 3, g);
  boost::add_edge(3, 1, g);
  boost::add_edge(3, 4, g);

  auto distances = gl::floyd_warshall(g);

  int inf = std::numeric_limits<int>::max();

  std::vector<std::vector<int>> expected = {{0, 1, 1, 2, 2},
                                            {inf, 0, 1, 1, 1},
                                            {inf, 2, 0, 1, 2},
                                            {inf, 1, 2, 0, 1},
                                            {inf, inf, inf, inf, 0}};

  CHECK(distances == expected);
}

TEST_CASE("floyd warshall bitadjmat") {

  auto gb = gl::grid(2, 2);
  auto &g = gb.graph;

  gl::BitAdjmat adjmat(g);

  SUBCASE("int distance") {
    std::vector<std::vector<int>> distances = gl::floyd_warshall(adjmat);

    std::vector<std::vector<int>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }

  SUBCASE("double distance") {
    auto distances = gl::floyd_warshall<double>(adjmat);

    std::vector<std::vector<double>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }

  SUBCASE("size_t distance") {
    auto distances = gl::floyd_warshall<std::size_t>(adjmat);

    std::vector<std::vector<std::size_t>> expected = {
        {0, 1, 1, 2}, {1, 0, 2, 1}, {1, 2, 0, 1}, {2, 1, 1, 0}};

    CHECK(distances == expected);
  }
}
