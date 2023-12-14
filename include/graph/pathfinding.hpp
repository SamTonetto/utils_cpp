/**********************************************************************
 * @brief Graph pathfinding wrappers around boost graph.
 * @details Includes BFS, Dijkstra and A-star. "WeightType" is the type of both
 * the weight-map and the computed distances. "CostType" is the type returned
 * from the A-star heuristic. Note that although WeightType and CostType will
 * often be the same type, this is not always the case. For example, even if a
 * graph has entirely integer-valued weights, the Euclidean distance may not be
 * integer-valued due to the square root.

 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "graph/graph.hpp"

#include <boost/graph/astar_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <type_traits>

namespace utils {
namespace gl {

namespace detail {

template <typename GraphType>
inline EdgeMap<std::size_t, GraphType>
make_unit_weight_map(const GraphType &g) {
  EdgeMap<std::size_t, GraphType> weight_map;
  weight_map.reserve(boost::num_edges(g));
  for (auto e : boost::make_iterator_range(boost::edges(g))) {
    weight_map[e] = 1;
  }
  return weight_map;
}

} // namespace detail

/**
 *   @brief Run BFS from a source vertex to all other vertices in the graph
 *   @param g The graph to search
 *   @param source The source vertex
 *   @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 *   predecessors of each vertex in the graph
 */
