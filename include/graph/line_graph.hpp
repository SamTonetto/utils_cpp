/**********************************************************************
 * @brief Transform a graph into its corresponding line graph. Based heavily on
 *networkx implementation.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "graph.hpp"
#include "print.hpp"

namespace utils {

namespace gl {

/**
 * @brief Given an undirected graph g, creates a new graph lg such that vertices
 of LG
 * correspond to edges of g, and edges of lg correspond to edges of g that share
 * a common vertex. This is known as the 'line graph' of g.
 *
 * @param g The undirected input graph.
 * @return std::pair<Graph, VertexMap<Edge<>>> The line graph of g and a map
 * from vertices of lg to edges of g.
 */
inline std::pair<Graph, VertexMap<Edge<>>> line_graph(const Graph &g) {

  Graph lg(boost::num_edges(g));

  EdgeMap<Vertex<>, Graph> edge_to_vertex;
  VertexMap<Edge<>, Graph> vertex_to_edge;

  std::size_t index = 0;
  for (Edge<> e : boost::make_iterator_range(boost::edges(g))) {
    edge_to_vertex[e] = index;
    vertex_to_edge[index] = e;
    ++index;
  }

  for (Vertex<> v : boost::make_iterator_range(boost::vertices(g))) {

    std::vector<std::size_t> lg_clique_nodes;

    for (Vertex<> nb :
         boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
      lg_clique_nodes.push_back(edge_to_vertex[boost::edge(v, nb, g).first]);
    }

    for (std::size_t i = 0; i < lg_clique_nodes.size(); ++i) {
      for (std::size_t j = i + 1; j < lg_clique_nodes.size(); ++j) {
        boost::add_edge(lg_clique_nodes[i], lg_clique_nodes[j], lg);
      }
    }
  }
  return {lg, vertex_to_edge};
}

} // namespace gl

} // namespace utils
