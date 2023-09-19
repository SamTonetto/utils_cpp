#pragma once

#include "graph/graph.hpp"
#include "graph/properties.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <random>

namespace utils {
namespace gl {

/**
 * Remove vertices, returning an updated GraphBundle.
 */
GraphBundle remove_vertices(const GraphBundle &g,
                            const std::vector<std::size_t> &vertices);

GraphBundle rand_prune_connected(const GraphBundle &gb,
                                 std::size_t num_to_remove,
                                 unsigned seed = std::random_device{}());

GraphBundle rand_prune_connected(const Graph &g, double fraction_to_remove,
                                 unsigned seed = std::random_device{}());

/**
 * Remove edges, returning an updated GraphBundle.
 */
GraphBundle remove_edges(const GraphBundle &gb,
                         const std::vector<std::vector<std::size_t>> &edges);

GraphBundle rand_prune_edges_connected(const GraphBundle &gb,
                                       std::size_t num_to_remove,
                                       unsigned seed = std::random_device{}());

GraphBundle rand_prune_edges_connected(const Graph &g,
                                       double fraction_to_remove,
                                       unsigned seed = std::random_device{}());

} // namespace gl
} // namespace utils
