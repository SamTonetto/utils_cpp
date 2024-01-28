#include "utils_cpp/graph/bitadjmat.hpp"
#include "utils_cpp/graph/library.hpp"

#include <iostream>

using namespace utils;

int main() {

  auto gb = gl::grid(6, 6);
  auto g = gb.graph;

  // Create bitmatrix adjmat
  gl::BitAdjmat mat = gl::BitAdjmat(g);

  // Print the adjmat
  std::cout << mat << std::endl;

  // Can index with operator[] to get a Row.
  // In practice just use 'auto' to avoid writing whole type.
  gl::BitAdjmat::Row row = mat[0];

  // You can check if a row contains an adjacency
  std::cout << "row.contains(0) = " << row.contains(0)
            << std::endl; // should be false - diagonal of adjmat is zero
  std::cout << "row.contains(1) = " << row.contains(1)
            << std::endl; // should be true
  std::cout << "row.contains(2) = " << row.contains(2)
            << std::endl; // should be false

  // This will iterate through a row, only outputting the entries which are 1.
  for (auto x : row) {
    std::cout << x << ' ';
  }
  std::cout << '\n';

  // You can also set and get entries directly:
  mat.get(0, 1); // true
  mat.get(0, 2); // false

  // You can set entries to true or false (or just 1 or 0).
  // Note that setting (i,j) to a value will automatically set (j,i) to the same
  // value.
  mat.set(0, 3, true);
  mat.set(0, 2, false);

  // Swapping vertices of a graph essentially just swaps the "ordering" of the
  // vertices in the adjacency matrix.
  mat.swap(0, 1);

  // Toggle the entire adjacency matrix, flipping ones and zeros.
  mat.toggle();

  // Perform bitwise in-place against another adjacency matrix. Assumes they
  // have the same dimensions.
  auto mat2 = gl::BitAdjmat(g);
  mat &= mat2;
  mat |= mat2;
  mat ^= mat2;

  // Transform to a graph representation
  gl::Graph g2 = mat.to_graph();

  // Count number of 1s in adjmat
  std::size_t num_ones = mat.count_ones();
  std::cout << "num_ones = " << num_ones << std::endl;

  // Count number of edges in adjmat, or the degree of a vertex.
  std::size_t num_edges = mat.num_edges();
  std::size_t num_edges_incident_to_zero = mat.degree(0);

  std::cout << "num_edges = " << num_edges << std::endl;
  std::cout << "num_edges_incident_to_zero = " << num_edges_incident_to_zero
            << std::endl;

  // Number of vertices in the graph described by the adjmat
  std::size_t num_vertices = mat.num_vertices();
  std::cout << "num_vertices = " << num_vertices << std::endl;
}
