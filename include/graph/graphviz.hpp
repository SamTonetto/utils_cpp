/**********************************************************************
 * @brief Utilities for writing to graphviz
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#include "graph/graph.hpp"

#include <unordered_map>
#include <vector>

namespace utils {
namespace gl {

template <typename GraphType>
struct GraphWriter {

  using string_map = std::unordered_map<std::string, std::string>;

  string_map graph_props;
  VertexMap<string_map, GraphType> vertex_props;
  EdgeMap<string_map, GraphType> edge_props;

  GraphWriter(const GraphType &g) : g{g} {

    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
      vertex_props[Vertex<GraphType>{v}] = string_map{};
    }

    for (auto e : boost::make_iterator_range(boost::edges(g))) {
      edge_props[Edge<GraphType>{e}] = string_map{};
    }
  }

  std::string get_graph_type_string() const {

    if constexpr (std::is_same<typename boost::graph_traits<
                                   GraphType>::directed_category,
                               boost::directed_tag>::value) {
      return "digraph";
    } else {
      return "graph";
    }
  }

  void add_vertex_property(const std::string &name, const std::string &value) {
    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
      vertex_props[v][name] = value;
    }
  }

  void add_edge_property(const std::string &name, const std::string &value) {
    for (auto e : boost::make_iterator_range(boost::edges(g))) {
      edge_props[e][name] = value;
    }
  }

  void add_graph_property(const std::string &name, const std::string &value) {
    graph_props[name] = value;
  }

  void write(std::ostream &os) const {
    os << get_graph_type_string() << " G {\n";

    for (auto &p : graph_props) {
      os << "  " << p.first << "=" << p.second << ";\n";
    }

    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
      os << "  " << v;

      if (vertex_props.at(v).size() > 0) {
        os << " [";

        std::size_t index = 0;
        for (auto &p : vertex_props.at(v)) {
          os << p.first << "=" << p.second;
          if (index + 1 < vertex_props.at(v).size()) {
            os << ", ";
          }
          ++index;
        }
        os << "];\n";
      } else {
        os << ";\n";
      }
    }

    for (auto e : boost::make_iterator_range(boost::edges(g))) {
      os << "  " << boost::source(e, g) << " -> " << boost::target(e, g);

      if (edge_props.at(e).size() > 0) {
        os << " [";
        std::size_t index = 0;
        for (auto &p : edge_props.at(e)) {
          os << p.first << "=" << p.second;
          if (index + 1 < edge_props.at(e).size()) {
            os << ", ";
          }
          ++index;
        }
        os << "];\n";
      } else {
        os << ";\n";
      }
    }

    os << "}\n";
  }

  const GraphType &g;
};

} // namespace gl

} // namespace utils
