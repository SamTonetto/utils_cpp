/**********************************************************************
 * @brief Various graph algorithms
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "utils_cpp/graph/bitadjmat.hpp"
#include "utils_cpp/graph/graph.hpp"

namespace utils {
namespace gl {

/**
 * @brief Graph coloring strategies that can be used in the `graph_coloring`
 * function.
 */
enum class graph_coloring_strategy {
  LARGEST_FIRST,
  SMALLEST_LAST,
};

/**
 * @brief Computes a graph coloring, returning a map from vertices to their
 * color index.
 */

template <typename GraphType>
VertexMap<std::size_t, GraphType>
graph_coloring(const GraphType &g, graph_coloring_strategy strategy) {

  VertexMap<std::size_t, GraphType> color_map;
  color_map.reserve(boost::num_vertices(g));

  using vertex_t = Vertex<GraphType>;

  if (strategy == graph_coloring_strategy::LARGEST_FIRST) {

    // Get vector of (vertex, degree) pairs for all vertices in graph.
    std::vector<std::pair<std::size_t, vertex_t>> vertex_degree_pairs;
    vertex_degree_pairs.reserve(boost::num_vertices(g));
    for (auto v : boost::make_iterator_range(boost::vertices(g))) {

      vertex_degree_pairs.push_back({boost::degree(v, g), v});
    }

    // Sort vertices in descending order of degree
    std::sort(vertex_degree_pairs.begin(), vertex_degree_pairs.end(),
              std::greater<std::pair<std::size_t, vertex_t>>());

    for (auto [deg, v] : vertex_degree_pairs) {

      // Get the colors of the neighbors of v
      std::unordered_set<std::size_t> neighbor_colors;

      for (auto nb :
           boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
        if (color_map.contains(nb)) {
          neighbor_colors.insert(color_map[nb]);
        }
      }

      // Find the smallest color not used by the neighbors
      std::size_t smallest_color = 0;
      while (neighbor_colors.contains(smallest_color)) {
        ++smallest_color;
      }

      // Assign the smallest color to v
      color_map[v] = smallest_color;
    }

    return color_map;

  } else if (strategy == graph_coloring_strategy::SMALLEST_LAST) {

    // not implemented
    throw std::runtime_error("Smallest last graph coloring not implemented");

  } else {

    throw std::runtime_error("Invalid graph coloring strategy");
  }
}

template <typename DistanceType>
using distances_t = std::vector<std::vector<DistanceType>>;

/**
 * @brief Floyd Warshall algorithm for undirected boost graph. Call using
 * floyd_warshall<DistanceType>(g), or floyd_warshall(g) (default DistanceType
 * is int)
 */
template <typename DistanceType = int>
distances_t<DistanceType> floyd_warshall(const Graph &g) {

  std::size_t n = boost::num_vertices(g);
  DistanceType inf = std::numeric_limits<DistanceType>::max();
  distances_t<DistanceType> d(n, std::vector<DistanceType>(n, inf));
  for (std::size_t i = 0; i < n; ++i) {
    d[i][i] = 0;
  }
  for (auto e : boost::make_iterator_range(boost::edges(g))) {
    std::size_t u = boost::source(e, g);
    std::size_t v = boost::target(e, g);
    d[u][v] = 1;
    d[v][u] = 1;
  }

  // Start the floyd warshall iterations
  for (std::size_t k = 0; k < n; ++k) {
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        if (d[i][k] != inf && d[k][j] != inf && d[i][j] > d[i][k] + d[k][j]) {
          d[i][j] = d[i][k] + d[k][j];
        }
      }
    }
  }

  return d;
}

/**
 * @brief Floyd Warshall algorithm for directed boost graph. Call using
 * floyd_warshall<DistanceType>(g), or floyd_warshall(g) (default DistanceType
 * is int)
 */
template <typename DistanceType = int>
distances_t<DistanceType> floyd_warshall(const DiGraph &g) {
  // Set up initial state
  std::size_t n = boost::num_vertices(g);
  DistanceType inf = std::numeric_limits<DistanceType>::max();
  distances_t<DistanceType> d(n, std::vector<DistanceType>(n, inf));
  for (std::size_t i = 0; i < n; ++i) {
    d[i][i] = 0;
  }
  for (auto e : boost::make_iterator_range(boost::edges(g))) {
    std::size_t u = boost::source(e, g);
    std::size_t v = boost::target(e, g);
    d[u][v] = 1;
  }

  // Start the floyd warshall iterations
  for (std::size_t k = 0; k < n; ++k) {
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        if (d[i][k] != inf && d[k][j] != inf && d[i][j] > d[i][k] + d[k][j]) {
          d[i][j] = d[i][k] + d[k][j];
        }
      }
    }
  }

  return d;
}

/**
 * @brief Floyd Warshall algorithm for bit-adjacency matrices. Call using
 * floyd_warshall<DistanceType>(g), where DistanceType can be any integer type.
 */
template <typename DistanceType = int>
distances_t<DistanceType> floyd_warshall(const BitAdjmat &g) {

  std::size_t n = g.num_vertices();
  DistanceType inf = std::numeric_limits<DistanceType>::max();
  distances_t<DistanceType> d(n, std::vector<DistanceType>(n, inf));
  for (std::size_t i = 0; i < n; ++i) {
    d[i][i] = 0;
  }

  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j : g[i]) {
      d[i][j] = 1;
    }
  }

  // Start the floyd warshall iterations
  for (std::size_t k = 0; k < n; ++k) {
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        if (d[i][k] != inf && d[k][j] != inf && d[i][j] > d[i][k] + d[k][j]) {
          d[i][j] = d[i][k] + d[k][j];
        }
      }
    }
  }

  return d;
}

} // namespace gl
} // namespace utils
