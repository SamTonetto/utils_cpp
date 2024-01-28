#pragma once

#include "utils_cpp/graph/graph.hpp"
#include "utils_cpp/graph/properties.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <random>
#include <unordered_map>

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

/**
 * Relabel the vertices according to a 1-to-1 vertex mapping.
 */
Graph relabel_vertices(
    const Graph &g,
    const std::unordered_map<std::size_t, std::size_t> &mapping);

/**
 * Shuffle existing vertex labels, maintaining the same graph topology.
 */
template <typename RandomGenerator>
Graph shuffle_vertex_labels(const Graph &g, RandomGenerator &&gen);

Graph shuffle_vertex_labels(const Graph &g, unsigned seed);

/**
 * Make a graph's vertices "contiguous". If there are any isolated vertices in
 * the graph, it "flattens" them out, while keeping other vertices in the same
 * order, but collapsing them to make them contiguous. Returns the contiguized
 * graph, as well as the reverse vertex label-mapping (so that the original
 * graph may be recovered).
 *
 * Note that as long as the reverse mapping is kept, the contiguization can be
 * undone by simply relabelling vertices according to the reverse mapping.
 */
std::pair<Graph, std::unordered_map<std::size_t, std::size_t>>
contiguize(const Graph &g);

} // namespace gl
} // namespace utils
