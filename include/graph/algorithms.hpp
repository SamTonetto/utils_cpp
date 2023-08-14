/**********************************************************************
 * @brief Various graph algorithms
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "graph/bitadjmat.hpp"
#include "graph/graph.hpp"

namespace utils {
namespace gl {

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
