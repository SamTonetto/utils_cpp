#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "graph/properties.hpp"

/*
 * For some reason, Doctest's CHECK does not like operator== for PropMaps, so
 * instead we move the equality check out of doctest's CHECK macro.
 */

TEST_CASE("create vertex property") {

  utils::gl::Properties p;

  SUBCASE("scalar mapped value") {
    utils::gl::VertexMap<double> d = {{1, 2}, {2, 3}};
    utils::gl::VertexMap<int> i = {{1, 2}, {2, 3}};

    p.vertex["d"] = d;
    p.vertex["i"] = i;

    bool check1 = p.vertex["d"] == d;
    bool check2 = p.vertex["i"] != i;
    bool check3 = p.vertex["d"] == p.vertex["i"];

    CHECK(check1);
    CHECK(check2);
    CHECK(check3);
  }

  SUBCASE("vector mapped value") {
    utils::gl::VertexMap<std::vector<double>> d = {{1, {2, 2}}, {2, {3, 3}}};
    utils::gl::VertexMap<std::vector<int>> i = {{1, {2, 2}}, {2, {3, 3}}};

    p.vertex["d"] = d;
    p.vertex["i"] = i;

    bool check1 = p.vertex["d"] == d;
    bool check2 = p.vertex["i"] != i;
    bool check3 = p.vertex["d"] == p.vertex["i"];

    CHECK(check1);
    CHECK(check2);
    CHECK(check3);
  }

  SUBCASE("string") {

    utils::gl::VertexMap<std::string> smap = {{1, "2"}, {2, "3"}};

    p.vertex["spos"] = smap;
    p.vertex["spos"][3] = "4";

    bool check1 = p.vertex["spos"] != smap;
    bool check2 = p.vertex["spos"] == utils::gl::VertexMap<std::string>{
                                          {1, "2"}, {2, "3"}, {3, "4"}};

    CHECK(check1);
    CHECK(check2);
  }
}

TEST_CASE("initialize with int map") {

  utils::gl::Graph g;
  utils::gl::Properties p(g);

  utils::gl::VertexMap<int> map = {{1, 2}, {2, 3}};

  p.vertex["position"] = map;

  utils::gl::VertexMap<double> position = p.vertex["position"];

  bool check1 = p.vertex["position"] != map;
  bool check2 =
      p.vertex["position"] == utils::gl::VertexMap<double>({{1, 2}, {2, 3}});

  CHECK(check1);
  CHECK(check2);
}

TEST_CASE("test to json") {

  utils::gl::GraphPropMap gpm;
  gpm["name"] = "chimera";
  gpm["size"] = 8;
  gpm["vertices"] = std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7};

  std::stringstream ss;

  nlohmann::json json = gpm.to_json();

  ss << json;

  CHECK(ss.str() == "{\"name\":\"chimera\",\"size\":8.0,\"vertices\":[0.0,1.0,"
                    "2.0,3.0,4.0,5.0,6.0,7.0]}");
}
