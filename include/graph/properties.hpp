#pragma once

#include <iostream>
#include <memory>
#include <typeinfo>

#include "graph/graph.hpp"
#include "metaprogramming/is_instantiation.hpp"
#include "print.hpp"

#include <nlohmann/json.hpp>

namespace utils {
namespace gl {

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template <template <typename...> typename MapType, typename GraphType>
struct GenericProp;

template <typename Key, template <typename...> typename MapType,
          typename GraphType>
struct GenericPropValueProxy {

  Key k;
  GenericProp<MapType, GraphType> &gp;

  GenericPropValueProxy(const Key &k, GenericProp<MapType, GraphType> &gp)
      : k{k}, gp{gp} {}

  template <Arithmetic T>
  GenericPropValueProxy &operator=(const T &n) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = static_cast<double>(n);
  }

  template <Arithmetic T>
  GenericPropValueProxy &operator=(const std::vector<T> &v) {
    std::vector<double> tmp;
    tmp.reserve(v.size());
    for (const auto &e : v) {
      tmp.push_back(static_cast<double>(e));
    }
    std::get<MapType<std::vector<double>, GraphType>>(gp.p)[k] = std::move(tmp);
    return *this;
  }

  GenericPropValueProxy &operator=(const std::string &s) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = s;
    return *this;
  }

  GenericPropValueProxy &operator=(const char *s) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = std::string(s);
    return *this;
  }

  GenericPropValueProxy() = default;

  template <typename T>
  operator const T &() const {
    std::get<MapType<T, GraphType>>(gp.p)[k];
  }

  template <typename T>
  operator T &() {
    std::get<MapType<T, GraphType>>(gp.p)[k];
  }

  template <typename T>
  friend bool operator==(const GenericPropValueProxy &lhs, const T &rhs);
};

template <typename Key, template <typename...> typename M, typename G,
          typename T>
bool operator==(const GenericPropValueProxy<Key, M, G> &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T,
                               GenericPropValueProxy<Key, M, G>>) { // RHS is
                                                                    // generic

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];

  } else if constexpr (std::is_same_v<T,
                                      M<std::string, G>>) { // RHS is string map

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<double, G>>) {

    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<std::vector<double>, G>>) {

    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else {
    return false;
  }
}

/**
 * MapType can be VertexMap or EdgeMap.
 */
template <template <typename...> typename MapType, typename GraphType>
struct GenericProp {

  std::variant<MapType<std::string, GraphType>, MapType<double, GraphType>,
               MapType<std::vector<double>, GraphType>>
      p;

  GenericProp(const MapType<std::string, GraphType> &m) : p(m) {}

  template <Arithmetic T>
  GenericProp(const MapType<T, GraphType> &m) {

    if constexpr (std::is_same_v<T, double>) {
      p = m;
    } else {
      p.template emplace<MapType<double, GraphType>>();
      for (const auto &[k, v] : m) {
        std::get<MapType<double, GraphType>>(p)[k] = static_cast<double>(v);
      }
    }
  }

  template <Arithmetic T>
  GenericProp(const MapType<std::vector<T>, GraphType> &m) {

    if constexpr (std::is_same_v<T, double>) {
      p = m;
    } else {

      p.template emplace<MapType<std::vector<double>, GraphType>>();
      for (const auto &[k, v] : m) {
        std::vector<double> tmp;
        tmp.reserve(v.size());
        for (const auto &e : v) {
          tmp.push_back(static_cast<double>(e));
        }
        std::get<MapType<std::vector<double>, GraphType>>(p)[k] = tmp;
      }
    }
  }

  GenericProp() = default;

  template <typename Key>
  GenericPropValueProxy<Key, MapType, GraphType> operator[](Key &&k) {
    return GenericPropValueProxy<Key, MapType, GraphType>{std::forward<Key>(k),
                                                          *this};
  }

  template <typename T>
  operator const MapType<T, GraphType> &() const {
    return std::get<MapType<T, GraphType>>(p);
  }

  template <typename T>
  operator MapType<T, GraphType> &() {
    return std::get<MapType<T, GraphType>>(p);
  }

  template <template <typename...> typename M, typename G, typename T>
  friend bool operator==(const GenericProp<M, G> &lhs, const T &rhs);
};

template <template <typename...> typename M, typename G, typename T>
bool operator==(const GenericProp<M, G> &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T, GenericProp<M, G>>) { // RHS is generic prop
    return lhs.p == rhs.p;
  } else if constexpr (std::is_same_v<T,
                                      M<std::string, G>>) { // RHS is string map
    return std::get<M<std::string, G>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, M<double, G>>) {
    return std::get<M<double, G>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, M<std::vector<double>, G>>) {
    return std::get<M<std::vector<double>, G>>(lhs.p) == rhs;
  } else {
    return false;
  }
}

