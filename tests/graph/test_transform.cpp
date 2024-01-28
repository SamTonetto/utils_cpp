#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/graph/library.hpp"
#include "utils_cpp/graph/transforms.hpp"

using namespace utils;

#include "utils_cpp/graph/graphviz.hpp"

TEST_CASE("test remove") {

  auto gb = gl::grid(3, 3);

  auto new_gb = gl::remove_vertices(gb, {0, 1, 3, 5});

  gl::VertexMap<std::vector<double>> pos = gb.props.vertex["position"];
  std::cout << "pos = " << pos << std::endl;

  utils::gl::to_dot(new_gb, "test_remove.dot");
}

TEST_CASE("test remove edges") {

  auto gb = gl::grid(20, 20);

  double frac_to_remove = 0.1;
  unsigned seed = 0;

  auto new_gb = gl::rand_prune_edges_connected(gb, frac_to_remove, seed);

  CHECK(std::abs((double)boost::num_edges(new_gb.graph) /
                     boost::num_edges(gb.graph) -
                 0.9) < 0.1);
}

TEST_CASE("test remove vertices") {
  auto gb = gl::grid(9, 9);

  double frac_to_remove = 0.1;
  unsigned seed = 0;

  auto new_gb = gl::rand_prune_connected(gb, frac_to_remove, seed);

  CHECK(std::abs((double)boost::num_vertices(new_gb.graph) /
                     boost::num_vertices(gb.graph) -
                 0.99) < 0.1);
}

TEST_CASE("test remove vertices by list") {

  auto gb = gl::grid(3, 3);

  CHECK(boost::num_vertices(gb.graph) == 9);
  CHECK(boost::num_edges(gb.graph) == 12);

  std::vector<std::size_t> to_remove = {0, 1, 3, 5};
  auto new_gb = gl::remove_vertices(gb, to_remove);

  CHECK(boost::num_vertices(new_gb.graph) == 5);
  CHECK(boost::num_edges(new_gb.graph) == 3);

  CHECK(boost::edge(1, 3, new_gb.graph).second);
  CHECK(boost::edge(2, 3, new_gb.graph).second);
  CHECK(boost::edge(3, 4, new_gb.graph).second);

  CHECK(new_gb["removed_vertices"] == std::vector<double>({0, 1, 3, 5}));
}

TEST_CASE("test remove edges by list") {

  auto gb = gl::grid(3, 3);

  CHECK(boost::num_vertices(gb.graph) == 9);
  CHECK(boost::num_edges(gb.graph) == 12);

  std::vector<std::vector<std::size_t>> to_remove = {
      {0, 1}, {1, 2}, {1, 4}, {6, 7}};
  auto new_gb = gl::remove_edges(gb, to_remove);

  CHECK(boost::num_vertices(new_gb.graph) == 9);
  CHECK(boost::num_edges(new_gb.graph) == 8);

  CHECK(boost::edge(0, 3, new_gb.graph).second);
  CHECK(boost::edge(2, 5, new_gb.graph).second);
  CHECK(boost::edge(3, 4, new_gb.graph).second);
  CHECK(boost::edge(4, 5, new_gb.graph).second);
  CHECK(boost::edge(3, 6, new_gb.graph).second);
  CHECK(boost::edge(4, 7, new_gb.graph).second);
  CHECK(boost::edge(5, 8, new_gb.graph).second);
  CHECK(boost::edge(7, 8, new_gb.graph).second);

  CHECK(new_gb["removed_edges"] ==
        std::vector<std::vector<double>>{{0, 1}, {1, 2}, {1, 4}, {6, 7}});
}

TEST_CASE("gl::relabel_vertices") {

  gl::Graph g;

  SUBCASE("valid mapping") {

    boost::add_edge(0, 1, g);
    boost::add_edge(1, 2, g);

    std::unordered_map<std::size_t, std::size_t> mapping = {
        {0, 2}, {1, 0}, {2, 1}};

    gl::Graph new_g = gl::relabel_vertices(g, mapping);

    CHECK(boost::edge(2, 0, new_g).second);
    CHECK(boost::edge(0, 1, new_g).second);
  }

  SUBCASE("incomplete mapping") {

    boost::add_edge(0, 1, g);
    boost::add_edge(1, 2, g);

    std::unordered_map<std::size_t, std::size_t> mapping = {{0, 2}, {1, 0}};
    CHECK_THROWS(gl::relabel_vertices(g, mapping));
  }

  SUBCASE("mapping with larger index") {

    boost::add_edge(0, 1, g);

    std::unordered_map<std::size_t, std::size_t> mapping = {{0, 2}, {1, 3}};

    gl::Graph new_graph = gl::relabel_vertices(g, mapping);

    CHECK(boost::num_vertices(new_graph) == 4);
    CHECK(boost::edge(2, 3, new_graph).second);
  }

  SUBCASE("empty graph") {

    std::unordered_map<std::size_t, std::size_t> mapping;
    gl::Graph new_g = gl::relabel_vertices(g, mapping);

    CHECK(boost::num_vertices(new_g) == 0);
  }

  SUBCASE("non-existing vertex in mapping") {

    boost::add_edge(0, 1, g);

    std::unordered_map<std::size_t, std::size_t> mapping = {
        {0, 2}, {1, 3}, {2, 4}};

    gl::Graph new_g = gl::relabel_vertices(g, mapping);

    CHECK(boost::num_vertices(new_g) == 4);
    CHECK(boost::edge(2, 3, new_g).second);
  }
}
