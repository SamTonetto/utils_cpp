#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <iostream>
#include <sstream>
#include <string>

#include "matrix.hpp"

using namespace utils;

TEST_CASE("initialize matrix") {

  std::size_t size = 4;

  Matrix<std::size_t> matrix(size, size, 0);

  for (std::size_t i = 0; i < size; ++i) {
    matrix(i, (i + 1) % size) = 1;
    matrix((i + 1) % size, i) = 1;
  }
  std::stringstream ss;
  ss << matrix;

  std::string correct = "0 1 0 1 \n1 0 1 0 \n0 1 0 1 \n1 0 1 0 \n";

  CHECK(ss.str() == correct);
}

TEST_CASE("row access") {

  std::size_t nrows = 3, ncols = 2;

  Matrix matrix(nrows, ncols, 0);

  for (std::size_t i = 0; i < nrows; ++i) {
    for (std::size_t j = 0; j < ncols; ++j) {
      matrix(i, j) = i * ncols + j;
    }
  }

  CHECK(matrix[0][0] == 0);
  CHECK(matrix[0][1] == 1);
  CHECK(matrix[1][0] == 2);
  CHECK(matrix[1][1] == 3);
  CHECK(matrix[2][0] == 4);
  CHECK(matrix[2][1] == 5);
}

TEST_CASE("Test Matrix Construction and Resizing") {
  Matrix<int> m(2, 2, 1);

  CHECK(m.size() == 2);
  CHECK(m.shape().first == 2);
  CHECK(m.shape().second == 2);

  SUBCASE("Test Resizing") {
    m.resize(3, 3, 0);
    CHECK(m.size() == 3);
    CHECK(m.shape().first == 3);
    CHECK(m.shape().second == 3);
  }
}

TEST_CASE("Test Matrix Access") {
  Matrix<int> m(2, 2, 1);
  m(1, 1) = 2;

  CHECK(m(1, 1) == 2);
  CHECK(m[1][1] == 2);
}

TEST_CASE("Test Matrix Iteration") {
  Matrix<int> m(2, 2, 1);
  int total = 0;

  for (std::size_t i = 0; i < m.size(); i++) {
    auto row = m[i];
    for (int val : row) {
      total += val;
    }
  }

  CHECK(total == 4);
}
