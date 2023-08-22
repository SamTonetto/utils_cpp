#pragma once

#include "graph.hpp"
#include "nlohmann/json.hpp"

namespace utils {

namespace gl {

class Properties {
public:
  std::map<std::string, nlohmann::json> graph;
  std::map<std::string, nlohmann::json> vertex;
  std::map<std::string, nlohmann::json> edge;

  Properties() = default;

  template <typename GraphType>
  Properties(const GraphType &g) {
    graph["num_vertices"] = boost::num_vertices(g);
    graph["num_edges"] = boost::num_edges(g);
  }
};

namespace graph {

template <typename GraphType>
class GraphBundle {
public:
  GraphType graph;

private:
  Properties props;

public:
  std::map<std::string, nlohmann::json> &vertex;
  std::map<std::string, nlohmann::json> &edge;

public:
  GraphBundle() : graph{}, props{}, vertex{props.vertex}, edge{props.edge} {}

  GraphBundle(const GraphType &g)
      : props{}, vertex{props.vertex}, edge{props.edge} {
    props.graph["num_vertices"] = boost::num_vertices(g);
    props.graph["num_edges"] = boost::num_edges(g);
  }

  GraphBundle(const GraphBundle &other)
      : graph{other.graph}, props{other.props}, vertex{props.vertex},
        edge{props.edge} {}

  GraphBundle(GraphBundle &&other) noexcept
      : graph{std::move(other.graph)}, props{std::move(other.props)},
        vertex{props.vertex}, edge{props.edge} {}

  GraphBundle &operator=(const GraphBundle &other) {
    if (this != &other) {
      graph = other.graph;
      props = other.props;
    }
    return *this;
  }

  GraphBundle &operator=(GraphBundle &&other) noexcept {
    if (this != &other) {
      graph = std::move(other.graph);
      props = std::move(other.props);
    }
    return *this;
  }

  auto &operator[](const std::string &key) const { return props.graph.at(key); }
  auto &operator[](std::string &&key) { return props.graph[key]; }

  auto begin() { return props.graph.begin(); }
  auto end() { return props.graph.end(); }
  auto begin() const { return props.graph.begin(); }
  auto end() const { return props.graph.end(); }
};

} // namespace graph

using GraphBundle = graph::GraphBundle<Graph>;

} // namespace gl

} // namespace utils
