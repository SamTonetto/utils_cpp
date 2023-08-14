#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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
