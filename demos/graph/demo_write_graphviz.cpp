#include "graph/graph.hpp"

#include <boost/graph/graphviz.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <fstream>
#include <iostream>

using namespace utils;

/**
 * @private
 */
struct VertexProperty {
  std::string label;
  double x;
  double y;
};

/**
 * @private
 */
struct EdgeProperty {
  int weight;
};

// a.k.a `using Graph = gl::graph::Graph<VertexProperty, EdgeProperty>;`
using Graph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                          VertexProperty, EdgeProperty>;

int main() {

  // Creating a graph
  Graph g;
  boost::add_vertex(VertexProperty{"A", 0.0, 0.0}, g);
  boost::add_vertex(VertexProperty{"B", 1.0, 0.0}, g);
  boost::add_vertex(VertexProperty{"C", 0.0, 1.0}, g);
  boost::add_edge(0, 1, EdgeProperty{1}, g);
  boost::add_edge(1, 2, EdgeProperty{2}, g);
  boost::add_edge(2, 0, EdgeProperty{3}, g);

  // boost::dynamic_properties allows us to dynamically create properties
  // external to the graph that can be used to write the DOT file.
  boost::dynamic_properties dp;

  // node_id *must* be specified for boost::write_graphviz_dp to work
  dp.property("node_id", boost::get(boost::vertex_index, g));

  // Extracting existing interior label property into dynamic property maps
  dp.property("label", boost::get(&VertexProperty::label, g));

  // Creating new dynamic vertex properties
  gl::VertexMap<std::string> positionMap, pinMap, shapeMap;
  for (auto v : boost::make_iterator_range(boost::vertices(g))) {
    positionMap[v] =
        std::to_string(g[v].x) + "," + std::to_string(g[v].y) + "!";
    pinMap[v] = "true";
    shapeMap[v] = "circle";
  }
  dp.property("pos", boost::make_assoc_property_map(positionMap));
  dp.property("pin", boost::make_assoc_property_map(pinMap));
  dp.property("shape", boost::make_assoc_property_map(shapeMap));

  // Creating new dynamic edge properties
  gl::EdgeMap<double> widthMap;
  gl::EdgeMap<std::string> colorMap;
  for (auto e : boost::make_iterator_range(boost::edges(g))) {
    widthMap[e] = g[e].weight;

    if (g[e].weight == 1) {
      colorMap[e] = "red";
    } else if (g[e].weight == 2) {
      colorMap[e] = "blue";
    } else if (g[e].weight == 3) {
      colorMap[e] = "green";
    }
  }
  dp.property("penwidth", boost::make_assoc_property_map(widthMap));
  dp.property("color", boost::make_assoc_property_map(colorMap));

  // Writing the graph to a DOT file
  std::ofstream ofs("graph.dot");
  boost::write_graphviz_dp(ofs, g, dp);

  return 0;
}
