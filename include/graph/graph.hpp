/** @file
 * @brief Defines the graph types and related types.
 *
 * This file defines the graph types and related types. It also provides
 * specializations of the std::hash struct for the vertex and edge descriptors
 * of the graph types.
 *
 */

#pragma once

#include "hash.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <iostream>

namespace utils {
namespace gl {

// ---- Graph Types ----

/**
 * We nest the graph types in a namespace so that later we can use the simple
 * type aliases:
 * ```
 * using Graph = graph::Graph<>;
 * using DiGraph = graph::DiGraph<>;
 * using BiGraph = graph::BiGraph<>;
 *```
 */
namespace graph {

/**
 * @brief Basic alias for undirected graphs

 */
template <typename VertexProperty = boost::no_property,
          typename EdgeProperty = boost::no_property,
          typename GraphProperty = boost::no_property>
using Graph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                          VertexProperty, EdgeProperty, GraphProperty>;

/**
 * @brief Basic alias for directed graphs
 */
template <typename VertexProperty = boost::no_property,
          typename EdgeProperty = boost::no_property,
          typename GraphProperty = boost::no_property>
using DiGraph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                          VertexProperty, EdgeProperty, GraphProperty>;

/**
 * @brief Basic alias for bidirectional graphs
 */
template <typename VertexProperty = boost::no_property,
          typename EdgeProperty = boost::no_property,
          typename GraphProperty = boost::no_property>
using BiGraph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                          VertexProperty, EdgeProperty, GraphProperty>;
} // namespace graph

/**
 * @brief Simple aliases for graphs without interior properties.
 */
using Graph = gl::graph::Graph<>;
using DiGraph = gl::graph::DiGraph<>;
using BiGraph = gl::graph::BiGraph<>;

// ---- Vertex and Edge Types ----

/**
 * @brief Type alias for vertex descriptor.
 */
template <typename GraphType = Graph>
using Vertex = typename boost::graph_traits<GraphType>::vertex_descriptor;

/**
 * @brief Type alias for edge descriptor.
 */
template <typename GraphType = Graph>
using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;

// ---- Iterators ----

/**
 * @brief Type aliases for vertex iterator.
 */
template <typename GraphType>
using VertexIterator = typename boost::graph_traits<GraphType>::vertex_iterator;

/**
 * @brief Type aliases for edge iterator.
 */
template <typename GraphType>
using EdgeIterator = typename boost::graph_traits<GraphType>::edge_iterator;

/**
 * @brief Type aliases for out-edge iterator.
 */
template <typename GraphType>
using OutEdgeIterator =
    typename boost::graph_traits<GraphType>::out_edge_iterator;

/**
 * @brief Type aliases for in-edge iterator.
 */
template <typename GraphType>
using InEdgeIterator =
    typename boost::graph_traits<GraphType>::in_edge_iterator;

/**
 * @brief Type aliases for adjacency iterator.
 */
template <typename GraphType>
using AdjacencyIterator =
    typename boost::graph_traits<GraphType>::adjacency_iterator;

// ---- Some helper functions ----

/**
 * Returns the index of a vertex.
 * For most reasonable graph types, Vertex is implicitly convertible to size_t.
 * This function is a reminder of that.
 * @tparam GraphType The graph type.
 * @param v The vertex.
 * @return The index of the vertex.
 */
template <typename GraphType>
std::size_t index(Vertex<GraphType> v) {
  return v;
}

/**
 * Returns the source vertex of an edge.
 * Did you know you can get the source of an edge without passing in the graph
 * as an argument, i.e. boost::source(e, g)? It's nowhere in the boost
 * documentation, so this function is a reminder that you can.
 * @tparam GraphType The graph type.
 * @param e The edge.
 * @return The source vertex of the edge.
 */
template <typename GraphType>
Vertex<GraphType> source(Edge<GraphType> e) {
  return e.m_source;
}

/**
 * Returns the target vertex of an edge.
 * Did you know you can get the target of an edge without passing in the graph
 * as an argument, i.e. boost::target(e, g)? It's nowhere in the boost
 * documentation, so this function is a reminder that you can.
 * @tparam GraphType The graph type.
 * @param e The edge.
 * @return The target vertex of the edge.
 */
template <typename GraphType>
Vertex<GraphType> target(Edge<GraphType> e) {
  return e.m_target;
}

// ---- Hash functions ----

namespace detail {

/**
 * @brief Template struct for determining the directedness of a graph.
 *
 * This struct is specialized for the boost::adjacency_list class.
 * It provides a nested type `type` that represents the directedness of the
 * graph.
 *
 * @tparam GraphType The graph type.
 */
template <typename GraphType>
struct GraphDirectedness;

/**
 * @brief Template struct for determining the directedness of a
 * boost::adjacency_list graph.
 *
 * This struct is specialized for the boost::adjacency_list class with specific
 * template parameters. It extracts the Directed template parameter from the
 * boost::adjacency_list specialization and assigns it to the type using `using`
 * alias declaration.
 *
 * @tparam OutEdgeList The type of the container used to represent out-edges of
 * each vertex in the graph.
 * @tparam VertexList The type of the container used to store the vertices of
 * the graph.
 * @tparam Directed A boolean value indicating whether the graph is directed or
 * not.
 * @tparam VertexProperty The type of the property bundle associated with each
 * vertex.
 * @tparam EdgeProperty The type of the property bundle associated with each
 * edge.
 * @tparam GraphProperty The type of the property bundle associated with the
 * graph.
 */
template <typename OutEdgeList, typename VertexList, typename Directed,
          typename VertexProperty, typename EdgeProperty,
          typename GraphProperty>
struct GraphDirectedness<
    boost::adjacency_list<OutEdgeList, VertexList, Directed, VertexProperty,
                          EdgeProperty, GraphProperty>> {
  using type = Directed;
};

/**
 * @internal
 * @brief Template struct for providing a hash function for edge descriptors of
 * a graph.
 *
 * This struct takes a graph type as a template parameter and an optional
 * Directed template parameter, which defaults to the directedness obtained from
 * GraphDirectedness<GraphType>. It is responsible for providing a hash function
 * for the edge descriptors of the graph.
 *
 * @tparam GraphType The graph type.
 * @tparam Directed The directedness of the graph.
 */
template <typename GraphType,
          typename Directed = typename GraphDirectedness<GraphType>::type>
struct EdgeHash;

/**
 * @internal
 * @brief Specialization of detail::EdgeHash struct for undirected graphs.
 *
 * This specialization provides a hash function for edge descriptors of
 * undirected graphs. It computes the hash by combining the hash values of the
 * source and target vertices using XOR.
 *
 * @tparam GraphType The graph type.
 */
template <typename GraphType>
struct EdgeHash<GraphType, boost::undirectedS> {
  /**
   * @brief Computes the hash value for the given edge descriptor of an
   * undirected graph.
   *
   * @param e The edge descriptor.
   * @return The computed hash value.
   */
  std::size_t operator()(const typename GraphType::edge_descriptor &e) const {
    std::size_t hash = 0;
    symmetric_hash_combine(hash, e.m_source);
    symmetric_hash_combine(hash, e.m_target);
    return hash;
  }
};

/**
 * @internal
 * @brief Specialization of detail::EdgeHash struct for directed graphs.
 *
 * This specialization provides a hash function for edge descriptors of directed
 * graphs. It computes the hash value by combining the hash values of the source
 * and target vertices using boost::hash_combine.
 *
 * @tparam GraphType The graph type.
 */
template <typename GraphType>
struct EdgeHash<GraphType, boost::directedS> {
  /**
   * @brief Computes the hash value for the given edge descriptor of a directed
   * graph.
   *
   * @param e The edge descriptor.
   * @return The computed hash value.
   */
  std::size_t operator()(const typename GraphType::edge_descriptor &e) const {
    std::size_t hash = 0;
    hash_combine(hash, e.m_source);
    hash_combine(hash, e.m_target);
    return hash;
  }
};

/**
 * @internal
 * @brief Template struct for providing an equality function for edge, used in
 * unordered_set and unordered_map.
 */
template <typename GraphType,
          typename Directed = typename GraphDirectedness<GraphType>::type>
struct EdgeKeyEquals;

/**
 * @internal
 * @brief Specialization for undirected edges.
 */
template <typename GraphType>
struct EdgeKeyEquals<GraphType, boost::undirectedS> {
  bool operator()(const typename GraphType::edge_descriptor &lhs,
                  const typename GraphType::edge_descriptor &rhs) const {
    return (lhs.m_source == rhs.m_source && lhs.m_target == rhs.m_target) ||
           (lhs.m_source == rhs.m_target && lhs.m_target == rhs.m_source);
  }
};

/**
 * @internal
 * @brief Specialization for directed edges.
 */
template <typename GraphType>
struct EdgeKeyEquals<GraphType, boost::directedS> {
  bool operator()(const typename GraphType::edge_descriptor &lhs,
                  const typename GraphType::edge_descriptor &rhs) const {
    return lhs.m_source == rhs.m_source && lhs.m_target == rhs.m_target;
  }
};

} // namespace detail

// ---- Property Maps ----

/**
 * @brief Alias for an unordered_map using edge descriptors as keys.
 *
 * This alias represents an unordered_map with edge descriptors of a graph as
 * keys.
 *
 * @tparam ValueType The value type associated with each edge.
 * @tparam GraphType The graph type.
 */
template <typename ValueType, typename GraphType = Graph,
          typename Directed =
              typename detail::GraphDirectedness<GraphType>::type>
using EdgeMap =
    typename std::unordered_map<Edge<GraphType>, ValueType,
                                detail::EdgeHash<GraphType, Directed>,
                                detail::EdgeKeyEquals<GraphType, Directed>>;

/**
 * @brief Alias for an unordered_map using vertex descriptors as keys.
 *
 * This alias represents an unordered_map with vertex descriptors of a graph as
 * keys.
 *
 * @tparam ValueType The value type associated with each vertex.
 * @tparam GraphType The graph type.
 */
template <typename ValueType, typename GraphType = Graph>
using VertexMap = std::unordered_map<Vertex<GraphType>, ValueType>;

// ---- Property Sets ----

/**
 * @brief Alias for an unordered_set containing edge descriptors.
 *
 * @tparam GraphType The graph type.
 */
template <typename GraphType = Graph,
          typename Directed =
              typename detail::GraphDirectedness<GraphType>::type>
using EdgeSet =
    typename std::unordered_set<Edge<GraphType>,
                                detail::EdgeHash<GraphType, Directed>,
                                detail::EdgeKeyEquals<GraphType, Directed>>;

/**
 * @brief Alias for an unordered_set containing vertex descriptors.
 *
 * @tparam GraphType The graph type.
 */
template <typename GraphType = Graph>
using VertexSet = typename std::unordered_set<Vertex<GraphType>>;

// ---- Loading from Edgelist ----

template <typename T>
  requires std::is_integral_v<T>
Graph from_edgelist(const std::vector<std::vector<T>> &edgelist) {
  Graph g;
  for (const auto &edge : edgelist) {
    add_edge(edge[0], edge[1], g);
  }
  return g;
}

template <typename T>
  requires std::is_integral_v<T>
Graph from_edgelist(const std::vector<std::array<T, 2>> &edgelist) {
  Graph g;
  for (const auto &edge : edgelist) {
    add_edge(edge[0], edge[1], g);
  }
  return g;
}

template <typename T>
  requires std::is_integral_v<T>
Graph from_edgelist(const std::vector<std::pair<T, T>> &edgelist) {
  Graph g;
  for (const auto &edge : edgelist) {
    add_edge(edge.first, edge.second, g);
  }
  return g;
}

inline std::vector<std::pair<std::size_t, std::size_t>>
to_edgelist(const Graph &g) {
  std::vector<std::pair<std::size_t, std::size_t>> edgelist;
  for (auto e : boost::make_iterator_range(boost::edges(g))) {
    edgelist.push_back({boost::source(e, g), boost::target(e, g)});
  }
  return edgelist;
}

} // namespace gl

// ---- Output ----

template <typename GraphType, typename ValueType>
std::ostream &operator<<(std::ostream &os,
                         const utils::gl::VertexMap<GraphType, ValueType> &m) {
  for (auto &[vertex, value] : m) {
    os << vertex << ": " << value << '\n';
  }
  return os;
}

template <typename GraphType, typename ValueType>
std::ostream &operator<<(std::ostream &os,
                         const utils::gl::EdgeMap<GraphType, ValueType> &m) {
  for (auto &[edge, value] : m) {
    os << '(' << edge.m_source << ", " << edge.m_target << "): " << value
       << '\n';
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const utils::gl::Graph &g) {
  for (auto v : boost::make_iterator_range(boost::vertices(g))) {
    os << v << ": ";
    for (auto nb : boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
      os << nb << " ";
    }
    os << '\n';
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const utils::gl::BiGraph &g) {
  for (auto v : boost::make_iterator_range(boost::vertices(g))) {
    os << v << ": ";
    for (auto nb : boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
      os << nb << " ";
    }
    os << '\n';
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const utils::gl::DiGraph &g) {
  for (auto v : boost::make_iterator_range(boost::vertices(g))) {
    os << v << ": ";
    for (auto nb : boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
      os << nb << " ";
    }
    os << '\n';
  }
  return os;
}

} // namespace utils
