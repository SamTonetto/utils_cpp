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
#include "graph/transforms_def.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include "print.hpp"

#include <random>

namespace utils {
namespace gl {

// ==== Vertices ====

inline GraphBundle remove_vertices(const GraphBundle &gb,
                                   const std::vector<std::size_t> &vertices) {

  // Instead of removing vertices from existing graph,
  // instead construct a new graph.
  // 1. Obtain a mapping from old -> new vertex indices: v -> f(v)
  // 2. Construct new graph from these mappings.

  Graph new_graph;

  std::unordered_set<Vertex<Graph>> removal_set(vertices.begin(),
                                                vertices.end());

  // Step 1.
  std::unordered_map<Vertex<Graph>, Vertex<Graph>> old_to_new_index_map;
  Vertex<Graph> new_vertex_idx = 0;
  for (auto v : boost::make_iterator_range(boost::vertices(gb.graph))) {
    if (!removal_set.contains(v)) {
      old_to_new_index_map[v] = new_vertex_idx;
      ++new_vertex_idx;
    }
  }

  // Step 2.
  for (auto e : boost::make_iterator_range(boost::edges(gb.graph))) {
    auto u = boost::source(e, gb.graph);
    auto v = boost::target(e, gb.graph);

    if (!removal_set.contains(u) && !removal_set.contains(v)) {
      boost::add_edge(old_to_new_index_map[u], old_to_new_index_map[v],
                      new_graph);
    }
  }

  // --- Update Properties ----

  std::unordered_map<Vertex<Graph>, Vertex<Graph>> new_to_old_index_map;
  for (auto &[k, v] : old_to_new_index_map) {
    new_to_old_index_map[v] = k;
  }

  Properties new_props(new_graph);

  // Update vertex properties with remapped indices

  for (auto &[key, val] : gb.props.vertex) {
    for (auto v : boost::make_iterator_range(boost::vertices(new_graph))) {
      new_props.vertex[key][v] = gb.props.vertex[key][new_to_old_index_map[v]];
    }
  }

  // Update edge properties with remapped indices

  for (auto &[key, val] : gb.props.edge) {
    for (auto new_edge : boost::make_iterator_range(boost::edges(new_graph))) {

      auto old_edge =
          boost::edge(new_to_old_index_map[boost::source(new_edge, new_graph)],
                      new_to_old_index_map[boost::target(new_edge, new_graph)],
                      gb.graph)
              .first;

      new_props.edge[key][new_edge] = gb.props.edge[key][old_edge];
    }
  }

  // Update global graph properties

  new_props.graph = gb.props.graph;
  new_props.graph["removed_vertices"] = vertices;

  return GraphBundle{new_graph, new_props};
}

// -------------------------------------------

inline std::vector<std::size_t>
get_vertices_to_remove(const Graph &g, std::size_t num_to_remove,
                       std::mt19937 &gen) {

  // Each while loop:
  // Iterate through shuffled vertices, testing each by isolating it.
  // - If remaining graph stays connected, add vertex to removal list.
  // - else, un-isolate the vertex and proceed to next vertex.
  // Isolating a vertex just means removing its incident edges. This has the
  // same topological effect as removal, but doesn't mess with vertex numbering.

  Graph new_g{g};

  std::vector<std::size_t> to_remove;

  std::vector<std::pair<Vertex<Graph>, bool>> candidates(
      boost::num_vertices(new_g));
  for (std::size_t i = 0; i < boost::num_vertices(new_g); ++i) {
    candidates[i] = {i, true};
  }

  std::vector<int> components(boost::num_vertices(new_g));

  while (to_remove.size() < num_to_remove) {

    std::shuffle(candidates.begin(), candidates.end(), gen);

    for (auto &[v, selectable] : candidates) {

      std::vector<Vertex<Graph>> cached_nbs;
      if (selectable) {
        for (auto nb :
             boost::make_iterator_range(boost::adjacent_vertices(v, new_g))) {
          cached_nbs.push_back(nb);
        }
      }

      auto num_conn_before = boost::connected_components(new_g, &components[0]);
      boost::clear_vertex(v, new_g);
      auto num_conn_after = boost::connected_components(new_g, &components[0]);

      bool no_longer_connected = (num_conn_after > num_conn_before + 1);
      if (no_longer_connected) {
        for (auto nb : cached_nbs) {
          boost::add_edge(v, nb, new_g);
        }
      } else {
        to_remove.push_back(v);
        if (to_remove.size() == num_to_remove) {
          break;
        }
      }

      selectable = false;
    }
  }
  return to_remove;
}

inline GraphBundle rand_prune_connected(const GraphBundle &gb,
                                        std::size_t num_to_remove,
                                        unsigned seed) {

  std::mt19937 gen(seed);

  if (num_to_remove == 0) {
    return gb;
  }

  std::vector<std::size_t> to_remove =
      get_vertices_to_remove(gb.graph, num_to_remove, gen);

  GraphBundle result = remove_vertices(gb, to_remove);
  return result;
}

// -------------------------------------------

inline GraphBundle rand_prune_connected(const GraphBundle &gb,
                                        double fraction_to_remove,
                                        unsigned seed) {

  std::size_t num_to_remove = static_cast<std::size_t>(
      fraction_to_remove * boost::num_vertices(gb.graph));

  GraphBundle result = rand_prune_connected(gb, num_to_remove, seed);
  return result;
}

// ==== Edges ====

inline GraphBundle
remove_edges(const GraphBundle &gb,
             const std::vector<std::vector<std::size_t>> &edges) {

  GraphBundle new_gb{gb};

  for (auto e : edges) {
    boost::remove_edge(e[0], e[1], new_gb.graph);
  }

  new_gb.props.graph["removed_edges"] = edges;
  return new_gb;
}

// -------------------------------------------

inline GraphBundle rand_prune_edges_connected(const GraphBundle &gb,
                                              std::size_t num_to_remove,
                                              unsigned seed) {

  std::mt19937 gen(seed);

  GraphBundle new_gb{gb};

  if (num_to_remove == 0) {
    return gb;
  }

  // .second = true => can select edge, else can't select it
  std::vector<std::pair<Edge<Graph>, bool>> edge_vec(
      boost::num_edges(new_gb.graph));
  for (auto edge : boost::make_iterator_range(boost::edges(new_gb.graph))) {
    edge_vec.push_back({edge, true});
  }

  std::vector<std::size_t> components(boost::num_vertices(new_gb.graph));

  std::vector<std::vector<std::size_t>> removed_edges;

  while (removed_edges.size() < num_to_remove) {

    std::shuffle(edge_vec.begin(), edge_vec.end(), gen);

    for (auto &[e, selectable] : edge_vec) {

      if (selectable) {

        auto num_conn_before =
            boost::connected_components(new_gb.graph, &components[0]);
        boost::remove_edge(e, new_gb.graph);
        auto num_conn_after =
            boost::connected_components(new_gb.graph, &components[0]);

        if (num_conn_after > num_conn_before) {
          boost::add_edge(e.m_source, e.m_target, new_gb.graph);
        } else {
          removed_edges.push_back({{e.m_source, e.m_target}});

          if (removed_edges.size() == num_to_remove) {
            break;
          }
        }
        selectable = false;
      }
    }
  }

  new_gb.props.graph["removed_edges"] = removed_edges;
  return new_gb;
}

// -------------------------------------------

inline GraphBundle rand_prune_edges_connected(const GraphBundle &gb,
                                              double fraction_to_remove,
                                              unsigned seed) {

  std::size_t num_to_remove =
      static_cast<std::size_t>(fraction_to_remove * boost::num_edges(gb.graph));

  return rand_prune_edges_connected(gb, num_to_remove, seed);
}

} // namespace gl

} // namespace utils
