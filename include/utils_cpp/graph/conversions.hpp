/** @file
 * @brief Defines conversions from graph to adjmat.
 */

#pragma once

#include "utils_cpp/graph/graph.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/type_traits/is_same.hpp>

#include <cstdint>
#include <iostream>

namespace utils {
namespace gl {

/**
 *   @brief Construct a boost adjacency matrix from a directed graph
 *   @param graph The graph to construct the adjacency matrix from
 *   @return The adjacency matrix
 */
template <typename GraphType>
auto construct_boost_adjacency_matrix(const GraphType &graph)
    -> std::enable_if_t<
        std::is_same<typename boost::graph_traits<GraphType>::directed_category,
                     boost::directed_tag>::value,
        boost::adjacency_matrix<boost::directedS>> {

  std::vector<std::pair<int, int>> edge_pairs(boost::num_vertices(graph));

  auto [ei, end] = boost::edges(graph);
  for (auto it = ei; it != end; ++it) {
    edge_pairs.push_back(
        std::make_pair(boost::source(*it, graph), boost::target(*it, graph)));
  }

  return boost::adjacency_matrix<boost::directedS>(
      edge_pairs.begin(), edge_pairs.end(), boost::num_vertices(graph));
}

/**
 * @brief Construct a boost adjacency matrix from an undirected graph
 * @param graph The graph to construct the adjacency matrix from
 * @return The adjacency matrix
 */
template <typename GraphType>
auto construct_boost_adjacency_matrix(const GraphType &graph)
    -> std::enable_if_t<
        std::is_same<typename boost::graph_traits<GraphType>::directed_category,
                     boost::undirected_tag>::value,
        boost::adjacency_matrix<boost::undirectedS>> {

  std::vector<std::pair<int, int>> edge_pairs(boost::num_vertices(graph));

  auto [ei, end] = boost::edges(graph);
  for (auto it = ei; it != end; ++it) {
    edge_pairs.push_back(
        std::make_pair(boost::source(*it, graph), boost::target(*it, graph)));
  }

  return boost::adjacency_matrix<boost::undirectedS>(
      edge_pairs.begin(), edge_pairs.end(), boost::num_vertices(graph));
}

/**
 * @brief Construct a vector<vector<int>> adjacency matrix from an undirected
 * graph
 * @param graph The graph to construct the adjacency matrix from
 * @return The adjacency matrix
 */

template <typename GraphType>
auto construct_adjacency_matrix(const GraphType &graph) -> std::enable_if_t<
    std::is_same<typename boost::graph_traits<GraphType>::directed_category,
                 boost::undirected_tag>::value,
    std::vector<std::vector<int>>> {

  std::vector<std::vector<int>> adjmat(
      boost::num_vertices(graph),
      std::vector<int>(boost::num_vertices(graph), 0));

  auto [ei, end] = boost::edges(graph);
  for (auto it = ei; it != end; ++it) {
    adjmat[boost::source(*it, graph)][boost::target(*it, graph)] = 1;
    adjmat[boost::target(*it, graph)][boost::source(*it, graph)] = 1;
  }

  return adjmat;
}

/**
 * @brief Construct a vector<vector<int>> adjacency matrix from a directed graph
 * @param graph The graph to construct the adjacency matrix from
 * @return The adjacency matrix
 */
template <typename GraphType>
auto construct_adjacency_matrix(const GraphType &graph) -> std::enable_if_t<
    std::is_same<typename boost::graph_traits<GraphType>::directed_category,
                 boost::directed_tag>::value,
    std::vector<std::vector<int>>> {

  std::vector<std::vector<int>> adjmat(
      boost::num_vertices(graph),
      std::vector<int>(boost::num_vertices(graph), 0));

  auto [ei, end] = boost::edges(graph);
  for (auto it = ei; it != end; ++it) {
    adjmat[boost::source(*it, graph)][boost::target(*it, graph)] = 1;
  }

  return adjmat;
}

} // namespace gl

} // namespace utils
