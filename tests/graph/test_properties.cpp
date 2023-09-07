#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "graph/properties.hpp"

using namespace utils;
using namespace gl;

TEST_CASE("create vertex property") {

  Properties p;

  SUBCASE("scalar mapped value") {
    VertexMap<double> d = {{1, 2}, {2, 3}};
    VertexMap<int> i = {{1, 2}, {2, 3}};

    p.vertex["d"] = d;
    p.vertex["i"] = i;

    CHECK(p.vertex["d"] == d);
    CHECK(p.vertex["i"] != i);
    CHECK(p.vertex["d"] == p.vertex["i"]);
  }

  SUBCASE("vector mapped value") {
    VertexMap<std::vector<double>> d = {{1, {2, 2}}, {2, {3, 3}}};
    VertexMap<std::vector<int>> i = {{1, {2, 2}}, {2, {3, 3}}};

    p.vertex["d"] = d;
    p.vertex["i"] = i;

    CHECK(p.vertex["d"] == d);
    CHECK(p.vertex["i"] != i);
    CHECK(p.vertex["d"] == p.vertex["i"]);
  }

  SUBCASE("string") {

    VertexMap<std::string> smap = {{1, "2"}, {2, "3"}};

    p.vertex["spos"] = smap;
    p.vertex["spos"][3] = "4";

    CHECK(p.vertex["spos"] != smap);
    CHECK(p.vertex["spos"] ==
          VertexMap<std::string>{{1, "2"}, {2, "3"}, {3, "4"}});
  }
}

TEST_CASE("initialize with int map") {

  Graph g;
  Properties p(g);

  VertexMap<int> map = {{1, 2}, {2, 3}};

  p.vertex["position"] = map;

  VertexMap<double> position = p.vertex["position"];

  CHECK(p.vertex["position"] != map);
  CHECK(p.vertex["position"] == VertexMap<double>{{1, 2}, {2, 3}});
}

TEST_CASE("test to json") {

  GraphPropMap gpm;
  gpm["name"] = "chimera";
  gpm["size"] = 8;
  gpm["vertices"] = std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7};

  std::stringstream ss;

  nlohmann::json json = gpm.to_json();

  ss << json;

  CHECK(ss.str() == "{\"name\":\"chimera\",\"size\":8.0,\"vertices\":[0.0,1.0,"
                    "2.0,3.0,4.0,5.0,6.0,7.0]}");
}
