/**********************************************************************
 * @brief A bitwise adjacency matrix representing an undirected graph.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "matrix.hpp"

#include "graph/graph.hpp"

#include <array>
#include <bit>
#include <iostream>
#include <vector>

namespace utils {
namespace gl {

/**
   BitAdjmat implementation of a binary square matrix, which should
   be more efficient than using vector<vector<int>>. Stores 64 adjmat entries in
   a single uint64_t and uses bit operations to interact with its entries.
 */
class BitAdjmat {

public:
  // =========== Edge Iteration ==========
  // For iterating through all edges of the graph.
  // Usage: for(auto [ei, end] = g.edges(); ei != end; ++ei) { ... }
  // =====================================
  class edge_iterator {

    using edge_pair = std::pair<std::size_t, std::size_t>;
    using iterator = std::vector<uint64_t>::const_iterator;

  public:
    edge_iterator(std::size_t num_uint64_per_row, iterator finish,
                  std::size_t index_of_uint64_in_row, std::size_t row_index,
                  iterator it) noexcept;

    edge_iterator &operator++() noexcept;

    edge_pair operator*() const noexcept;
    edge_pair const *operator->() const noexcept;

    bool operator==(const edge_iterator &other) const noexcept;
    bool operator!=(const edge_iterator &other) const noexcept;

  private:
    std::size_t num_uint64_per_row;
    iterator finish;

    std::size_t index_of_uint64_in_row;
    std::size_t row_index;
    iterator it;
    uint64_t uint64_copy;

    edge_pair current_edge;
  };

  std::pair<edge_iterator, edge_iterator> edges() const noexcept;

  // =========== Edge Range ==========
  // For iterating through all edges of the graph.
  // Usage: for(auto edge : g.edge_range()) { ... }
  // =================================

  class edge_range_proxy {
  public:
    edge_range_proxy(const BitAdjmat &mat) noexcept;

    edge_iterator begin() const noexcept;
    edge_iterator end() const noexcept;

  private:
    const BitAdjmat &mat;
  };

  edge_range_proxy edge_range() const noexcept;

  // =========== Row Iteration ===========
  // For iterating through all the neighbors of a vertex.
  // =====================================
  class Row {
    using iterator = std::vector<uint64_t>::const_iterator;

  public:
    /**
     * Read-only iterator
     */
    class row_iterator {

    public:
      row_iterator(iterator it, iterator end) noexcept;

      row_iterator &operator++() noexcept;
      std::size_t operator*() const noexcept;
      bool operator==(const row_iterator &other) const noexcept;
      bool operator!=(const row_iterator &other) const noexcept;

    private:
      iterator it_;
      iterator end_;

      uint64_t current_;

      std::size_t cumulative_ = 0;
    };

  public:
    Row(const BitAdjmat &mat, std::size_t row) noexcept;

    bool contains(std::size_t nb_index) const noexcept;

    std::size_t size() const noexcept;

    row_iterator begin() const noexcept;
    row_iterator end() const noexcept;

  private:
    iterator start_;
    std::size_t num_vertices_;
  };

  Row operator[](std::size_t row_index) noexcept;
  const Row operator[](std::size_t row_index) const noexcept;

public:
  // ==========================================
  // =========== Basic functionality ==========
  // ==========================================

  BitAdjmat() = default;
  BitAdjmat(std::size_t n);
  BitAdjmat(const Graph &g);

  /**
   * Returns a Graph that corresponds to the current adjacency matrix
   * representation.
   * @return Graph
   */
  Graph to_graph() const;

  /**
   * @return number of 1s in the adjmat.
   */
  std::size_t count_ones() const noexcept;

  /**
   * @return Number of vertices described by the adjmat.
   * adjmat
   */
  std::size_t num_vertices() const noexcept;

  /**
   * @return number of edges in the adjmat (half of count_ones)
   */
  std::size_t num_edges() const noexcept;

  /**
   * @return number of edges incident to a particular vertex
   */
  std::size_t degree(int v) const noexcept;

  /**
   * Get the value of the (i,j)th entry of the adjmat.
   * @param i row index
   * @param j column index
   * @return The (i,j)th entry of the adjmat
   */
  bool get(std::size_t i, std::size_t j) const noexcept;

