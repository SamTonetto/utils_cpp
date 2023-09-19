#pragma once

#include "graph/detail/generic_prop.hpp"
#include "graph/detail/generic_prop_map.hpp"
#include "graph/detail/generic_prop_value_proxy.hpp"

#include "graph/detail/graph_prop.hpp"
#include "graph/graph.hpp"
#include "metaprogramming/is_instantiation.hpp"

#include <iostream>
#include <memory>
#include <typeinfo>

#include "print.hpp"

namespace utils {
namespace gl {

template <typename GraphType = Graph>
using VertexProp = GenericProp<VertexMap, GraphType>;

template <typename GraphType = Graph>
using EdgeProp = GenericProp<EdgeMap, GraphType>;

// ===============================
// Property Maps
// ===============================

using GraphPropMap = GenericPropMap<GraphProp>;

template <typename GraphType = Graph>
using VertexPropMap = GenericPropMap<VertexProp<GraphType>>;

template <typename GraphType = Graph>
using EdgePropMap = GenericPropMap<EdgeProp<GraphType>>;

// ====================
// Properties
// ====================

template <typename GraphType = Graph>
struct Properties {
public:
  Properties() = default;

  Properties(const GraphType &g) {
    graph["num_vertices"] = boost::num_vertices(g);
    graph["num_edges"] = boost::num_edges(g);
  }

  GraphPropMap graph;
  VertexPropMap<GraphType> vertex;
  EdgePropMap<GraphType> edge;
};

namespace graph {

/**
 * GraphBundle is a convenience struct that bundles together the graph with
 * properties.
 */
template <typename GraphType>
class GraphBundle {
public:
  GraphType graph;
  Properties<GraphType> props;

  GraphBundle() : graph{}, props{} {}

  GraphBundle(const GraphType &g) : graph{g}, props{g} {}

  GraphBundle(const GraphType &g, const Properties<GraphType> &p)
      : graph{g}, props{p} {
    props.graph["num_vertices"] = boost::num_vertices(g);
    props.graph["num_edges"] = boost::num_edges(g);
  }

  auto &operator[](const std::string &key) { return props.graph[key]; }
  auto &operator[](const std::string &key) const { return props.graph[key]; }

  auto begin() { return props.graph.begin(); }
  auto end() { return props.graph.end(); }
  auto begin() const { return props.graph.begin(); }
  auto end() const { return props.graph.end(); }
};
} // namespace graph

using GraphBundle = graph::GraphBundle<Graph>;

} // namespace gl

} // namespace utils

template <typename GraphType>
std::ostream &operator<<(std::ostream &os,
                         utils::gl::graph::GraphBundle<GraphType> &gb) {
  os << " graph {\n";
  for (auto &[key, value] : gb.props.graph) {
    os << "  " << key << ": " << value << ",\n";
  }

  os << "  vertices {\n";
  for (auto v : boost::make_iterator_range(boost::vertices(gb.graph))) {
    os << "   " << v << ": {";
    std::size_t i = 0;
    for (auto &[key, value] : gb.vertex) {
      os << key << ": " << value[v];
      if (i < gb.vertex.size() - 1) {
        os << ", ";
      }
      ++i;
    }
    os << "},\n";
  }

  os << "  edges {\n";
  for (auto e : boost::make_iterator_range(boost::edges(gb.graph))) {
    os << "   " << e << ": {";
    std::size_t i = 0;
    for (auto &[key, value] : gb.edge) {
      os << key << ": " << value;
      if (i < gb.edge.size() - 1) {
        os << ", ";
      }
      ++i;
    }
    os << "},\n";
  }

  os << " }\n";

  return os;
}
