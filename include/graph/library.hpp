/** @file
 * @brief Graph library - a collection of functions that create various graphs.
 */

#pragma once

#include "graph/graph.hpp"
#include "graph/properties.hpp"

#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace utils {
namespace gl {

/**
 *   @brief Construct basic grid graph
 *   @param h Height of grid
 *   @param w Width of grid
 *   @return A pair of the graph and its properties
 */
GraphBundle grid(std::size_t h, std::size_t w);

/**
 *    @brief Construct basic complete graph
 *    @param size Number of vertices
 *   @return A pair of the graph and its properties
 */
GraphBundle complete(std::size_t size);

/**
 * @brief Construct kuratowski graph
 * @param h Height of grid
 * @param w Width of grid
 * @return A pair of the graph and its properties
 */
GraphBundle kuratowski(std::size_t h, std::size_t w);

/**
 * @brief Construct chimera graph
 * @param h Height of grid
 * @param w Width of grid
 * @param k_cc Size of bipartite graphs
 * @return A pair of the graph and its properties
 */
GraphBundle chimera(std::size_t h, std::size_t w, std::size_t k_cc);

/**
 * @brief Construct ringgraph
 * @param size Number of vertices
 * @return A pair of the graph and its properties
 */
GraphBundle ring(std::size_t size);

/**
 * @brief Construct path graph
 * @param size Number of vertices
 * @return A pair of the graph and its properties
 */
GraphBundle path(std::size_t size);

/**
 * @brief Construct ibm_hex graph
 * @param nrows Number of rows
 * @param ncols Number of columns
 * @return A pair of the graph and its properties
 */
GraphBundle ibm_hex(std::size_t nrows, std::size_t ncols);

/**
 * @brief Construct the kagome lattice graph
 * @param nrows Number of face cell rows
 * @param ncols Number of face cell columns
 * @return A pair of the graph and its properties
 */
GraphBundle kagome(std::size_t nrows, std::size_t ncols);

/**
 * @brief Construct random graph, where density determines the probability of
 * connecting two vertices
 * @param size Number of vertices
 * @param density Probability of connecting two vertices
 * @param seed Seed for random number generator
 * @return A pair of the graph and its properties
 */
GraphBundle random(std::size_t size, double density,
                   unsigned seed = std::random_device{}());

/**
 * @brief Construct random bipartite graph, where density determines the
 * probability of connecting two vertices in opposite partitions.
 * @param size1 Number of vertices in first partition
 * @param size2 Number of vertices in second partition
 * @param density Probability of connecting two vertices in opposite partitions
 * @param seed Seed for random number generator
 * @return A pair of the graph and its properties
 */
GraphBundle random_bipartite(std::size_t size1, std::size_t size2,
                             double density,
                             unsigned seed = std::random_device{}());

/**
 * @brief Construct random regular graph, where degree determines the degree of
 * each vertex. Based heavily off the NetworkX implementation of random regular.
 * Requires that size * degree is even, and also that size < degree.
 * @param size Number of vertices
 * @param degree Degree of each vertex
 * @param seed Seed for random number generator
 * @return A pair of the graph and its properties
 */
GraphBundle random_regular(std::size_t size, std::size_t degree,
                           unsigned seed = std::random_device{}());

// ==========================================
// =========== Implementation ===============
// ==========================================

inline GraphBundle grid(std::size_t h, std::size_t w) {

  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < h; ++i) {
    for (std::size_t j = 0; j < w; ++j) {
      positions[boost::add_vertex(gb.graph)] = {static_cast<double>(j),
                                                static_cast<double>(i)};
    }
  }

  for (std::size_t i = 0; i < h * w; ++i) {
    if (i % w < w - 1) {
      boost::add_edge(i, i + 1, g);
    }
    if (i / w < h - 1) {
      boost::add_edge(i, i + w, g);
    }
  }

