#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "graph/library.hpp"

using namespace utils;

TEST_CASE("grid graph") {

  auto gb = gl::grid(3, 3);
  auto &g = gb.graph;

  CHECK(boost::num_vertices(g) == 9);
  CHECK(boost::num_edges(g) == 12);

  std::string name = gb["name"];
  std::size_t height = gb["height"];

  CHECK(height == 3);

  CHECK(gb["width"].get<std::size_t>() == 3);
}

TEST_CASE("complete graph") {

  auto gb = gl::complete(100);
  auto &g = gb.graph;

  CHECK(boost::num_vertices(g) == 100);
  CHECK(boost::num_edges(g) == 4950);
}

TEST_CASE("random graph") {

  auto gb = gl::random(100, 0.1);
  auto &g = gb.graph;

  std::cout << "boost::num_edges(g) = " << boost::num_edges(g) << std::endl;
}
