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

// Vertex removal

void remove_vertices(GraphBundle &g, const std::vector<std::size_t> &vertices);

void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove,
    unsigned seed = std::random_device{}());

void randomly_remove_vertices_without_disconnecting(
    Graph &g, double fraction_to_remove,
    unsigned seed = std::random_device{}());

// Edge removal

void remove_edges(const GraphBundle &gb,
                  const std::vector<std::array<std::size_t, 2>> &edges);

void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove,
    unsigned seed = std::random_device{}());

void randomly_remove_dges_without_disconnecting(
    Graph &g, double fraction_to_remove,
    unsigned seed = std::random_device{}());

// ==========================================
// =========== Implementation ===============
// ==========================================

// ==== Vertices ====

inline void remove_vertices(GraphBundle &gb,

                            const std::vector<std::size_t> &vertices) {

  Graph new_graph;

  std::unordered_set<Vertex<Graph>> removal_set(vertices.begin(),
                                                vertices.end());

  std::unordered_map<Vertex<Graph>, Vertex<Graph>> index_map;

  Vertex<Graph> new_vertex_idx = 0;
  for (auto v : boost::make_iterator_range(boost::vertices(gb.graph))) {
    if (!removal_set.contains(v)) {
      index_map[v] = new_vertex_idx;
      ++new_vertex_idx;
    }
  }

  for (auto e : boost::make_iterator_range(boost::edges(gb.graph))) {
    auto u = boost::source(e, gb.graph);
    auto v = boost::target(e, gb.graph);

    if (!removal_set.contains(u) && !removal_set.contains(v)) {
      boost::add_edge(index_map[u], index_map[v], new_graph);
    }
  }

  nlohmann::json transform_metadata;
  transform_metadata["type"] = "remove_vertices";
  transform_metadata["removed_vertices"] = vertices;
  transform_metadata["removed_vertices_count"] = vertices.size();
  transform_metadata["removed_vertices_frac"] =
      static_cast<double>(vertices.size()) / boost::num_vertices(gb.graph);

  gb["transforms"].push_back(transform_metadata);

  gb.graph = std::move(new_graph);
}

// -------------------------------------------

inline void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove, unsigned seed) {
  std::mt19937 gen(seed);

  auto &g = gb.graph;

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

  remove_vertices(gb, to_actually_remove);
  gb["transforms"].back()["removed_vertices_seed"] = seed;
}

// -------------------------------------------

inline void randomly_remove_vertices_without_disconnecting(
    GraphBundle &gb, double fraction_to_remove, unsigned seed) {

  std::size_t num_to_remove = static_cast<std::size_t>(
      fraction_to_remove * boost::num_vertices(gb.graph));

  randomly_remove_vertices_without_disconnecting(gb, num_to_remove, seed);
}

// ==== Edges ====

inline void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, std::size_t num_to_remove, unsigned seed) {

  std::mt19937 gen(seed);

  auto &g = gb.graph;

  if (num_to_remove == 0) {
    return;
  }

  std::size_t original_num_edges = boost::num_edges(g);

  // bool = true => can select edge, else don't select.
  std::vector<std::pair<Edge<Graph>, bool>> edge_vec(boost::num_edges(g));
  for (auto edge : boost::make_iterator_range(boost::edges(g))) {
    edge_vec.push_back({edge, true});
  }

  std::vector<std::size_t> components(boost::num_vertices(g));

  std::size_t num_removed = 0;

  std::vector<std::array<std::size_t, 2>> removed_edges;
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
          removed_edges.push_back({{e.m_source, e.m_target}});
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

  nlohmann::json transform_metadata;
  transform_metadata["type"] = "remove_edges";
  transform_metadata["removed_edges"] = removed_edges;
  transform_metadata["removed_edges_count"] = removed_edges.size();
  transform_metadata["removed_edges_seed"] = seed;
  transform_metadata["removed_edges_frac"] =
      static_cast<double>(num_to_remove) / original_num_edges;

  gb["transforms"].push_back(transform_metadata);
}

// -------------------------------------------

inline void randomly_remove_edges_without_disconnecting(
    GraphBundle &gb, double fraction_to_remove, unsigned seed) {

  std::size_t num_to_remove =
      static_cast<std::size_t>(fraction_to_remove * boost::num_edges(gb.graph));

  randomly_remove_edges_without_disconnecting(gb, num_to_remove, seed);
}

// -------------------------------------------

inline void remove_edges(GraphBundle &gb,
                         const std::vector<std::array<std::size_t, 2>> &edges) {

  std::size_t original_num_edges = boost::num_edges(gb.graph);

  for (auto e : edges) {
    boost::remove_edge(e[0], e[1], gb.graph);
  }

  nlohmann::json transform_metadata;
  transform_metadata["type"] = "remove_edges";
  transform_metadata["removed_edges"] = edges;
  transform_metadata["removed_edges_count"] = edges.size();
  transform_metadata["removed_edges_frac"] =
      static_cast<double>(edges.size()) / original_num_edges;

  gb["transforms"].push_back(transform_metadata);
}

} // namespace gl

} // namespace utils
