#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "graph/properties.hpp"

using namespace utils;

TEST_CASE("create graph property") {

  gl::Graph g;
  gl::Properties p(g);

  gl::VertexMap<std::array<double, 2>> map = {{1, {2, 2}}, {2, {3, 3}}};

  p.vertex["position"] = map;

  p.graph["name"] = "test";
  p.graph["height"] = 3;

  auto position = p.vertex["position"].get<std::array<double, 2>>();

  CHECK(position == map);

  auto name = p.graph["name"].get<std::string>();
  auto height = p.graph["height"].get<int>();

  CHECK(height == 3);
  CHECK(name == "test");
}