  /**
   * Sets both the (i,j) and (j,i) entries of the adjmat to 1.
   * @param i row index
   * @param j column index
   */
  void set(std::size_t i, std::size_t j) noexcept;

  /**
   * Clears both the (i,j) and (j,i) entries of the adjmat.
   * @param i row index
   * @param j column index
   */
  void reset(std::size_t i, std::size_t j) noexcept;

  /**
   * Sets both the (i,j) and (j,i) entries of the adjmat to val
   * @param i row index
   * @param jp column index
   * @param val value to set
   */
  void set(std::size_t i, std::size_t j, bool val) noexcept;

  /**
   * Modify the adjmat inplace to reflect the swapping of two vertices in the
   * vertex ordering of the adjmat. This is is equivalent to swapping the rows
   * and columns of the adjmat.
   * @param v1 first vertex to swap
   * @param v2 second vertex to swap
   * @return reference to this adjmat
   */
  BitAdjmat &swap(std::size_t v1, std::size_t v2);

  /**
   * Swap all vertices in a matching, inplace.
   * @param matching vector of pairs of vertices to swap
   * @return reference to this adjmat
   */
  BitAdjmat &
  swap(const std::vector<std::pair<std::size_t, std::size_t>> &matching);

  /**
   * Swap all vertices in a matching, inplace.
   * @param matching vector of pairs of vertices to swap
   * @return reference to this adjmat
   */
  BitAdjmat &swap(const std::vector<std::array<std::size_t, 2>> &matching);

  /**
   * Swap all vertices in a matching, inplace.
   * @param matching vector of pairs of vertices to swap
   * @return reference to this adjmat
   */
  BitAdjmat &swap(const std::vector<std::vector<std::size_t>> &matching);

  /**
   * Toggle all the entries in the adjmat 0 -> 1 and 1 -> 0.
   * @return reference to this adjmat
   */
  BitAdjmat &toggle() noexcept;

  /**
   * Return a bitwise-negated copy.
   */
  BitAdjmat operator~() const noexcept;

  BitAdjmat &operator^=(const BitAdjmat &other) noexcept;
  BitAdjmat &operator&=(const BitAdjmat &other) noexcept;
  BitAdjmat &operator|=(const BitAdjmat &other) noexcept;

  friend BitAdjmat operator&(const BitAdjmat &x, const BitAdjmat &y) noexcept;
  friend BitAdjmat operator|(const BitAdjmat &x, const BitAdjmat &y) noexcept;
  friend BitAdjmat operator^(const BitAdjmat &x, const BitAdjmat &y) noexcept;

  /**
   * Print a row of the adjmat to an output stream.
   */
  std::ostream &print_row(int row, std::ostream &os = std::cout) const noexcept;

  friend std::ostream &operator<<(std::ostream &os,
                                  const BitAdjmat &m) noexcept;

  friend bool operator==(const BitAdjmat &lhs, const BitAdjmat &rhs) noexcept;

  friend bool operator!=(const BitAdjmat &lhs, const BitAdjmat &rhs) noexcept;

private:
  BitAdjmat &swap_rows(std::size_t r1, std::size_t r2);
  BitAdjmat &swap_columns(std::size_t c1, std::size_t c2);

public:
  std::size_t num_vertices_;
  std::size_t num_uint64_per_row;
  Matrix<uint64_t> matrix;

  constexpr static std::size_t N = 64;
};

// ==========================================
// =========== Implementation ===============
// ==========================================

// =========== BitAdjmat::edge_iterator ===========

inline BitAdjmat::edge_iterator::edge_iterator(
    std::size_t num_uint64_per_row,
    std::vector<uint64_t>::const_iterator finish,
    std::size_t index_of_uint64_in_row, std::size_t row_index,
    std::vector<uint64_t>::const_iterator it) noexcept
    : num_uint64_per_row{num_uint64_per_row}, finish{finish},
      index_of_uint64_in_row{index_of_uint64_in_row}, row_index{row_index},
      it{it}, uint64_copy{*it} {

  if (uint64_copy == 0) {
    ++*this;
  } else {
    current_edge = {row_index,
                    index_of_uint64_in_row * N + std::countr_zero(uint64_copy)};
  };
}

