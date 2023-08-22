#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "graph/graph.hpp"
#include "graph/library.hpp"
#include "graph/pathfinding.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace utils;

TEST_CASE("test_bfs") {
  auto gb = gl::ibm_hex(2, 2);
  auto &graph = gb.graph;
  int source = 3;

  std::vector<int> correct_distances = {3, 2,  1, 0, 1, 2,  3, 4, 5, 6, 7, 8,
                                        7, 6,  5, 4, 5, 6,  5, 4, 5, 6, 7, 8,
                                        9, 10, 9, 8, 9, 10, 9, 8, 7, 3, 7};

  std::vector<gl::Vertex<gl::Graph>> correct_predecessors = {
      1,  2, 3,  0,  3,  4,  5,  6,  7,  8,  9,  10, 13, 14, 15, 33, 15, 18,
      19, 0, 19, 20, 21, 22, 23, 26, 27, 34, 27, 28, 31, 32, 13, 5,  17};

  SUBCASE("distances only") {
    std::vector<int> distances = gl::bfs_distances(graph, source);
    CHECK(distances == correct_distances);
  }
  SUBCASE("predecessors only") {
    std::vector<gl::Vertex<gl::Graph>> predecessors =
        gl::bfs_predecessors(graph, source);
    CHECK(predecessors == correct_predecessors);
  }

  SUBCASE("distances and predecessors") {
    auto [distances, predecessors] = gl::bfs(graph, source);
    CHECK(distances == correct_distances);
    CHECK(predecessors == correct_predecessors);
  }
}

TEST_CASE("test_dijkstra") {
  auto gb = gl::ibm_hex(2, 2);
  auto &graph = gb.graph;
  int source = 3;

  using WeightType = int;

  long long seed = 0;
  std::mt19937 rng(seed);

  // I wanted to use std::uniform_int_distribution but it gives different
  // results on different platforms. So instead, just use the fact that
  // std::mt19937 produces uniform random numbers in [0,2^(32)-1], so we can
  // just get random integers in {0,1} by taking the last bit of the random
  // number.
  gl::EdgeMap<WeightType, gl::Graph> weights;
  for (auto e : boost::make_iterator_range(boost::edges(graph))) {
    weights[e] = rng() & 1;
  }

  std::vector<WeightType> correct_distances = {
      1, 1, 1, 0, 1, 1, 2, 3, 4, 5, 5, 4, 3, 3, 3, 2, 2, 2,
      2, 2, 2, 3, 3, 4, 4, 4, 4, 3, 4, 5, 4, 4, 3, 2, 3

  };

  std::vector<gl::Vertex<gl::Graph>> correct_predecessors = {
      1,  2, 3,  3,  3,  4,  5,  6,  7,  8,  11, 12, 13, 14, 15, 16, 17, 18,
      19, 0, 19, 20, 21, 22, 25, 26, 27, 34, 27, 28, 31, 32, 13, 5,  17};

  SUBCASE("distances only") {
    std::vector<WeightType> distances =
        gl::dijkstra_distances(graph, source, weights);
    CHECK(distances == correct_distances);
  }
  SUBCASE("predecessors only") {
    std::vector<gl::Vertex<gl::Graph>> predecessors =
        gl::dijkstra_predecessors(graph, source, weights);
    CHECK(predecessors == correct_predecessors);
  }

  SUBCASE("distances and predecessors") {
    auto [distances, predecessors] = gl::dijkstra(graph, source, weights);
    CHECK(distances == correct_distances);
    CHECK(predecessors == correct_predecessors);
  }
}
