#pragma once

#include <iostream>
#include <memory>
#include <typeinfo>

#include "graph.hpp"

namespace utils {
namespace gl {

template <typename... Ts>
struct variant_unwrapper {
  std::variant<Ts...> &var;

  template <typename T>
  operator T &() {
    return std::get<T>(var);
  }

  template <typename T>
  operator T &&() {
    return std::get<T>(var);
  }
};

template <typename... Ts>
struct const_variant_unwrapper {
  const std::variant<Ts...> &var;

  template <typename T>
  operator const T &() {
    return std::get<T>(var);
  }

  template <typename T>
  operator const T &&() {
    return std::get<T>(var);
  }
};

template <typename... Ts>
variant_unwrapper(std::variant<Ts...> &) -> variant_unwrapper<Ts...>;
template <typename... Ts>
const_variant_unwrapper(const std::variant<Ts...> &)
    -> const_variant_unwrapper<Ts...>;

// Graph Properties

template <template <typename...> typename Map, typename GraphType>
using mapping_type = std::variant<
    Map<std::string, GraphType>, Map<double, GraphType>, Map<int, GraphType>,
    Map<std::size_t, GraphType>, Map<std::vector<std::string>, GraphType>,
    Map<std::vector<double>, GraphType>, Map<std::vector<int>, GraphType>,
    Map<std::vector<std::size_t>, GraphType>,
    Map<std::array<double, 2>, GraphType>, Map<std::array<int, 2>, GraphType>,
    Map<std::array<std::size_t, 2>, GraphType>,
    Map<std::pair<double, double>, GraphType>,
    Map<std::pair<int, int>, GraphType>,
    Map<std::pair<std::size_t, std::size_t>, GraphType>>;

// Same template arguments as the variant...
template <template <typename...> typename Map, typename GraphType>
using variant_unwrapper_type = variant_unwrapper<
    Map<std::string, GraphType>, Map<double, GraphType>, Map<int, GraphType>,
    Map<std::size_t, GraphType>, Map<std::vector<std::string>, GraphType>,
    Map<std::vector<double>, GraphType>, Map<std::vector<int>, GraphType>,
    Map<std::vector<std::size_t>, GraphType>,
    Map<std::array<double, 2>, GraphType>, Map<std::array<int, 2>, GraphType>,
    Map<std::array<std::size_t, 2>, GraphType>,
    Map<std::pair<double, double>, GraphType>,
    Map<std::pair<int, int>, GraphType>,
    Map<std::pair<std::size_t, std::size_t>, GraphType>>;

template <template <typename...> typename Map, typename GraphType>
using const_variant_unwrapper_type = const_variant_unwrapper<
    Map<std::string, GraphType>, Map<double, GraphType>, Map<int, GraphType>,
    Map<std::size_t, GraphType>, Map<std::vector<std::string>, GraphType>,
    Map<std::vector<double>, GraphType>, Map<std::vector<int>, GraphType>,
    Map<std::vector<std::size_t>, GraphType>,
    Map<std::array<double, 2>, GraphType>, Map<std::array<int, 2>, GraphType>,
    Map<std::array<std::size_t, 2>, GraphType>,
    Map<std::pair<double, double>, GraphType>,
    Map<std::pair<int, int>, GraphType>,
    Map<std::pair<std::size_t, std::size_t>, GraphType>>;

template <typename GraphType>
struct VertexProp {

  mapping_type<VertexMap, GraphType> vprop;

  template <typename T>
  VertexProp(const T &t) : vprop(t) {}

  VertexProp() = default;

  template <typename T>
  VertexMap<T, GraphType> &get() {
    return std::get<VertexMap<T, GraphType>>(vprop);
  }

  template <typename T>
  const VertexMap<T, GraphType> &get() const {
    return std::get<VertexMap<T, GraphType>>(vprop);
  }

  variant_unwrapper_type<VertexMap, GraphType> get() {
    return variant_unwrapper{vprop};
  }
  const_variant_unwrapper_type<VertexMap, GraphType> get() const {
    return const_variant_unwrapper{vprop};
  }
};

template <typename GraphType>
struct EdgeProp {