  gb.props.graph["name"] = "grid";
  gb.props.graph["height"] = h;
  gb.props.graph["width"] = w;
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle complete(std::size_t size) {
  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < size; ++i) {
    positions[boost::add_vertex(gb.graph)] = {
        std::cos(2.0 * M_PI * i / static_cast<double>(size)),
        std::sin(2.0 * M_PI * i / static_cast<double>(size))};
  }

  for (std::size_t i = 0; i < size; ++i) {
    for (std::size_t j = i + 1; j < size; ++j) {
      boost::add_edge(i, j, g);
    }
  }

  gb.props.graph["name"] = "complete";
  gb.props.graph["size"] = size;
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle kuratowski(std::size_t h, std::size_t w) {
  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < h * w; ++i) {

    std::size_t x = i % w;
    std::size_t y = i / w;
    positions[boost::add_vertex(gb.graph)] = {static_cast<double>(x),
                                              static_cast<double>(y)};
  }

  for (std::size_t i = 0; i < h * w; ++i) {
    std::size_t xpos = i % w;
    std::size_t ypos = i / w;
    if (xpos + 1 != w)
      boost::add_edge(i, i + 1, g); // connect right
    if (ypos % 2 == 0 && xpos + 1 != w && ypos + 1 != h)
      boost::add_edge(i, i + w + 1, g); // connect down-right
    if (ypos % 2 == 0 && xpos != 0 && ypos + 1 != h)
      boost::add_edge(i, i + w - 1, g); // connect down-left
    if (ypos % 2 == 1 && ypos + 1 != h)
      boost::add_edge(i, i + w, g); // connect down
  }

  gb.props.graph["name"] = "kuratowski";
  gb.props.graph["height"] = h;
  gb.props.graph["width"] = w;
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle chimera(std::size_t h, std::size_t w, std::size_t k_cc) {

  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < 2 * k_cc * h * w; ++i) {

    std::size_t quotient = i / (2 * w);
    std::size_t remainder = i % (2 * w);

    positions[boost::add_vertex(gb.graph)] = {static_cast<double>(remainder),
                                              h * k_cc - 1.0 - quotient};
  }

  for (std::size_t i = 0; i < 2 * k_cc * h * w; ++i) {
    std::size_t x = i % (2 * w);
    std::size_t y = i / (2 * w);

    if (x % 2 == 0) // Left column of unit cells
    {
      std::size_t col_pos = y % k_cc;
      for (std::size_t j = 0; j < k_cc; ++j) {
        boost::add_edge(i, i + 2 * w * (j - col_pos) + 1,
                        g); // intra-cell
      }

      if (y < (h - 1) * k_cc - 1) {
        boost::add_edge(i, i + 2 * k_cc * w, g); // inter-cell vertical
      }
    } else {
      if (x < 2 * w - 2) {
        boost::add_edge(i, i + 2, g); // inter-cell horizontal
      }
    }
  }

  gb.props.graph["name"] = "chimera";
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle ring(std::size_t size) {
  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < size; ++i) {

    positions[boost::add_vertex(gb.graph)] = {
        std::cos(2.0 * M_PI * i / static_cast<double>(size)),
        std::sin(2.0 * M_PI * i / static_cast<double>(size))};
  }

  for (std::size_t i = 0; i < size; ++i) {
    boost::add_edge(i, (i + 1) % size, g);
  }

  gb.props.graph["name"] = "ring";
  gb.props.graph["size"] = size;
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle path(std::size_t size) {

  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < size; ++i) {
    positions[boost::add_vertex(gb.graph)] = {static_cast<double>(i), 0.0};
  }

  for (std::size_t i = 0; i < size - 1; ++i) {
    boost::add_edge(i, (i + 1) % size, g);
  }

