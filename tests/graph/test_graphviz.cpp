#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <iostream>
#include <sstream>
#include <string>

#include "graph/graphviz.hpp"
#include "graph/library.hpp"

using namespace utils;

TEST_CASE("write basic graph") {

  auto gb = gl::grid(2, 2);
  auto &g = gb.graph;

  std::stringstream ss;

  gl::GraphWriter<gl::Graph> graph_writer(g);
  graph_writer.add_graph_property("splines", "true");
  graph_writer.add_vertex_property("shape", "circle");
  graph_writer.add_edge_property("penwidth", "2");

  graph_writer.vertex_props[0]["shape"] = "square";

  graph_writer.write(ss);

  std::string expected =
      "graph G {\n  splines=true;\n  0 [shape=square];\n  1 [shape=circle];\n  "
      "2 [shape=circle];\n  3 [shape=circle];\n  0 -> 1 [penwidth=2];\n  0 -> "
      "2 [penwidth=2];\n  1 -> 3 [penwidth=2];\n  2 -> 3 [penwidth=2];\n}\n";

  CHECK(ss.str() == expected);
}

TEST_CASE("write basic digraph with positioning") {

  gl::DiGraph g;

  boost::add_edge(0, 1, g);
  boost::add_edge(0, 2, g);
  boost::add_edge(2, 0, g);

  std::stringstream ss;

  gl::GraphWriter<gl::DiGraph> graph_writer(g);
  graph_writer.add_graph_property("splines", "true");
  graph_writer.add_vertex_property("shape", "circle");
  graph_writer.add_edge_property("penwidth", "2");

  graph_writer.vertex_props[0]["shape"] = "square";

  // Create position map
  gl::VertexMap<std::pair<int, int>, gl::DiGraph> pos;
  pos[0] = {0, 0};
  pos[1] = {1, 0};
  pos[2] = {1, 1};

  for (auto [v, p] : pos) {
    graph_writer.vertex_props[v]["pos"] =
        std::to_string(p.first) + "," + std::to_string(p.second) + "!";
  }

  graph_writer.write(ss);

  std::string expected =
      "digraph G {\n  splines=true;\n  0 [pos=0,0!, shape=square];\n  1 "
      "[pos=1,0!, shape=circle];\n  2 [pos=1,1!, shape=circle];\n  0 -> 1 "
      "[penwidth=2];\n  0 -> 2 [penwidth=2];\n  2 -> 0 [penwidth=2];\n}\n";

  CHECK(ss.str() == expected);
}
