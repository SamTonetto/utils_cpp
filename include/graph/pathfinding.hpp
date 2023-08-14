/** @file
 * @brief Functions for running various graph search algorithms
 */

#pragma once

#include "graph/graph.hpp"

#include <boost/graph/astar_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace utils {
namespace gl {

/**
 *   @brief Run BFS from a source vertex to all other vertices in the graph
 *   @param g The graph to search
 *   @param source The source vertex
 *   @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 *   predecessors of each vertex in the graph
 */
template <typename GraphType>
std::pair<std::vector<int>, std::vector<Vertex<GraphType>>>
bfs(const GraphType &g, Vertex<GraphType> source) {

  std::vector<int> distances(boost::num_vertices(g));
  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  auto recorders = std::make_pair(
      boost::record_distances(distances.data(), boost::on_tree_edge{}),
      boost::record_predecessors(predecessors.data(), boost::on_tree_edge{}));

  boost::breadth_first_search(
      g, source, boost::visitor(boost::make_bfs_visitor(recorders)));

  return {distances, predecessors};
}

/**
 * @brief  Run BFS from a source vertex to all other vertices in the graph,
 * returning distances only
 * @param g The graph to search
 * @param source The source vertex
 * @return A vector containing the distances from the source vertex to each
 * vertex in the graph.
 */
template <typename GraphType>
std::vector<int> bfs_distances(const GraphType &g, Vertex<GraphType> source) {

  std::vector<int> distances(boost::num_vertices(g));

  auto recorder =
      boost::record_distances(distances.data(), boost::on_tree_edge{});

  boost::breadth_first_search(
      g, source, boost::visitor(boost::make_bfs_visitor(recorder)));

  return distances;
}

/**
 * @brief Run BFS from a source vertex to all other vertices in the graph,
  returning predecessors only
 * @param g The graph to search
 * @param source The source vertex
 * @return A vector containing the predecessors of each vertex in the graph.
 */
template <typename GraphType>
std::vector<Vertex<GraphType>> bfs_predecessors(const GraphType &g,
                                                Vertex<GraphType> source) {

  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  auto recorder =
      boost::record_predecessors(predecessors.data(), boost::on_tree_edge{});

  boost::breadth_first_search(
      g, source, boost::visitor(boost::make_bfs_visitor(recorder)));

  return predecessors;
}

/**
 * @brief Run Dijkstra from a source vertex to all other vertices in the graph,
  returning predecessors only
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights
 * @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 * predecessors of each vertex in the graph
 */
template <typename GraphType, typename DistanceType>
std::pair<std::vector<DistanceType>, std::vector<Vertex<GraphType>>>
dijkstra(const GraphType &g, Vertex<GraphType> source,
         const EdgeMap<DistanceType, GraphType> &weight_map) {

  std::vector<DistanceType> distances(boost::num_vertices(g));
  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::dijkstra_shortest_paths(
      g, source,
      boost::predecessor_map(predecessors.data())
          .distance_map(distances.data())
          .weight_map(boost::make_assoc_property_map(weight_map)));

  return {distances, predecessors};
}

/**
 * @brief Run Dijkstra from a source vertex to all other vertices in the
 * graph, returning distances only
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights
 * @return A vector containing the distances from the source vertex to each
 vertex in the graph.
 */
template <typename GraphType, typename DistanceType>
std::vector<DistanceType>
dijkstra_distances(const GraphType &g, Vertex<GraphType> source,
                   const EdgeMap<DistanceType, GraphType> &weight_map) {

  std::vector<DistanceType> distances(boost::num_vertices(g));

  boost::dijkstra_shortest_paths(
      g, source,
      boost::distance_map(distances.data())
          .weight_map(boost::make_assoc_property_map(weight_map)));

  return distances;
}

/**
 * @brief Run Dijkstra from a source vertex to all other vertices in the graph,
 *        returning predecessors only
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights
 * @return A vector containing the predecessors of each vertex in the graph.
 */
template <typename GraphType, typename DistanceType>
std::vector<Vertex<GraphType>>
dijkstra_predecessors(const GraphType &g, Vertex<GraphType> source,
                      const EdgeMap<DistanceType, GraphType> &weight_map) {

  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::dijkstra_shortest_paths(
      g, source,
      boost::predecessor_map(predecessors.data())
          .weight_map(boost::make_assoc_property_map(weight_map)));

  return predecessors;
}

/**
 * @brief Run A* from a source vertex, using a
 *        given heuristic, returning distances and predecessors
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights
 * @param heuristic The heuristic to use
 * @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 * predecessors of each vertex in the graph
 * @todo Figure out how to actually use boost astar properly, with custom
 * heuristics as well, and produce working tests and demos.
 */
template <typename GraphType, typename DistanceType, typename AStarHeuristic>
std::pair<std::vector<DistanceType>, std::vector<Vertex<GraphType>>>
astar(const GraphType &g, Vertex<GraphType> source,
      const EdgeMap<DistanceType, GraphType> &weight_map,
      const AStarHeuristic &heuristic) {

  std::vector<DistanceType> distances(boost::num_vertices(g));
  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::astar_search(g, source, heuristic,
                      boost::predecessor_map(predecessors.data())
                          .distance_map(distances.data())
                          .weight_map(weight_map));

  return {distances, predecessors};
}

/**
 * @brief Run A* from a source vertex to a target vertex, using a
 *        given heuristic, returning distances only
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights
 * @param heuristic The heuristic to use
 * @return A vector containing the distances from the source vertex to each
 * vertex in the graph.
 */
template <typename GraphType, typename DistanceType, typename AStarHeuristic>
std::vector<DistanceType>
astar_distances(const GraphType &g, Vertex<GraphType> source,
                const EdgeMap<DistanceType, GraphType> &weight_map,
                const AStarHeuristic &heuristic) {

  std::vector<DistanceType> distances(boost::num_vertices(g));

  boost::astar_search(
      g, source, heuristic,
      boost::distance_map(distances.data()).weight_map(weight_map));

  return distances;
}

/**
 * @brief Run A* from a source vertex to a target vertex, using a
 *        given heuristic, returning predecessors only
 * @param g The graph to search
 * @param source The source vertex
 * @param target The target vertex
 * @param weight_map A map from edges to their weights
 * @param heuristic The heuristic to use
 * @return A vector containing the predecessors of each vertex in the graph.
 */
template <typename GraphType, typename DistanceType, typename AStarHeuristic>
std::vector<Vertex<GraphType>>
astar_predecessors(const GraphType &g, Vertex<GraphType> source,
                   const EdgeMap<GraphType, GraphType> &weight_map,
                   const AStarHeuristic &heuristic) {

  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::astar_search(
      g, source, heuristic,
      boost::predecessor_map(predecessors.data()).weight_map(weight_map));

  return predecessors;
}

//============== A-Star Heuristics ==============

/**
 * @brief A Euclidean heuristic for A* search, valid for graphs with vertex
 * properties x and y.
 * @param GraphType The type of the graph
 * @param CostType The type of the cost
 * @param LocMap The type of the location map
 */
template <typename GraphType, typename CostType, typename LocMap>
class EuclideanHeuristic : public boost::astar_heuristic<GraphType, CostType> {

public:
  /**
   * @brief Construct a new Euclidean Heuristic object
   * @param l The location map
   * @param goal The goal vertex
   */
  EuclideanHeuristic(const LocMap &loc_map, Vertex<GraphType> goal)
      : loc_map{loc_map}, goal{goal} {}