inline BitAdjmat::edge_iterator &
BitAdjmat::edge_iterator::operator++() noexcept {

  uint64_copy &= ~(1ULL << std::countr_zero(uint64_copy));

  while (uint64_copy == 0 && it != finish) {

    ++index_of_uint64_in_row;

    // end of current row
    if (index_of_uint64_in_row == num_uint64_per_row) {

      // end of iteration
      if (it == finish - 1) {
        ++it;
        return *this;

      } else {

        // continue to next row
        ++row_index;
        it += row_index / N + 1;
        current_edge.first = row_index;
        index_of_uint64_in_row = row_index / N;
        uint64_copy = (*it) & ~((1ULL << (row_index % N)) - 1);
      }

    } else {
      ++it;
      uint64_copy = *it;
    }
  }

  current_edge.second =
      index_of_uint64_in_row * N + std::countr_zero(uint64_copy);

  return *this;
}

inline std::pair<std::size_t, std::size_t>
BitAdjmat::edge_iterator::operator*() const noexcept {
  return current_edge;
}

inline std::pair<std::size_t, std::size_t> const *
BitAdjmat::edge_iterator::operator->() const noexcept {
  return &current_edge;
}

inline bool BitAdjmat::edge_iterator::operator==(
    const BitAdjmat::edge_iterator &other) const noexcept {
  return it == other.it;
}

inline bool BitAdjmat::edge_iterator::operator!=(
    const BitAdjmat::edge_iterator &other) const noexcept {
  return it != other.it;
}

inline std::pair<BitAdjmat::edge_iterator, BitAdjmat::edge_iterator>
BitAdjmat::edges() const noexcept {
  return {edge_iterator(num_uint64_per_row, matrix.data(num_vertices_), 0, 0,
                        matrix.data()),
          edge_iterator(num_uint64_per_row, matrix.data(num_vertices_), 0,
                        num_vertices_, matrix.data(num_vertices_))};
}

inline BitAdjmat::edge_range_proxy::edge_range_proxy(
    const BitAdjmat &mat) noexcept
    : mat{mat} {}

inline BitAdjmat::edge_range_proxy BitAdjmat::edge_range() const noexcept {
  return edge_range_proxy{*this};
}

inline BitAdjmat::edge_iterator
BitAdjmat::edge_range_proxy::begin() const noexcept {
  return edge_iterator(mat.num_uint64_per_row,
                       mat.matrix.data(mat.num_vertices_), 0, 0,
                       mat.matrix.data());
}

inline BitAdjmat::edge_iterator
BitAdjmat::edge_range_proxy::end() const noexcept {
  return edge_iterator(mat.num_uint64_per_row,
                       mat.matrix.data(mat.num_vertices_), 0, mat.num_vertices_,
                       mat.matrix.data(mat.num_vertices_));
}

// =========== BitAdjmat::Row::const_iterator ===========
inline BitAdjmat::Row::row_iterator::row_iterator(
    std::vector<uint64_t>::const_iterator it,
    std::vector<uint64_t>::const_iterator end) noexcept
    : it_{it}, end_{end}, current_{*it_} {
  while (current_ == 0 && it_ != end_) {
    ++it_;
    ++cumulative_;
    current_ = *it_;
  }
}

inline BitAdjmat::Row::row_iterator &
BitAdjmat::Row::row_iterator::operator++() noexcept {
  current_ &= ~(1ULL << std::countr_zero(current_));

  while (current_ == 0 && it_ != end_) {
    ++it_;
    ++cumulative_;
    current_ = *it_;
  }
  return *this;
}

inline std::size_t BitAdjmat::Row::row_iterator::operator*() const noexcept {
  return std::countr_zero(current_) + N * cumulative_;
}

inline bool BitAdjmat::Row::row_iterator::operator==(
    const row_iterator &other) const noexcept {
  return it_ == other.it_;
}

inline bool BitAdjmat::Row::row_iterator::operator!=(
    const row_iterator &other) const noexcept {
  return it_ != other.it_;
}

