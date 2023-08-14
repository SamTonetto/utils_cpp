#include <iostream>

#include "graph/graph.hpp"
#include "graph/library.hpp"
#include "graph/pathfinding.hpp"

using namespace utils;

int main() {

  auto gb = gl::ibm_hex(2, 2);
  auto graph = gb.graph;

  std::cout << "boost::num_vertices(graph) = " << boost::num_vertices(graph)
            << std::endl;

  int source = 3; // Vertex<Graph> can usually be represented as int.

  // Get bfs distances only - return type std::vector<int>
  {
    auto bfs_distances = gl::bfs_distances(graph, source);

    std::cout << "Distances:" << std::endl;
    for (std::size_t i = 0; i < bfs_distances.size(); ++i) {
      std::cout << "d(" << source << ", " << i << ") = " << bfs_distances[i]
                << '\n';
    }
  }

  // Get bfs predecessors only -  return type std::vector<gl::Vertex<gl::Graph>>
  {
    auto bfs_predecessors = gl::bfs_predecessors(graph, source);

    std::cout << "Predecessors: " << std::endl;
    for (std::size_t i = 0; i < bfs_predecessors.size(); ++i) {
      std::cout << "predecessor of " << i << ": " << bfs_predecessors[i]
                << '\n';
    }
  }

  {

    // Get both distances and predecessors:
    auto [bfs_distances, bfs_predecessors] = gl::bfs(graph, source);

    std::cout << "Distances:" << std::endl;
    for (std::size_t i = 0; i < bfs_distances.size(); ++i) {
      std::cout << "d(" << source << ", " << i << ") = " << bfs_distances[i]
                << '\n';
    }

    std::cout << "Predecessors: " << std::endl;
    for (std::size_t i = 0; i < bfs_predecessors.size(); ++i) {
      std::cout << "predecessor of " << i << ": " << bfs_predecessors[i]
                << '\n';
    }
  }
}