// Vertex Property:
template <typename GraphType = Graph>
using VertexProp = GenericProp<VertexMap, GraphType>;

// Edge Property:
template <typename GraphType = Graph>
using EdgeProp = GenericProp<EdgeMap, GraphType>;

// =====================
// Graph Property
// =====================

struct GraphProp {

  std::variant<std::string, double, std::vector<double>> p;

  GraphProp(const std::string &s) : p{s} {}

  GraphProp(const char *s) : p{std::string{s}} {}

  template <Arithmetic T>
  GraphProp(const T &n) {
    p.template emplace<double>(static_cast<double>(n));
  }

  template <Arithmetic T>
  GraphProp(const std::vector<T> &v) {
    p.template emplace<std::vector<double>>();
    for (const auto &e : v) {
      std::get<std::vector<double>>(p).push_back(static_cast<double>(e));
    }
  }

  GraphProp() = default;

  template <typename T>
  operator const T &() const {
    return std::get<T>(p);
  }

  template <typename T>
  operator T &() {
    return std::get<T>(p);
  }

  template <typename T>
  friend bool operator==(const GraphProp &lhs, const T &rhs);
};

template <typename T>
bool operator==(const GraphProp &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T, GraphProp>) { // RHS is GraphPropp

    return lhs.p == rhs.p;

  } else if constexpr (std::is_arithmetic_v<T>) { // RHS is arithmetic

    if constexpr (!std::is_same_v<T, double>) {
      return false;
    } else {
      return std::get<double>(lhs.p) == static_cast<double>(rhs);
    }

  } else if constexpr (std::is_same_v<T, std::string> ||
                       (std::is_array_v<std::remove_reference_t<T>> &&
                        std::is_same_v<
                            std::remove_extent_t<std::remove_reference_t<T>>,
                            char>)) { // RHS is string or string literal

    return std::get<std::string>(lhs.p) == rhs;

  } else {

    return false;
  }
}

// ===============================
// Property Maps
// ===============================

template <typename PropType>
struct GenericPropMap {
  std::map<std::string, PropType> data;

  PropType &operator[](const std::string &key) { return data[key]; }

  const PropType &operator[](const std::string &key) const {
    return data.at(key);
  }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }

  std::size_t size() const { return data.size(); }

  bool contains(const std::string &key) const {
    return data.find(key) != data.end();
  }

  nlohmann::json to_json() const {
    nlohmann::json j;
    for (const auto &[k, v] : data) {
      std::visit([&j, &k](const auto &e) { j[k] = e; }, v.p);
    }
    return j;
  }
};

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

  VertexPropMap<GraphType> &vertex;
  EdgePropMap<GraphType> &edge;

  GraphBundle() : graph{}, props{}, vertex{props.vertex}, edge{props.edge} {}

  GraphBundle(const GraphType &g)
      : graph{g}, props{g}, vertex{props.vertex}, edge{props.edge} {

    props.graph["num_vertices"] = boost::num_vertices(g);
    props.graph["num_edges"] = boost::num_edges(g);
  }

  GraphBundle(const GraphBundle &other)
      : graph{other.graph}, props{other.props}, vertex{props.vertex},
        edge{props.edge} {}

  GraphBundle(GraphBundle &&other)
      : graph{std::move(other.graph)}, props{std::move(other.props)},
        vertex{props.vertex}, edge{props.edge} {}

  GraphBundle &operator=(const GraphBundle &other) {
    if (this != &other) {
      graph = other.graph;
      props = other.props;
      vertex = props.vertex;
      edge = props.edge;
    }
    return *this;
  }

  GraphBundle &operator=(GraphBundle &&other) noexcept {
    if (this != &other) {
      graph = std::move(other.graph);
      props = std::move(other.props);
      vertex = props.vertex;
      edge = props.edge;
    }
    return *this;
  }

  auto &operator[](const std::string &key) { return props.graph[key]; }
  auto &operator[](const std::string &&key) { return props.graph[key]; }

  auto begin() { return props.graph.begin(); }
  auto end() { return props.graph.end(); }
  auto begin() const { return props.graph.begin(); }
  auto end() const { return props.graph.end(); }
};
} // namespace graph

using GraphBundle = graph::GraphBundle<Graph>;

} // namespace gl

} // namespace utils

template <template <typename...> typename M, typename G>
std::ostream &operator<<(std::ostream &os,
                         const utils::gl::GenericProp<M, G> &gp) {
  std::visit([&os](const auto &m) { os << m; }, gp.p);
  return os;
}

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