// =========== BitAdjmat::Row ===========

inline BitAdjmat::Row::Row(const BitAdjmat &parent,
                           std::size_t row_index) noexcept
    : start_{parent.matrix.data(row_index)},
      num_vertices_{parent.num_vertices_} {}

inline bool BitAdjmat::Row::contains(std::size_t nb_index) const noexcept {
  return ((*(start_ + nb_index / N)) >> (nb_index % N)) & 1ULL;
}

inline std::size_t BitAdjmat::Row::size() const noexcept {
  return num_vertices_;
}

inline BitAdjmat::Row::row_iterator BitAdjmat::Row::begin() const noexcept {
  return row_iterator(start_, start_ + (num_vertices_ + N - 1) / N);
}

inline BitAdjmat::Row::row_iterator BitAdjmat::Row::end() const noexcept {
  std::size_t range = (num_vertices_ + N - 1) / N;
  return row_iterator(start_ + range, start_ + range);
}

// =========== BitAdjmat ===========

inline BitAdjmat::BitAdjmat(std::size_t num_vertices_)
    : num_vertices_{num_vertices_},
      num_uint64_per_row{num_vertices_ / N + (num_vertices_ % N != 0)},
      matrix{num_vertices_, num_uint64_per_row, 0} {}

inline BitAdjmat::BitAdjmat(const Graph &g)
    : num_vertices_{boost::num_vertices(g)} {
  num_uint64_per_row = num_vertices_ / N + (num_vertices_ % N != 0);
  matrix = Matrix<uint64_t>(num_vertices_, num_uint64_per_row, 0);

  for (auto [ei, end] = boost::edges(g); ei != end; ++ei) {
    uint64_t i = boost::source(*ei, g);
    uint64_t j = boost::target(*ei, g);
    this->set(i, j);
    this->set(j, i);
  }
}

inline BitAdjmat::Row BitAdjmat::operator[](std::size_t row_index) noexcept {
  return Row(*this, row_index);
}

inline const BitAdjmat::Row
BitAdjmat::operator[](std::size_t row_index) const noexcept {
  return Row(*this, row_index);
}

inline Graph BitAdjmat::to_graph() const {
  Graph g;
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    boost::add_vertex(g);
  }

  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = i + 1; j < num_vertices_; ++j) {
      if (get(i, j)) {
        boost::add_edge(i, j, g);
      }
    }
  }
  return g;
}

inline std::size_t BitAdjmat::count_ones() const noexcept {
  std::size_t count = 0;
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
      count += std::popcount(matrix(i, j));
    }
  }
  return count;
}

inline std::size_t BitAdjmat::num_edges() const noexcept {
  return count_ones() / 2;
}

inline std::size_t BitAdjmat::degree(int v) const noexcept {
  std::size_t count = 0;
  for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
    count += std::popcount(matrix(v, j));
  }
  return count;
}

inline std::size_t BitAdjmat::num_vertices() const noexcept {
  return num_vertices_;
}

inline BitAdjmat &BitAdjmat::swap(std::size_t v1, std::size_t v2) {
  this->swap_rows(v1, v2);
  this->swap_columns(v1, v2);
  return *this;
}

inline BitAdjmat &BitAdjmat::swap(
    const std::vector<std::pair<std::size_t, std::size_t>> &matching) {
  for (const auto &[v1, v2] : matching) {
    this->swap(v1, v2);
  }
  return *this;
}

inline BitAdjmat &
BitAdjmat::swap(const std::vector<std::array<std::size_t, 2>> &matching) {
  for (const auto &e : matching) {
    this->swap(e[0], e[1]);
  }
  return *this;
}

inline BitAdjmat &
BitAdjmat::swap(const std::vector<std::vector<std::size_t>> &matching) {
  for (const auto &e : matching) {
    this->swap(e[0], e[1]);
  }
  return *this;
}

inline BitAdjmat &BitAdjmat::swap_rows(std::size_t r1, std::size_t r2) {
  auto r1_start_ = matrix[r1].begin();
  auto r1_end = matrix[r1].end();
  auto r2_start_ = matrix[r2].begin();

  std::swap_ranges(r1_start_, r1_end, r2_start_);
  return *this;
}

