#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/disjointset.hpp"

using namespace utils;

TEST_CASE("Constructor & Initial State") {
  DisjointSet ds(5);
  for (int i = 0; i < 5; ++i) {
    CHECK(ds.find(i) == i);
  }
}

TEST_CASE("Unifying Nodes") {
  DisjointSet ds(5);
  ds.unify(0, 1);
  CHECK(ds.find(0) == ds.find(1));
}

TEST_CASE("Finding Representatives") {
  DisjointSet ds(5);
  ds.unify(0, 1);
  ds.unify(2, 3);
  CHECK(ds.find(0) == ds.find(1));
  CHECK(ds.find(2) == ds.find(3));
  CHECK(ds.find(0) != ds.find(2));
}

TEST_CASE("Connected Components After Some Unifications") {
  DisjointSet ds(5);
  ds.unify(0, 1);
  ds.unify(2, 3);
  auto components = ds.get_connected_components();
  CHECK(components.size() == 3);

  std::unordered_map<int, int> component_count;
  for (const auto &component : components) {
    for (int node : component) {
      component_count[node]++;
    }
  }

  for (int i = 0; i < 5; ++i) {
    CHECK(component_count[i] == 1);
  }
}

TEST_CASE("Further Unifications") {
  DisjointSet ds(5);
  ds.unify(0, 1);
  ds.unify(2, 3);
  ds.unify(3, 4);
  CHECK(ds.find(3) == ds.find(4));
  CHECK(ds.find(2) == ds.find(4));

  auto components = ds.get_connected_components();
  CHECK(components.size() == 2);

  ds.unify(0, 1);
  CHECK(ds.find(0) == ds.find(1));

  components = ds.get_connected_components();
  CHECK(components.size() == 2);
}
