#include <iostream>
#include <random>

#include "utils_cpp/graph/graph.hpp"
#include "utils_cpp/graph/library.hpp"
#include "utils_cpp/graph/pathfinding.hpp"

using namespace utils;

int main() {

  auto gb = gl::ibm_hex(2, 2);
  auto &graph = gb.graph;

  long long seed = 0;
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<int> dist(0, 1);

  // Random weights
  using WeightType = int;
  gl::EdgeMap<WeightType, gl::Graph> weights;
  for (auto e : boost::make_iterator_range(boost::edges(graph))) {
    weights[e] = dist(rng);
  }

  int source = 3; // Vertex<Graph> can usually be represented as int.

  // Get dijkstra distances only - return type std::vector<int>
  {
    auto dijkstra_distances = gl::dijkstra_distances(graph, source, weights);

    std::cout << "Distances:" << std::endl;
    for (std::size_t i = 0; i < dijkstra_distances.size(); ++i) {
      std::cout << "d(" << source << ", " << i
                << ") = " << dijkstra_distances[i] << '\n';
    }
  }

  // Get dijkstra predecessors only -  return type
  // std::vector<gl::Vertex<gl::Graph>>
  {
    auto dijkstra_predecessors =
        gl::dijkstra_predecessors(graph, source, weights);

    std::cout << "Predecessors: " << std::endl;
    for (std::size_t i = 0; i < dijkstra_predecessors.size(); ++i) {
      std::cout << "predecessor of " << i << ": " << dijkstra_predecessors[i]
                << '\n';
    }
  }

  {

    // Get both distances and predecessors:
    auto [dijkstra_distances, dijkstra_predecessors] =
        gl::dijkstra(graph, source, weights);

    std::cout << "Distances:" << std::endl;
    for (std::size_t i = 0; i < dijkstra_distances.size(); ++i) {
      std::cout << "d(" << source << ", " << i
                << ") = " << dijkstra_distances[i] << '\n';
    }

    std::cout << "Predecessors: " << std::endl;
    for (std::size_t i = 0; i < dijkstra_predecessors.size(); ++i) {
      std::cout << "predecessor of " << i << ": " << dijkstra_predecessors[i]
                << '\n';
    }
  }
}