  mapping_type<EdgeMap, GraphType> eprop;

  template <typename T>
  EdgeProp(const T &t) : eprop(t) {}

  EdgeProp() = default;

  template <typename T>
  EdgeMap<T, GraphType> &get() {
    return std::get<EdgeMap<T, GraphType>>(eprop);
  }

  template <typename T>
  const EdgeMap<T, GraphType> &get() const {
    return std::get<EdgeMap<T, GraphType>>(eprop);
  }

  variant_unwrapper_type<EdgeMap, GraphType> get() {
    return variant_unwrapper{eprop};
  }

  const_variant_unwrapper_type<EdgeMap, GraphType> get() const {
    return const_variant_unwrapper{eprop};
  }
};

struct GraphProp {

  using variant_wrapper_type = variant_unwrapper<
      std::string, double, int, std::size_t, std::vector<double>,
      std::pair<double, double>, std::vector<int>, std::pair<int, int>,
      std::vector<std::size_t>, std::pair<std::size_t, std::size_t>>;

  using const_variant_wrapper_type = const_variant_unwrapper<
      std::string, double, int, std::size_t, std::vector<double>,
      std::pair<double, double>, std::vector<int>, std::pair<int, int>,
      std::vector<std::size_t>, std::pair<std::size_t, std::size_t>>;

  using type =
      std::variant<std::string, double, int, std::size_t, std::vector<double>,
                   std::pair<double, double>, std::vector<int>,
                   std::pair<int, int>, std::vector<std::size_t>,
                   std::pair<std::size_t, std::size_t>>;

  type gprop;

  template <typename T>
  GraphProp(const T &t) : gprop(t) {}

  GraphProp() = default;

  template <typename T>
  T &get() {
    return std::get<T>(gprop);
  }

  template <typename T>
  const T &get() const {
    return std::get<T>(gprop);
  }

  variant_wrapper_type get() { return variant_unwrapper{gprop}; }
  const_variant_wrapper_type get() const {
    return const_variant_unwrapper{gprop};
  }
};

struct GraphPropMap {
  std::map<std::string, GraphProp> data;

  GraphProp &operator[](const std::string &key) { return data[key]; }

  const GraphProp &operator[](const std::string &key) const {
    return data.at(key);
  }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }
};

template <typename GraphType>
struct VertexPropMap {
  std::map<std::string, VertexProp<GraphType>> data;

  VertexProp<GraphType> &operator[](const std::string &key) {
    return data[key];
  }

  const VertexProp<GraphType> &operator[](const std::string &key) const {
    return data.at(key);
  }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }
};

template <typename GraphType>
struct EdgePropMap {
  std::map<std::string, EdgeProp<GraphType>> data;

  EdgeProp<GraphType> &operator[](const std::string &key) { return data[key]; }

  const EdgeProp<GraphType> &operator[](const std::string &key) const {
    return data.at(key);
  }
  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }
};

template <typename GraphType = Graph>
struct Properties {
  GraphPropMap graph;
  VertexPropMap<GraphType> vertex;
  EdgePropMap<GraphType> edge;

  Properties() = default;

  Properties(const GraphType &g) {
    graph["num_vertices"] = boost::num_vertices(g);
    graph["num_edges"] = boost::num_edges(g);
  }
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

  VertexPropMap<GraphType> vertex; // vertex props
  EdgePropMap<GraphType> edge;     // edge props

  GraphProp &operator[](const std::string &key) { return global_props[key]; }

  const GraphProp &operator[](const std::string &key) const {
    return global_props.at(key);
  }

  auto begin() { return global_props.begin(); }
  auto end() { return global_props.end(); }
  auto begin() const { return global_props.begin(); }
  auto end() const { return global_props.end(); }

private:
  std::map<std::string, GraphProp> global_props;
};

} // namespace graph

using GraphBundle = graph::GraphBundle<Graph>;

} // namespace gl

} // namespace utils
