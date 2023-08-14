/**********************************************************************
 * @brief Various graph utilities, e.g. graph transformations
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "graph/graph.hpp"
#include "graph/properties.hpp"
#include "graph/transforms.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <random>

namespace utils {
namespace gl {

/**
 * This is the main function that other random edge removal functions call.
 * Randomly remove an integer number of edges, but keep the graph connected.
 * Updates the GraphProperties object in the GraphBundle as well.
 */
void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove,
    unsigned seed = std::random_device{}());

/**
 * Randomly remove a fraction of edges.
 */
void randomly_remove_dges_without_disconnecting(
    Graph &g, double fraction_to_remove,
    unsigned seed = std::random_device{}());

/**
 * This is the main function that other random vertex removal functions call.
 * Randomly remove an integer number of vertex, but keep the graph connected.
 * Updates the GraphProperties object in the GraphBundle as well.
 */
void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove,
    unsigned seed = std::random_device{}());

/**
 * Randomly remove a fraction of vertices.
 */
void randomly_remove_vertices_without_disconnecting(
    Graph &g, double fraction_to_remove,
    unsigned seed = std::random_device{}());

// ==========================================
// =========== Implementation ===============
// ==========================================

inline void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove, unsigned seed) {

  std::mt19937 gen(seed);

  auto &g = gb.graph;

  gb["removed_edges"] = num_to_remove;
  gb["removed_edges_seed"] = seed;
  gb["removed_edges_frac"] =
      static_cast<double>(num_to_remove) / boost::num_edges(g);

  if (num_to_remove == 0) {
    return;
  }

  // bool = true => can select edge, else don't select.
  std::vector<std::pair<Edge<Graph>, bool>> edge_vec;
  for (auto edge : boost::make_iterator_range(boost::edges(g))) {
    edge_vec.push_back({edge, true});
  }

  // boilerplate for using boost connected components.
  std::vector<std::size_t> components(boost::num_vertices(g));

  std::size_t num_removed = 0;
  while (num_removed < num_to_remove) {

    bool none_left_to_select = true;

    std::shuffle(edge_vec.begin(), edge_vec.end(), gen);

    for (std::size_t i = 0; i < edge_vec.size(); ++i) {

      if (edge_vec[i].second) {

        // unsuccessful exit flag
        none_left_to_select = false;

        auto e = edge_vec[i].first;

        std::size_t num_connected_before =
            boost::connected_components(g, &components[0]);

        boost::remove_edge(e, g);

        std::size_t num_connected_after =
            boost::connected_components(g, &components[0]);

        if (num_connected_after > num_connected_before) {
          boost::add_edge(e.m_source, e.m_target, g);
        } else {
          ++num_removed;
          if (num_removed == num_to_remove) {
            return;
          }
        }

        edge_vec[i].second = false;
      }
    }

    if (none_left_to_select) {
      throw std::runtime_error(
          "Unable to achieve given density without disconnecting graph.");
    }
  }
}

inline void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, double fraction_to_remove, unsigned seed) {

  std::size_t num_to_remove =
      static_cast<std::size_t>(fraction_to_remove * boost::num_edges(gb.graph));

  randomly_remove_edges_without_disconnecting(gb, num_to_remove, seed);
}

inline void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove, unsigned seed) {
  std::mt19937 gen(seed);

  auto &g = gb.graph;

  gb["removed_vertices"] = num_to_remove;
  gb["removed_vertices_seed"] = seed;
  gb["removed_vertices_frac"] =
      static_cast<double>(num_to_remove) / boost::num_vertices(g);

  if (num_to_remove == 0) {
    return;
  }

  // bool = true => can select vertex, else don't select.
  std::vector<std::pair<Vertex<Graph>, bool>> vertex_vec;
  for (auto v : boost::make_iterator_range(boost::vertices(g))) {
    vertex_vec.push_back({v, true});
  }

  // boilerplate for using boost connected components.
  std::vector<int> components(boost::num_vertices(g));

  std::size_t num_removed = 0;

  std::vector<std::size_t> to_actually_remove;

  while (true) {

    // unsuccessful exit flag
    bool none_left_to_select = true;
    bool end_while = false;

    std::shuffle(vertex_vec.begin(), vertex_vec.end(), gen);

    for (std::size_t i = 0; i < vertex_vec.size(); ++i) {

      if (vertex_vec[i].second) {

        none_left_to_select = false;

        auto v = vertex_vec[i].first;

        // Caching neighbors in-case restoration is necessary
        std::vector<Vertex<Graph>> nbs;
        for (auto nb :
             boost::make_iterator_range(boost::adjacent_vertices(v, g))) {
          nbs.push_back(nb);
        }

        std::size_t num_connected_before =
            boost::connected_components(g, &components[0]);

        // Don't actually remove vertex, just incident edges. Has same effect
        // on connectivity without screwing up vertex indexing.
        boost::clear_vertex(v, g);

        std::size_t num_connected_after =
            boost::connected_components(g, &components[0]);

        // clearing vertex implicitly introduces one more component, so just
        // check it doesn't increase by more than 1.

        if (num_connected_after > num_connected_before + 1) {

          for (auto nb : nbs) {
            boost::add_edge(v, nb, g);
          }
        } else {

          ++num_removed;
          to_actually_remove.push_back(vertex_vec[i].first);

          if (num_removed == num_to_remove) {

            end_while = true;
            break;
          }
        }

        vertex_vec[i].second = false;
      }
    }

    if (end_while) {
      break;
    }

    if (none_left_to_select) {
      throw std::runtime_error(
          "Unable to achieve given density without disconnecting graph.");
    }
  }

  // Sort from higher to lowest index, to make removal safe.
  std::sort(to_actually_remove.begin(), to_actually_remove.end(),
            std::greater<>());

  for (auto v : to_actually_remove) {
    boost::remove_vertex(v, g);
  }
}

inline void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, double fraction_to_remove, unsigned seed) {

  std::size_t num_to_remove = static_cast<std::size_t>(
      fraction_to_remove * boost::num_vertices(gb.graph));

  randomly_remove_vertices_without_disconnecting(gb, num_to_remove, seed);
}

} // namespace gl

} // namespace utils