  gb.props.graph["name"] = "path";
  gb.props.graph["size"] = size;
  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle ibm_hex(std::size_t nrows, std::size_t ncols) {

  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;

  const std::size_t num_qubits = 5 * nrows * ncols + 4 * (nrows + ncols) - 1;

  gb.props.graph["name"] = "ibm_hex";
  gb.props.graph["num_rows"] = nrows;
  gb.props.graph["num_cols"] = ncols;

  const std::size_t longest_line = 4 * (nrows * ncols + nrows + ncols) + 1;
  const std::size_t qwidth = 4 * (ncols + 1) - 1;
  const std::size_t qheight = 2 * nrows + 1;

  std::vector<std::pair<std::size_t, std::size_t>> vertex_to_coord(num_qubits);
  std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t,
                     pair_hash<std::size_t, std::size_t>>
      coord_to_vertex;

  std::size_t qx = 1;
  std::size_t qy = 0;

  for (std::size_t i = 1; i < longest_line - 1; ++i) {
    if (qx == qwidth - 1) { // qy % 4 can only be 0, 1 or 2.
      if (qy % 4 == 2) {
        qx--;
      } else {
        qy++;
      }
    } else if (qx == 0) { // qy % 4 can only be 0, 2, or 3.
      if (qy % 4 == 0) {
        qx++;
      } else {
        qy++;
      }
    } else {
      if (qy % 4 == 0) {
        qx++;
      } else {
        qx--;
      }
    }

    vertex_to_coord[i] = {qx, qy};
    coord_to_vertex[{qx, qy}] = i;
  }

  // Fill in the first line qubit
  vertex_to_coord[0] = {2, 1};
  coord_to_vertex[{2, 1}] = 0;

  // Fill in the last line qubit
  if (nrows > 1) {
    vertex_to_coord[longest_line - 1] = {
        vertex_to_coord[longest_line - 2].first,
        vertex_to_coord[longest_line - 2].second - 1};

    coord_to_vertex[vertex_to_coord[longest_line - 1]] = longest_line - 1;
  }

  // Fill in other qubits on the line
  std::size_t i = nrows > 1 ? longest_line : longest_line - 1;

  for (qy = 0; qy < qheight - 1; ++qy) {
    if (qy % 4 == 0) {
      for (qx = 2; qx < qwidth - 1; qx += 4) {
        if (!coord_to_vertex.contains({qx, qy + 1})) {
          vertex_to_coord[i] = {qx, qy + 1};
          coord_to_vertex[{qx, qy + 1}] = i;
          ++i;
        }
      }
    } else if (qy % 4 == 2) {
      for (qx = qwidth - 3; qx > 0; qx -= 4) {
        if (!coord_to_vertex.contains({qx, qy + 1})) {
          vertex_to_coord[i] = {qx, qy + 1};
          coord_to_vertex[{qx, qy + 1}] = i;
          ++i;
        }
      }
    }
  }

  for (auto coord : vertex_to_coord) {
    positions[boost::add_vertex(gb.graph)] = {
        static_cast<double>(coord.first), static_cast<double>(coord.second)};
  }

  // Edges along line excluding first and last
  for (std::size_t i = 1; i < longest_line - 2; ++i) {
    boost::add_edge(i, i + 1, g);
  }

  // Edges for node 0
  std::size_t above_index = coord_to_vertex[{vertex_to_coord[0].first,
                                             vertex_to_coord[0].second + 1}];
  boost::add_edge(0, above_index, g);
  boost::add_edge(0, 1, g);

  // Edges for other in-between nodes
  for (std::size_t i = longest_line - 1; i < num_qubits; ++i) {
    std::size_t below_index = coord_to_vertex[{vertex_to_coord[i].first,
                                               vertex_to_coord[i].second - 1}];
    std::size_t above_index = coord_to_vertex[{vertex_to_coord[i].first,
                                               vertex_to_coord[i].second + 1}];

    boost::add_edge(above_index, i, g);
    boost::add_edge(below_index, i, g);
  }

  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle kagome(std::size_t nrows, std::size_t ncols) {

  GraphBundle gb;

  Graph &g = gb.graph;

  gb.props.graph["name"] = "kagome";
  gb.props.graph["num_rows"] = nrows;
  gb.props.graph["num_cols"] = ncols;

  const std::size_t num_grid_vertices = (nrows + 1) * (ncols + 1);
  const std::size_t num_grid_edges = (nrows + 1) * ncols + (ncols + 1) * nrows;
  const std::size_t num_vertices = num_grid_vertices + num_grid_edges;

  gb.props.graph["num_grid_vertices"] = num_grid_vertices;
  gb.props.graph["num_grid_edges"] = num_grid_edges;
  gb.props.graph["num_vertices"] = num_vertices;

  std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t,
                     pair_hash<std::size_t, std::size_t>>
      coord_to_vertex;

  // Add vertices

  VertexMap<std::vector<double>> positions;
  for (std::size_t r = 0; r < 2 * nrows + 1; ++r) {
    for (std::size_t c = 0; c < 2 * ncols + 1; ++c) {
      if (r % 2 == 1 && c % 2 == 1) {
        continue;
      }
      auto v = boost::add_vertex(gb.graph);
      positions[v] = {static_cast<double>(c), static_cast<double>(r)};
      coord_to_vertex[{c, r}] = v;
    }
  }

  // Add edges
  for (std::size_t r = 0; r < 2 * nrows + 1; ++r) {
    for (std::size_t c = 0; c < 2 * ncols + 1; ++c) {
      if (r % 2 == 0) {
        if (c > 0) {
          std::size_t source = coord_to_vertex[{c - 1, r}];
          std::size_t target = coord_to_vertex[{c, r}];
          boost::add_edge(source, target, g);
        }
      } else {
        if (c % 2 == 0) {
          std::size_t source = coord_to_vertex[{c, r}];

          std::size_t above = coord_to_vertex[{c, r + 1}];
          std::size_t below = coord_to_vertex[{c, r - 1}];

          boost::add_edge(source, above, g);
          boost::add_edge(below, source, g);

          if (c < 2 * ncols) {
            std::size_t top_right = coord_to_vertex[{c + 1, r + 1}];
            boost::add_edge(source, top_right, g);
          }
          if (c > 0) {
            std::size_t bot_left = coord_to_vertex[{c - 1, r - 1}];
            boost::add_edge(source, bot_left, g);
          }
        }
      }
    }
  }

  gb.props.graph["num_vertices"] = boost::num_vertices(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle random(std::size_t size, double density, unsigned seed) {

  std::mt19937 gen(seed);

  GraphBundle gb;

  Graph &g = gb.graph;

  gb.props.graph["name"] = "random";
  gb.props.graph["size"] = size;
  gb.props.graph["density"] = density;
  gb.props.graph["seed"] = seed;

  VertexMap<std::vector<double>> positions;
  for (std::size_t i = 0; i < size; ++i) {

    positions[boost::add_vertex(gb.graph)] = {
        std::cos(2.0 * M_PI * i / static_cast<double>(size)),
        std::sin(2.0 * M_PI * i / static_cast<double>(size))};
  }

  std::uniform_real_distribution<> dis(0, 1);
  for (std::size_t i = 0; i < size; ++i) {
    for (std::size_t j = i + 1; j < size; ++j) {
      double randreal = dis(gen);
      if (randreal < density) {
        boost::add_edge(i, j, g);
      }
    }
  }

  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline GraphBundle random_bipartite(std::size_t size1, std::size_t size2,
                                    double density, unsigned seed) {
  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;

  std::mt19937 gen(seed);

  gb.props.graph["name"] = "random_bipartite";
  gb.props.graph["size1"] = size1;
  gb.props.graph["size2"] = size2;
  gb.props.graph["density"] = density;
  gb.props.graph["seed"] = seed;

  for (std::size_t i = 0; i < size1; ++i) {
    positions[boost::add_vertex(gb.graph)] = {0, static_cast<double>(i)};
  }

  for (std::size_t i = 0; i < size2; ++i) {
    positions[boost::add_vertex(gb.graph)] = {1, static_cast<double>(i)};
  }

  std::uniform_real_distribution<double> dist(0.0, 1.0);

  for (std::size_t i = 0; i < size1; ++i) {
    for (std::size_t j = 0; j < size2; ++j) {
      if (dist(gen) < density) {
        boost::add_edge(i, size1 + j, g);
      }
    }
  }

  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

inline bool _suitable(
    const std::unordered_set<std::pair<std::size_t, std::size_t>,
                             symmetric_pair_hash<std::size_t, std::size_t>,
                             symmetric_pair_equal<std::size_t, std::size_t>>
        &edges,
    const std::unordered_map<std::size_t, std::size_t> &potential_edges) {

  if (potential_edges.empty()) {
    return true;
  }

  for (auto [k1, v1] : potential_edges) {
    for (auto [k2, v2] : potential_edges) {
      if (k1 == k2)
        break;

      bool contains_edge = (k1 > k2 && edges.contains({k2, k1})) ||
                           (k1 < k2 && edges.contains({k1, k2}));
      if (!contains_edge)
        return true;
    }
  }
  return false;
}

inline std::optional<
    std::unordered_set<std::pair<std::size_t, std::size_t>,
                       symmetric_pair_hash<std::size_t, std::size_t>,
                       symmetric_pair_equal<std::size_t, std::size_t>>>
_try_creation(std::size_t size, std::size_t degree, std::mt19937 &gen) {

  std::unordered_set<std::pair<std::size_t, std::size_t>,
                     symmetric_pair_hash<std::size_t, std::size_t>,
                     symmetric_pair_equal<std::size_t, std::size_t>>
      edges;
  std::vector<std::size_t> stubs;
  stubs.reserve(size * degree);
  for (std::size_t i = 0; i < degree; ++i) {
    for (std::size_t j = 0; j < size; ++j) {
      stubs.push_back(j);
    }
  }

  while (!stubs.empty()) {

    std::unordered_map<std::size_t, std::size_t> potential_edges;
    std::shuffle(stubs.begin(), stubs.end(), gen);

    for (std::size_t i = 0; i < static_cast<std::size_t>(stubs.size()) - 1;
         i += 2) {
      std::size_t s1 = stubs[i];
      std::size_t s2 = stubs[i + 1];

      if (s1 > s2) {
        std::swap(s1, s2);
      }

      if (s1 != s2 && !edges.contains({s1, s2})) {
        edges.insert({s1, s2});
      } else {
        potential_edges[s1]++;
        potential_edges[s2]++;
      }
    }

    if (!_suitable(edges, potential_edges)) {
      return std::nullopt;
    }

    std::vector<std::size_t> next_stubs;
    for (auto [k, v] : potential_edges) {
      for (std::size_t i = 0; i < v; ++i) {
        next_stubs.push_back(k);
      }
    }
    stubs = next_stubs;
  }
  return edges;
}

inline GraphBundle random_regular(std::size_t size, std::size_t degree,
                                  unsigned seed) {

  GraphBundle gb;

  Graph &g = gb.graph;

  VertexMap<std::vector<double>> positions;

  std::mt19937 gen(seed);

  if (size * degree % 2 != 0) {
    throw std::runtime_error("size * degree must be even");
  }

  if (degree >= size) {
    throw std::runtime_error("degree must be less than size");
  }

  gb.props.graph["name"] = "random_regular";
  gb.props.graph["size"] = size;
  gb.props.graph["degree"] = degree;
  gb.props.graph["seed"] = seed;

  auto edges = _try_creation(size, degree, gen);
  while (!edges) {
    edges = _try_creation(size, degree, gen);
  }

  for (std::size_t i = 0; i < size; ++i) {
    positions[boost::add_vertex(gb.graph)] = {std::cos(2 * M_PI * i / size),
                                              std::sin(2 * M_PI * i / size)};
  }

  for (auto [s1, s2] : *edges) {
    boost::add_edge(s1, s2, g);
  }

  gb.props.graph["num_vertices"] = boost::num_vertices(g);
  gb.props.graph["num_edges"] = boost::num_edges(g);

  gb.props.vertex["position"] = positions;

  return gb;
}

} // namespace gl

} // namespace utils
