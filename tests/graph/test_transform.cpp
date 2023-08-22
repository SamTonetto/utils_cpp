#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "graph/library.hpp"
#include "graph/transforms.hpp"

using namespace utils;

TEST_CASE("test remove edges") {

  auto gb = gl::grid(20, 20);

  std::size_t n_edges_before = boost::num_edges(gb.graph);

  double frac_to_remove = 0.01;
  unsigned seed = 0;
  gl::randomly_remove_edges_without_disconnecting(gb, frac_to_remove, seed);

  std::size_t n_edges_after = boost::num_edges(gb.graph);

  CHECK(std::abs((double)n_edges_after / n_edges_before - 0.9) < 0.1);
}

TEST_CASE("test remove vertices") {
  auto gb = gl::grid(9, 9);

  std::size_t n_vertices_before = boost::num_vertices(gb.graph);

  double frac_to_remove = 0.01;
  unsigned seed = 0;
  gl::randomly_remove_vertices_without_disconnecting(gb, frac_to_remove, seed);

  std::size_t n_vertices_after = boost::num_vertices(gb.graph);

  CHECK(std::abs((double)n_vertices_after / n_vertices_before - 0.99) < 0.1);
}

TEST_CASE("test remove vertices by list") {

  auto gb = gl::grid(3, 3);

  CHECK(boost::num_vertices(gb.graph) == 9);
  CHECK(boost::num_edges(gb.graph) == 12);

  std::vector<std::size_t> to_remove = {0, 1, 3, 5};
  gl::remove_vertices(gb, to_remove);

  CHECK(boost::num_vertices(gb.graph) == 5);
  CHECK(boost::num_edges(gb.graph) == 3);

  CHECK(boost::edge(1, 3, gb.graph).second);
  CHECK(boost::edge(2, 3, gb.graph).second);
  CHECK(boost::edge(3, 4, gb.graph).second);

  CHECK(gb["transforms"][0]["removed_vertices"] ==
        std::vector<std::size_t>({0, 1, 3, 5}));
  CHECK(gb["transforms"][0]["removed_vertices_count"] == 4);
  CHECK(gb["transforms"][0]["removed_vertices_frac"] == 4.0 / 9.0);

  std::unordered_map<std::size_t, std::size_t> index_map =
      gb["transforms"][0]["index_map"];

  CHECK(index_map == std::unordered_map<std::size_t, std::size_t>{
                         {0, 2}, {1, 4}, {2, 6}, {3, 7}, {4, 8}});
}

TEST_CASE("test remove edges by list") {

  auto gb = gl::grid(3, 3);

  CHECK(boost::num_vertices(gb.graph) == 9);
  CHECK(boost::num_edges(gb.graph) == 12);

  std::vector<std::array<std::size_t, 2>> to_remove = {
      {0, 1}, {1, 2}, {1, 4}, {6, 7}};
  gl::remove_edges(gb, to_remove);

  CHECK(boost::num_vertices(gb.graph) == 9);
  CHECK(boost::num_edges(gb.graph) == 8);

  CHECK(boost::edge(0, 3, gb.graph).second);
  CHECK(boost::edge(2, 5, gb.graph).second);
  CHECK(boost::edge(3, 4, gb.graph).second);
  CHECK(boost::edge(4, 5, gb.graph).second);
  CHECK(boost::edge(3, 6, gb.graph).second);
  CHECK(boost::edge(4, 7, gb.graph).second);
  CHECK(boost::edge(5, 8, gb.graph).second);
  CHECK(boost::edge(7, 8, gb.graph).second);

  CHECK(
      gb["transforms"][0]["removed_edges"] ==
      std::vector<std::array<std::size_t, 2>>{{0, 1}, {1, 2}, {1, 4}, {6, 7}});

  CHECK(gb["transforms"][0]["removed_edges_count"] == 4);
  CHECK(gb["transforms"][0]["removed_edges_frac"] == 4.0 / 12.0);
}