template <typename GraphType>
std::pair<std::vector<std::size_t>, std::vector<Vertex<GraphType>>>
bfs(const GraphType &g, Vertex<GraphType> source) {

  std::vector<std::size_t> distances(boost::num_vertices(g));
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
std::vector<std::size_t> bfs_distances(const GraphType &g,
                                       Vertex<GraphType> source) {

  std::vector<std::size_t> distances(boost::num_vertices(g));

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
 * @param weight_map (optional) A map from edges to their weights.
 * @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 * predecessors of each vertex in the graph
 */
template <typename GraphType, typename WeightType>
std::pair<std::vector<WeightType>, std::vector<Vertex<GraphType>>>
dijkstra(const GraphType &g, Vertex<GraphType> source,
         const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<WeightType> distances(boost::num_vertices(g));
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
 * @param weight_map (optional) A map from edges to their weights.
 * @return A vector containing the distances from the source vertex to each
 vertex in the graph.
 */
template <typename GraphType, typename WeightType>
std::vector<WeightType>
dijkstra_distances(const GraphType &g, Vertex<GraphType> source,
                   const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<WeightType> distances(boost::num_vertices(g));

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
 * @param weight_map (optional) A map from edges to their weights. If omitted,
 * falls back to BFS.
 * @return A vector containing the predecessors of each vertex in the graph.
 */
template <typename GraphType, typename WeightType = std::size_t>
std::vector<Vertex<GraphType>>
dijkstra_predecessors(const GraphType &g, Vertex<GraphType> source,
                      const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::dijkstra_shortest_paths(
      g, source,
      boost::predecessor_map(predecessors.data())
          .weight_map(boost::make_assoc_property_map(weight_map)));

  return predecessors;
}

/**
 * @brief Run Dijkstra from a source vertex to all other vertices in the
 * graph,recording *every* predecessor that results in a shortest path, not just
 * one of them.
 * @param g The graph to search
 * @param source The source vertex
 * @param weight_map A map from edges to their weights.
 * @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 * predecessors of each vertex in the graph.
 *
 * This implementation is based on the this stackoverflow answer:
 * https://stackoverflow.com/questions/77374603/find-multiple-all-shortest-paths-between-a-pair-of-vertices-using-boosts-dijk#answer-77375620
 */
template <typename GraphType, typename WeightType>
std::pair<std::vector<WeightType>, std::vector<VertexSet<GraphType>>>
dijkstra_all_distances_all_predecessors(
    const GraphType &g, Vertex<GraphType> source,
    const EdgeMap<WeightType, GraphType> &weight_map) {

  // In this case, we use an unordered_set to store the predecessors of each
  // vertex. To make this work, we need to define a custom dijkstra visitor.

  // --- Dijkstra State ---
  struct state {
    const GraphType &graph;
    const EdgeMap<WeightType, GraphType> &weights;
    std::vector<WeightType> distances;
    std::vector<VertexSet<GraphType>> predecessors;

    state(const GraphType &graph, const EdgeMap<WeightType, GraphType> &weights)
        : graph{graph}, weights{weights}, distances(boost::num_vertices(graph)),
          predecessors(boost::num_vertices(graph)) {}
  };
  // ----------------------

  // --- Custom Visitor ---
  struct vis_t : boost::default_dijkstra_visitor {

    state &s;
    vis_t(state &s) : s{s} {}

    void edge_relaxed(Edge<GraphType> e, const GraphType &graph) const {
      s.predecessors[boost::target(e, graph)] = {boost::source(e, graph)};
    }

    void edge_not_relaxed(Edge<GraphType> e, const GraphType &graph) const {

      auto u = boost::source(e, graph);
      auto v = boost::target(e, graph);

      WeightType old_dist = s.distances[v];
      WeightType new_dist = s.distances[u] + s.weights.at(e);

      if (std::nextafter(new_dist, old_dist) == old_dist) {
        s.predecessors[v].insert(u);
      }
    }
  };
  // ----------------------

  state s{g, weight_map};
  vis_t vis{s};

  boost::dijkstra_shortest_paths(
      g, source,
      boost::distance_map(s.distances.data())
          .weight_map(boost::make_assoc_property_map(weight_map))
          .visitor(vis));

  return {s.distances, s.predecessors};
}

/**
 * Weightless version of dijkstra_all_distances_all_predecessors
 */
template <typename GraphType>
std::pair<std::vector<std::size_t>, std::vector<VertexSet<GraphType>>>
dijkstra_all_distances_all_predecessors(const GraphType &g,
                                        Vertex<GraphType> source) {
  EdgeMap<std::size_t, GraphType> weight_map = detail::make_unit_weight_map(g);
  return dijkstra_all_distances_all_predecessors(g, source, weight_map);
}

// ============== A-Star Visitors ================

/**
 * @brief A visitor for A* that terminates when a given vertex is reached,
 *  whereupon it throws an exception.
 * @details I have on idea why boost implements early stopping with
 * exceptions. Seems like it would be slow... but have to profile to know for
 * sure.
 */
template <class GraphType>
class astar_goal_visitor : public boost::default_astar_visitor {
public:
  struct found_goal {}; // exception for terminationn

  astar_goal_visitor(Vertex<GraphType> goal) : m_goal(goal) {}

  void examine_vertex(Vertex<GraphType> u,
                      [[maybe_unused]] const GraphType &g) {
    if (u == m_goal)
      throw found_goal();
  }

private:
  Vertex<GraphType> m_goal;
};
// ================== End A-Star Visitors ====================

/**
 * @brief Run A* from a source vertex, using a
 *        given heuristic, returning distances and predecessors
 * @param g The graph to search
 * @param source The source vertex
 * @param heuristic The heuristic to use
 * @param weight_map A map from edges to their weights
 * @return A pair of vectors, the first containing the distances from the
 * source vertex to each vertex in the graph, and the second containing the
 * predecessors of each vertex in the graph
 */
template <typename GraphType, typename WeightType, typename AStarHeuristic>
std::pair<std::vector<WeightType>, std::vector<Vertex<GraphType>>>
astar(const GraphType &g, Vertex<GraphType> source,
      const AStarHeuristic &heuristic,
      const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<WeightType> distances(boost::num_vertices(g));
  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::astar_search(
      g, source, heuristic,
      boost::predecessor_map(predecessors.data())
          .distance_map(distances.data())
          .weight_map(boost::make_assoc_property_map(weight_map))
          .visitor(boost::default_astar_visitor()));

  return {distances, predecessors};
}

template <typename GraphType, typename AStarHeuristic>
std::pair<std::vector<std::size_t>, std::vector<Vertex<GraphType>>>
astar(const GraphType &g, Vertex<GraphType> source,
      const AStarHeuristic &heuristic) {

  EdgeMap<std::size_t, GraphType> weight_map = detail::make_unit_weight_map(g);
  return astar(g, source, heuristic, weight_map);
}

/**
 * @brief Version of A-star with early stopping. This version will stop once
 * when goal vertex is reached. It returns the distance to the goal vertex and
 * the predecessors of the vertices along the path to the goal vertex.
 * @param g The graph to search
 * @param source The source vertex
 * @param goal The goal vertex
 * @param heuristic The heuristic to use
 * @param weight_map A map from edges to their weights
 */
template <typename GraphType, typename WeightType, typename AStarHeuristic>
std::pair<WeightType, std::vector<Vertex<GraphType>>>
astar_early_stopping(const GraphType &g, Vertex<GraphType> source,
                     Vertex<GraphType> goal, const AStarHeuristic &heuristic,
                     const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<WeightType> distances(boost::num_vertices(g));
  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));
  astar_goal_visitor<GraphType> vis{goal};

  try {
    boost::astar_search(
        g, source, heuristic,
        boost::predecessor_map(predecessors.data())
            .distance_map(distances.data())
            .weight_map(boost::make_assoc_property_map(weight_map))
            .visitor(vis));
  } catch (typename astar_goal_visitor<GraphType>::found_goal fg) {
    // do nothing
  }

  return {distances[goal], predecessors};
}

/**
 * @brief Weightless overload of A-star with early stopping
 */
template <typename GraphType, typename AStarHeuristic>
std::pair<std::size_t, std::vector<Vertex<GraphType>>>
astar_early_stopping(const GraphType &g, Vertex<GraphType> source,
                     Vertex<GraphType> goal, const AStarHeuristic &heuristic) {

  EdgeMap<std::size_t, GraphType> weight_map = detail::make_unit_weight_map(g);
  return astar_early_stopping(g, source, goal, heuristic, weight_map);
}

/**
 * @brief Run A* from a source vertex to a target vertex, using a
 *        given heuristic, returning distances only
 * @param g The graph to search
 * @param source The source vertex
 * @param heuristic The heuristic to use
 * @param weight_map A map from edges to their weights
 * @return A vector containing the distances from the source vertex to each
 * vertex in the graph.
 */
template <typename GraphType, typename WeightType, typename AStarHeuristic>
std::vector<WeightType>
astar_distances(const GraphType &g, Vertex<GraphType> source,
                const AStarHeuristic &heuristic,
                const EdgeMap<WeightType, GraphType> &weight_map) {

  std::vector<WeightType> distances(boost::num_vertices(g));

  boost::astar_search(
      g, source, heuristic,
      boost::distance_map(distances.data())
          .weight_map(boost::make_assoc_property_map(weight_map))
          .visitor(boost::default_astar_visitor()));

  return distances;
}

template <typename GraphType, typename AStarHeuristic>
std::vector<std::size_t> astar_distances(const GraphType &g,
                                         Vertex<GraphType> source,
                                         const AStarHeuristic &heuristic) {

  EdgeMap<std::size_t, GraphType> weight_map = detail::make_unit_weight_map(g);
  return astar_distances(g, source, heuristic, weight_map);
}

/**
 * @brief Run A* from a source vertex to a target vertex, using a
 *        given heuristic, returning predecessors only
 * @param g The graph to search
 * @param source The source vertex
 * @param target The target vertex
 * @param heuristic The heuristic to use
 * @param weight_map A map from edges to their weights
 * @return A vector containing the predecessors of each vertex in the graph.
 */
template <typename GraphType, typename WeightType, typename AStarHeuristic>
std::vector<Vertex<GraphType>>
astar_predecessors(const GraphType &g, Vertex<GraphType> source,
                   const AStarHeuristic &heuristic,
                   const EdgeMap<GraphType, GraphType> &weight_map) {

  std::vector<Vertex<GraphType>> predecessors(boost::num_vertices(g));

  boost::astar_search(
      g, source, heuristic,
      boost::predecessor_map(predecessors.data())
          .weight_map(boost::make_assoc_property_map(weight_map))
          .visitor(boost::default_astar_visitor()));

  return predecessors;
}

template <typename GraphType, typename AStarHeuristic>
std::vector<Vertex<GraphType>>
astar_predecessors(const GraphType &g, Vertex<GraphType> source,
                   const AStarHeuristic &heuristic) {

  EdgeMap<std::size_t, GraphType> weight_map = detail::make_unit_weight_map(g);
  return astar_predecessors(g, source, heuristic, weight_map);
}

//============== A-Star Heuristics ==============

namespace detail {

template <typename T>
concept Signed = std::is_signed_v<T>;

template <typename T>
concept Unsigned = std::is_unsigned_v<T>;

template <Unsigned T>
long long safe_subtract(T a, T b) {
  return static_cast<long long>(a) - static_cast<long long>(b);
}

template <Signed T>
T safe_subtract(T a, T b) {
  return a - b;
}

} // namespace detail

/**
 * @brief A Euclidean heuristic for A* search
 * @param GraphType The type of the graph
 * @param CostType The type of the cost
 * @param LocMap The type of the location map. Must be a container such that
 * loc_map[v] is a random-access container of arithmetic types, with size equal
 * to the number of dimensions.
 */
template <typename GraphType, typename LocMap>
class EuclideanHeuristic : public boost::astar_heuristic<GraphType, double> {

public:
  EuclideanHeuristic(const LocMap &loc_map, Vertex<GraphType> goal)
      : loc_map{loc_map}, goal{goal}, dim{loc_map[goal].size()} {}

  double operator()(Vertex<GraphType> v) const {

    double result = 0.0;
    for (std::size_t i = 0; i < dim; ++i) {
      auto diff = detail::safe_subtract(loc_map.at(goal)[i], loc_map.at(v)[i]);
      result += diff * diff;
    }
    return result;
  }

private:
  LocMap loc_map;
  Vertex<GraphType> goal;
  std::size_t dim;
};

/**
 * @brief A Manhattan heuristic for A* search
 * @param GraphType The type of the graph
 * @param CostType The type of the cost
 * @param LocMap The type of the location map. Must be a container such that
 * loc_map[v] is a random-access container of arithmetic types, with size equal
 * to the number of dimensions.
 */
template <typename GraphType, typename LocMap, typename CostType>
class ManhattanHeuristic : public boost::astar_heuristic<GraphType, CostType> {

public:
  ManhattanHeuristic(const LocMap &loc_map, Vertex<GraphType> goal)
      : loc_map{loc_map}, goal{goal}, dim{loc_map.at(goal).size()} {}

  CostType operator()(Vertex<GraphType> v) const {

    CostType result = 0;
    for (std::size_t i = 0; i < dim; ++i) {
      result += std::abs(
          detail::safe_subtract(loc_map.at(goal)[i], loc_map.at(v)[i]));
    }
    return result;
  }

private:
  LocMap loc_map;
  Vertex<GraphType> goal;
  std::size_t dim;
};

} // namespace gl

} // namespace utils
