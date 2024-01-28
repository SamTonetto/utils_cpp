#include "utils_cpp/graph/bitadjmat.hpp"
#include "utils_cpp/graph/conversions.hpp"
#include "utils_cpp/graph/library.hpp"

#include <iostream>

using namespace utils;

int main() {

  auto gb = gl::grid(6, 6);

  // Conversion to boost adjmat - interface seems more like a normal boost graph
  // interface than a matrix interface.
  auto boost_adjmat = gl::construct_boost_adjacency_matrix(gb.graph);

  std::cout << "Boost adjmat: " << std::endl;
  for (std::size_t i = 0; i < boost::num_vertices(boost_adjmat); ++i) {
    std::cout << "Vertex " << i << " is adjacent to: ";
    for (auto [adj_it, adj_end] = boost::adjacent_vertices(i, boost_adjmat);
         adj_it != adj_end; ++adj_it) {
      std::cout << *adj_it << " ";
    }
    std::cout << std::endl;
  }

  // Conversion to vector<vector<int>> adjmat.
  std::vector<std::vector<int>> adjmat =
      gl::construct_adjacency_matrix(gb.graph);

  std::cout << "Vector adjmat" << std::endl;
  for (auto &row : adjmat) {
    for (auto &col : row) {
      std::cout << col << " ";
    }
    std::cout << std::endl;
  }

  // Conversion to BitAdjmat
  gl::BitAdjmat bit_adjmat(gb.graph);
  std::cout << "Bit matrix adjmat" << std::endl;
  std::cout << bit_adjmat << std::endl;
}