inline BitAdjmat &BitAdjmat::swap_columns(std::size_t c1, std::size_t c2) {
  int d1 = c1 / N, r1 = c1 % N;
  int d2 = c2 / N, r2 = c2 % N;
  for (unsigned i = 0; i < num_vertices_; ++i) {
    uint64_t b1 = (matrix(i, d1) >> r1) & 1ULL;
    uint64_t b2 = (matrix(i, d2) >> r2) & 1ULL;

    matrix(i, d1) = (matrix(i, d1) & ~(1ULL << r1)) | (b2 << r1);
    matrix(i, d2) = (matrix(i, d2) & ~(1ULL << r2)) | (b1 << r2);
  }
  return *this;
}

inline BitAdjmat &BitAdjmat::toggle() noexcept {
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
      matrix(i, j) = ~matrix(i, j);
    }
  }
  return *this;
}

inline BitAdjmat BitAdjmat::operator~() const noexcept {
  BitAdjmat result(*this);
  result.toggle();
  return result;
}

inline BitAdjmat &BitAdjmat::operator^=(const BitAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
      matrix(i, j) ^= other.matrix(i, j);
    }
  }
  return *this;
}

inline BitAdjmat &BitAdjmat::operator&=(const BitAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
      matrix(i, j) &= other.matrix(i, j);
    }
  }
  return *this;
}

inline BitAdjmat &BitAdjmat::operator|=(const BitAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices_; ++i) {
    for (std::size_t j = 0; j < num_uint64_per_row; ++j) {
      matrix(i, j) |= other.matrix(i, j);
    }
  }
  return *this;
}

inline bool BitAdjmat::get(std::size_t i, std::size_t j) const noexcept {
  return (matrix(i, j / N) >> (j % N)) & 1ULL;
}

inline void BitAdjmat::set(std::size_t i, std::size_t j) noexcept {
  matrix(i, j / N) |= (1ULL << (j % N));
  matrix(j, i / N) |= (1ULL << (i % N));
}

inline void BitAdjmat::reset(std::size_t i, std::size_t j) noexcept {
  matrix(i, j / N) &= ~(1ULL << (j % N));
  matrix(j, i / N) &= ~(1ULL << (i % N));
}

inline void BitAdjmat::set(std::size_t i, std::size_t j, bool val) noexcept {
  matrix(i, j / N) = (matrix(i, j / N) & ~(1ULL << (j % N))) |
                     (static_cast<uint64_t>(val) << (j % N));
  matrix(j, i / N) = (matrix(j, i / N) & ~(1ULL << (i % N))) |
                     (static_cast<uint64_t>(val) << (i % N));
}

inline BitAdjmat operator&(const BitAdjmat &x, const BitAdjmat &y) noexcept {
  BitAdjmat result{x};
  result &= y;
  return result;
}

inline BitAdjmat operator|(const BitAdjmat &x, const BitAdjmat &y) noexcept {
  BitAdjmat result{x};
  result |= y;
  return result;
}

inline BitAdjmat operator^(const BitAdjmat &x, const BitAdjmat &y) noexcept {
  BitAdjmat result{x};
  result ^= y;
  return result;
}

inline std::ostream &BitAdjmat::print_row(int row,
                                          std::ostream &os) const noexcept {
  int d = row / N;

  for (std::size_t i = 0; i < num_vertices_; ++i) {
    if (i != 0) {
      os << ' ';
    }
    if ((matrix(row, d) >> i) & 1ULL) {
      os << 1;
    } else {
      os << 0;
    }
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const BitAdjmat &m) noexcept {
  for (std::size_t i = 0; i < m.num_vertices_; ++i) {
    m.print_row(i, os);
    os << '\n';
  }
  return os;
}

inline bool operator==(const BitAdjmat &lhs, const BitAdjmat &rhs) noexcept {
  return lhs.matrix == rhs.matrix;
}

inline bool operator!=(const BitAdjmat &lhs, const BitAdjmat &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace gl

} // namespace utils
