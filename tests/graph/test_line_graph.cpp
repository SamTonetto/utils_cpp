#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/graph/graphviz.hpp"
#include "utils_cpp/graph/library.hpp"
#include "utils_cpp/graph/line_graph.hpp"

using namespace utils;

bool verify_line_graph(
    const gl::Graph &lg, const gl::Graph &g,
    const std::unordered_map<gl::Vertex<>, gl::Edge<>> &v_to_e_map) {

  for (const auto &[v, e] : v_to_e_map) {

    if (!boost::edge(e.m_source, e.m_target, g).second) {

      std::cout << "Edge " << std::endl;
      std::cout << e.m_source << " " << e.m_target << std::endl;
      std::cout << "is not in the original graph" << std::endl;

      return false;
    }

    for (std::size_t nb :
         boost::make_iterator_range(boost::adjacent_vertices(v, lg))) {

      gl::Edge<> v_edge_in_g = v_to_e_map.at(v);
      gl::Edge<> nb_edge_in_g = v_to_e_map.at(nb);

      gl::Vertex<> vs = v_edge_in_g.m_source;
      gl::Vertex<> vt = v_edge_in_g.m_target;

      gl::Vertex<> nbs = nb_edge_in_g.m_source;
      gl::Vertex<> nbt = nb_edge_in_g.m_target;

      if (!(vs == nbs || vs == nbt || vt == nbs || vt == nbt)) {

        std::cout << "Edges " << std::endl;
        std::cout << v_edge_in_g << std::endl;
        std::cout << nb_edge_in_g << std::endl;
        std::cout << "are not adjacent in the original graph" << std::endl;

        return false;
      }
    }
  }
  return true;
}

TEST_CASE("test line graph") {

  SUBCASE("2x2 grid") {

    auto g = gl::grid(2, 2).graph;
    auto [lg, v_to_e_map] = gl::line_graph(g);

    CHECK(boost::num_vertices(lg) == 4);
    CHECK(boost::num_edges(lg) == 4);
    CHECK(verify_line_graph(lg, g, v_to_e_map));
  }

  SUBCASE("3x3 grid") {

    auto g = gl::grid(3, 3).graph;
    auto [lg, v_to_e_map] = gl::line_graph(g);

    CHECK(boost::num_vertices(lg) == 12);
    CHECK(boost::num_edges(lg) == 22);
    CHECK(verify_line_graph(lg, g, v_to_e_map));
  }

  SUBCASE("2x2 kagome") {

    auto g = gl::kagome(2, 2).graph;
    auto [lg, v_to_e_map] = gl::line_graph(g);

    CHECK(boost::num_vertices(lg) == 32);
    CHECK(boost::num_edges(lg) == 70);
    CHECK(verify_line_graph(lg, g, v_to_e_map));
  }
}