  /**
   * @brief Compute the Euclidean distance from a vertex to the goal
   * @param v The vertex
   * @return The Euclidean distance from v to the goal
   */
  CostType operator()(Vertex<GraphType> v) {
    CostType dx = loc_map[goal].x - loc_map[v].x;
    CostType dy = loc_map[goal].y - loc_map[v].y;
    return std::sqrt(dx * dx + dy * dy);
  }

private:
  LocMap loc_map;
  Vertex<GraphType> goal;
};

/**
 * @brief A Manhattan heuristic for A* search, valid for graphs with vertex
 * properties x and y.
 * @param GraphType The type of the graph
 * @param CostType The type of the cost
 * @param LocMap The type of the location map
 */
template <typename GraphType, typename CostType, typename LocMap>
class ManhattanHeuristic : public boost::astar_heuristic<GraphType, CostType> {

public:
  /**
   * @brief Construct a new Manhattan Heuristic object
   * @param l The location map
   * @param goal The goal vertex
   */
  ManhattanHeuristic(const LocMap &loc_map, Vertex<GraphType> goal)
      : loc_map{loc_map}, goal{goal} {}

  /**
   * @brief Compute the Manhattan distance from a vertex to the goal
   * @param v The vertex
   * @return The Manhattan distance from v to the goal
   */
  CostType operator()(Vertex<GraphType> v) {
    CostType dx = std::abs(loc_map[goal].x - loc_map[v].x);
    CostType dy = std::abs(loc_map[goal].y - loc_map[v].y);
    return dx + dy;
  }

private:
  LocMap loc_map;
  Vertex<GraphType> goal;
};

// ===================== A-Star Visitors =====================

/**
 * For some weird reason, boost pathfinding algorithms implement early-stopping
 * via throwing an exception. This is pretty inefficient, so we try to implement
 * early-stopping without throwing an exception.
 */
template <typename Visitors = boost::null_visitor>
class EarlyStoppingVisitor : public Visitors {};

} // namespace gl

} // namespace utils
