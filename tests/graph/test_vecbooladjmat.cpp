#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/graph/library.hpp"
#include "utils_cpp/graph/vecbooladjmat.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace utils;

// I noticed earlier that the adjmat would only be wrong *sometimes*
// Therefore, run the test a bunch of times to ensure that
// The answer is stable and consistent...
//
// Btw I think the issue is fixed now... but I don't know what I did to fix it.
TEST_CASE("initialization") {

  std::string prev;
  bool consistent = true;
  for (int i = 0; i < 1000; ++i) {
    auto gb = gl::grid(6, 6);
    auto &g = gb.graph;

    gl::VecBoolAdjmat bit_adjmat(g);

    std::stringstream ss;
    ss << bit_adjmat;

    if (i > 0) {
      if (ss.str() != prev) {
        consistent = false;
      }
    }
    prev = ss.str();
  }

  CHECK(consistent);
}

TEST_CASE("swap") {

  auto gb = gl::complete(36);
  auto &g = gb.graph;

  gl::VecBoolAdjmat bit_adjmat(g);

  auto bit_adjmat_copy = bit_adjmat;

  bit_adjmat_copy.swap(32, 33);
  bit_adjmat_copy.swap(32, 33);

  bool equal = true;
  for (int i = 0; i < 36; ++i) {
    for (int j = 0; j < 36; ++j) {
      if (bit_adjmat_copy.get(i, j) != bit_adjmat.get(i, j)) {
        equal = false;
        break;
      }
    }
  }

  CHECK(equal);
}

TEST_CASE("adjacency iterator") {

  auto gb = gl::grid(3, 3);
  auto &g = gb.graph;

  gl::VecBoolAdjmat bit_adjmat(g);

  auto row = bit_adjmat[0];

  CHECK(row.contains(0) == false);
  CHECK(row.contains(1) == true);
  CHECK(row.contains(2) == false);
  CHECK(row.contains(3) == true);
  CHECK(row.contains(4) == false);
  CHECK(row.contains(5) == false);
  CHECK(row.contains(6) == false);
  CHECK(row.contains(7) == false);
  CHECK(row.contains(8) == false);

  std::stringstream ss;
  for (auto x : row) {
    ss << x;
  }
  CHECK(ss.str() == "13");
}

TEST_CASE("test bug") {

  // Found a bug, fix it. Previously, adjmat iterator was not fetching links to
  // 0-indexed vertex properly.

  gl::Graph g;

  boost::add_vertex(g);
  boost::add_vertex(g);

  boost::add_edge(0, 1, g);

  gl::VecBoolAdjmat bit_adjmat(g);

  std::stringstream ss;

  for (auto x : bit_adjmat[1]) {
    ss << x;
  }

  CHECK(ss.str() == "0");
}

TEST_CASE("test bug2") {

  gl::Graph g;

  for (int i = 0; i < 6; ++i) {
    boost::add_vertex(g);
  }
  boost::add_edge(0, 4, g);
  boost::add_edge(0, 5, g);

  gl::VecBoolAdjmat bit_adjmat(g);

  CHECK(bit_adjmat.get(0, 1) == 0);

  std::stringstream ss;

  for (auto x : bit_adjmat[0]) {
    ss << x;
  }

  // The bug was it would evaluate to "145" instead.
  CHECK(ss.str() == "45");

  // Actually, exact same fix as previous bug, just in constructor this time...
}

TEST_CASE("test large matrix") {

  // Want to test a matrix with more than 64 nodes, so that more than one uint64
  // is required per row.

  auto gb = gl::grid(8, 9);
  auto &g = gb.graph;

  gl::VecBoolAdjmat bit_adjmat(g);

  std::stringstream ss;

  CHECK(bit_adjmat.size() == 72);
  CHECK(bit_adjmat[0].size() == 72);

  for (std::size_t col = 0; col < bit_adjmat.size(); ++col) {
    ss << bit_adjmat[0].contains(col) << ' ';
  }

  CHECK(ss.str() == "0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
                    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
                    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 ");

  std::stringstream ss2;
  for (auto x : bit_adjmat[0]) {
    ss2 << x << ' ';
  }

  CHECK(ss2.str() == "1 9 ");

  bit_adjmat.swap(0, 1);

  std::stringstream ss3;
  for (std::size_t col = 0; col < bit_adjmat.size(); ++col) {
    ss3 << bit_adjmat[0].contains(col) << ' ';
  }
  CHECK(ss3.str() == "0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
                     "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
                     "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ");

  std::stringstream ss4;
  for (auto x : bit_adjmat[0]) {
    ss4 << x << ' ';
  }

  CHECK(ss4.str() == "1 2 10 ");
}

TEST_CASE("test setting") {

  gl::VecBoolAdjmat adjmat(3);

  adjmat.set(0, 1, true);
  adjmat.set(2, 0, true);

  CHECK(adjmat.get(0, 1) == true);
  CHECK(adjmat.get(1, 0) == false);
  CHECK(adjmat.get(2, 0) == true);
  CHECK(adjmat.get(0, 2) == false);

  std::stringstream ss;
  for (auto x : adjmat[0]) {
    ss << x;
  }
  CHECK(ss.str() == "1");

  ss.str("");

  for (auto x : adjmat[1]) {
    ss << x;
  }
  CHECK(ss.str() == "");

  ss.str("");

  for (auto x : adjmat[2]) {
    ss << x;
  }
  CHECK(ss.str() == "0");
}
