/**********************************************************************
 * @brief A bitwise adjacency matrix implementation using vector<bool>.
 * Generally "BitAdjmat" is preferred since it takes better advantage of
 * bit-packing operations such as std::countr_zero and std::popcount.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "utils_cpp/graph/graph.hpp"

#include <bit>
#include <unordered_set>
#include <vector>

namespace utils {

namespace gl {

/**
   VecBoolAdjmat implementation of a binary square matrix, which should
   be more efficient than using vector<vector<int>>. Stores 64 adjmat entries in
   a single uint64_t and uses bit operations to interact with its entries.
   Prefer using bitadjmat.hpp since it is more efficient though.
 */
class VecBoolAdjmat {

public:
  class Row {

  public:
    class iterator {
    public:
      explicit iterator(std::vector<bool>::iterator begin,
                        std::vector<bool>::iterator end) noexcept;

      explicit iterator(std::vector<bool>::iterator end) noexcept;

      iterator &operator++() noexcept;
      std::size_t operator*() const noexcept;
      bool operator==(const iterator &other) const noexcept;
      bool operator!=(const iterator &other) const noexcept;

    private:
      std::vector<bool>::iterator begin_;
      std::vector<bool>::iterator it_;
      std::vector<bool>::iterator end_;
    };

  public:
    Row(VecBoolAdjmat &parent, std::size_t row_index) noexcept;

    bool contains(std::size_t col) const noexcept;
    std::size_t size() const noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

  private:
    std::vector<bool>::iterator begin_;
    std::vector<bool>::iterator end_;
  };

  class ConstRow {
  public:
    class const_iterator {
    public:
      explicit const_iterator(std::vector<bool>::const_iterator begin,
                              std::vector<bool>::const_iterator end) noexcept;

      explicit const_iterator(std::vector<bool>::const_iterator end) noexcept;
      const_iterator &operator++() noexcept;
      std::size_t operator*() const noexcept;
      bool operator==(const const_iterator &other) const noexcept;
      bool operator!=(const const_iterator &other) const noexcept;

    private:
      std::vector<bool>::const_iterator begin_;
      std::vector<bool>::const_iterator it_;
      std::vector<bool>::const_iterator end_;
    };

  public:
    ConstRow(const VecBoolAdjmat &parent, std::size_t row_index) noexcept;

    bool contains(std::size_t col) const noexcept;
    std::size_t size() const noexcept;

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

  private:
    std::vector<bool>::const_iterator begin_;
    std::vector<bool>::const_iterator end_;
  };

public:
  VecBoolAdjmat() = default;
  VecBoolAdjmat(std::size_t n);
  VecBoolAdjmat(const Graph &g);

  Row operator[](std::size_t row_index) noexcept;
  ConstRow operator[](std::size_t row_index) const noexcept;

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
   * @return number of edges in the adjmat (half of count_ones)
   */
  std::size_t count_edges() const noexcept;

  /**
   * @return number of edges incident to a particular vertex
   */
  std::size_t count_edges(int v) const noexcept;

  /**
   * @return Number of vertices described by the adjmat.
   * adjmat
   */
  std::size_t size() const noexcept;

  /**
   * @param i row index
   * @param j column index
   * @return The (i,j)th entry of the adjmat
   */
  bool get(std::size_t i, std::size_t j) const noexcept;

  /**
   * Set the (i,j)th entry of the adjmat to val
   * @param i row index
   * @param j column index
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
  VecBoolAdjmat &swap(std::size_t v1, std::size_t v2);

  /**
   * Swap all vertices in a matching, inplace.
   * @param matching vector of pairs of vertices to swap
   * @return reference to this adjmat
   */
  VecBoolAdjmat &
  swap(const std::vector<std::pair<std::size_t, std::size_t>> &matching);

  /**
   * Toggle all the entries in the adjmat 0 -> 1 and 1 -> 0.
   * @return reference to this adjmat
   */
  VecBoolAdjmat &flip() noexcept;

  /**
   * Perform a bitwise xor of this adjmat with another adjmat.
   * @param other adjmat to XOR with
   * @return reference to this adjmat
   */
  VecBoolAdjmat &operator^=(const VecBoolAdjmat &other) noexcept;

  /**
   * Perform a bitwise and of this adjmat with another adjmat.
   * @param other adjmat to AND with
   * @return reference to this adjmat
   */
  VecBoolAdjmat &operator&=(const VecBoolAdjmat &other) noexcept;

  /**
   * Perform a bitwise or of this adjmat with another adjmat.
   * @param other adjmat to OR with
   * @return reference to this adjmat
   */
  VecBoolAdjmat &operator|=(const VecBoolAdjmat &other) noexcept;

  friend std::ostream &operator<<(std::ostream &os,
                                  const VecBoolAdjmat &m) noexcept;

  friend bool operator==(const VecBoolAdjmat &lhs,
                         const VecBoolAdjmat &rhs) noexcept;

  friend bool operator!=(const VecBoolAdjmat &lhs,
                         const VecBoolAdjmat &rhs) noexcept;

private:
  VecBoolAdjmat &swap_rows(std::size_t r1, std::size_t r2);
  VecBoolAdjmat &swap_columns(std::size_t c1, std::size_t c2);

private:
  std::size_t num_vertices;
  std::vector<bool> matrix;

  constexpr static std::size_t N = 64;

private:
  std::size_t flat_index(std::size_t i, std::size_t j) const noexcept;
};

// ==========================================
// =========== Implementation ===============
// ==========================================

// =========== VecBoolAdjmat::Row::iterator ===========
inline VecBoolAdjmat::Row::iterator::iterator(
    std::vector<bool>::iterator begin, std::vector<bool>::iterator end) noexcept
    : begin_{begin}, it_{begin}, end_{end} {
  if (it_ != end_ && !*it_) {
    ++(*this);
  }
}

inline VecBoolAdjmat::Row::iterator::iterator(
    std::vector<bool>::iterator end) noexcept
    : begin_{end}, it_{end}, end_{end} {}

inline VecBoolAdjmat::Row::iterator &
VecBoolAdjmat::Row::iterator::operator++() noexcept {
  do {
    ++it_;
  } while (it_ != end_ && !*it_);
  return *this;
}

inline std::size_t VecBoolAdjmat::Row::iterator::operator*() const noexcept {
  return it_ - begin_;
}

inline bool
VecBoolAdjmat::Row::iterator::operator==(const iterator &other) const noexcept {
  return it_ == other.it_;
}

inline bool
VecBoolAdjmat::Row::iterator::operator!=(const iterator &other) const noexcept {
  return it_ != other.it_;
}

// =========== VecBoolAdjmat::Row ===========
inline VecBoolAdjmat::Row::Row(VecBoolAdjmat &parent,
                               std::size_t row_index) noexcept
    : begin_{parent.matrix.begin() + row_index * parent.num_vertices},
      end_{begin_ + parent.num_vertices} {}

inline bool
VecBoolAdjmat::Row::contains(std::size_t neighbor_index) const noexcept {
  return *(begin_ + neighbor_index);
}

inline std::size_t VecBoolAdjmat::Row::size() const noexcept {
  return end_ - begin_;
}

inline VecBoolAdjmat::Row::iterator VecBoolAdjmat::Row::begin() const noexcept {
  return iterator(begin_, end_);
}

inline VecBoolAdjmat::Row::iterator VecBoolAdjmat::Row::end() const noexcept {
  return iterator(end_, end_);
}

// =========== VecBoolAdjmat::Row::const_iterator ===========
inline VecBoolAdjmat::ConstRow::const_iterator::const_iterator(
    std::vector<bool>::const_iterator begin,
    std::vector<bool>::const_iterator end) noexcept
    : begin_{begin}, it_{begin}, end_{end} {
  if (it_ != end_ && !*it_) {
    ++(*this);
  }
}

inline VecBoolAdjmat::ConstRow::const_iterator::const_iterator(
    std::vector<bool>::const_iterator end) noexcept
    : begin_{end}, it_{end}, end_{end} {}

inline VecBoolAdjmat::ConstRow::const_iterator &
VecBoolAdjmat::ConstRow::const_iterator::operator++() noexcept {
  do {
    ++it_;
  } while (it_ != end_ && !*it_);
  return *this;
}

inline std::size_t
VecBoolAdjmat::ConstRow::const_iterator::operator*() const noexcept {
  return it_ - begin_;
}

inline bool VecBoolAdjmat::ConstRow::const_iterator::operator==(
    const const_iterator &other) const noexcept {
  return it_ == other.it_;
}

inline bool VecBoolAdjmat::ConstRow::const_iterator::operator!=(
    const const_iterator &other) const noexcept {
  return it_ != other.it_;
}

// =========== VecBoolAdjmat::ConstRow ===========

inline VecBoolAdjmat::ConstRow::ConstRow(const VecBoolAdjmat &parent,
                                         std::size_t row_index) noexcept
    : begin_{parent.matrix.begin() + row_index * parent.num_vertices},
      end_{begin_ + parent.num_vertices} {}

inline bool
VecBoolAdjmat::ConstRow::contains(std::size_t neighbor_index) const noexcept {
  return *(begin_ + neighbor_index);
}

inline std::size_t VecBoolAdjmat::ConstRow::size() const noexcept {
  return end_ - begin_;
}

inline VecBoolAdjmat::ConstRow::const_iterator
VecBoolAdjmat::ConstRow::begin() const noexcept {
  return const_iterator(begin_, end_);
}

inline VecBoolAdjmat::ConstRow::const_iterator
VecBoolAdjmat::ConstRow::end() const noexcept {
  return const_iterator(end_, end_);
}

// =========== VecBoolAdjmat ===========

inline VecBoolAdjmat::VecBoolAdjmat(std::size_t num_vertices)
    : num_vertices{num_vertices},
      matrix{std::vector<bool>(num_vertices * num_vertices, 0)} {}

inline VecBoolAdjmat::VecBoolAdjmat(const Graph &g)
    : num_vertices{boost::num_vertices(g)},
      matrix{std::vector<bool>(num_vertices * num_vertices, 0)} {

  for (auto [ei, end] = boost::edges(g); ei != end; ++ei) {
    uint64_t i = boost::source(*ei, g);
    uint64_t j = boost::target(*ei, g);
    matrix[flat_index(i, j)] = 1;
    matrix[flat_index(j, i)] = 1;
  }
}

inline VecBoolAdjmat::Row
VecBoolAdjmat::operator[](std::size_t row_index) noexcept {
  return Row(*this, row_index);
}

inline VecBoolAdjmat::ConstRow
VecBoolAdjmat::operator[](std::size_t row_index) const noexcept {
  return ConstRow(*this, row_index);
}

inline Graph VecBoolAdjmat::to_graph() const {
  Graph g;
  for (std::size_t i = 0; i < num_vertices; ++i) {
    boost::add_vertex(g);
  }

  for (std::size_t i = 0; i < num_vertices; ++i) {
    for (std::size_t j = i + 1; j < num_vertices; ++j) {
      if (matrix[flat_index(i, j)]) {
        boost::add_edge(i, j, g);
      }
    }
  }
  return g;
}

inline std::size_t VecBoolAdjmat::count_ones() const noexcept {
  std::size_t count = 0;
  for (auto element : matrix) {
    count += element;
  }
  return count;
}

inline std::size_t VecBoolAdjmat::count_edges() const noexcept {
  return count_ones() / 2;
}

inline std::size_t VecBoolAdjmat::count_edges(int v) const noexcept {
  std::size_t count = 0;
  for (auto element : this->operator[](v)) {
    count += element;
  }
  return count;
}

inline std::size_t VecBoolAdjmat::size() const noexcept { return num_vertices; }

inline VecBoolAdjmat &VecBoolAdjmat::swap(std::size_t v1, std::size_t v2) {
  swap_rows(v1, v2);
  swap_columns(v1, v2);
  return *this;
}

inline VecBoolAdjmat &VecBoolAdjmat::swap(
    const std::vector<std::pair<std::size_t, std::size_t>> &matching) {
  for (auto [v1, v2] : matching) {
    swap(v1, v2);
  }
  return *this;
}

inline VecBoolAdjmat &VecBoolAdjmat::swap_rows(std::size_t r1, std::size_t r2) {
  auto r1_begin_ = matrix.begin() + r1 * num_vertices;
  auto r1_end = matrix.begin() + (r1 + 1) * num_vertices;
  auto r2_begin_ = matrix.begin() + r2 * num_vertices;
  std::swap_ranges(r1_begin_, r1_end, r2_begin_);
  return *this;
}

inline VecBoolAdjmat &VecBoolAdjmat::swap_columns(std::size_t c1,
                                                  std::size_t c2) {
  for (std::size_t i = 0; i < num_vertices; ++i) {
    std::vector<bool>::swap(matrix[flat_index(i, c1)],
                            matrix[flat_index(i, c2)]);
  }
  return *this;
}

inline VecBoolAdjmat &VecBoolAdjmat::flip() noexcept {
  matrix.flip();
  return *this;
}

inline VecBoolAdjmat &
VecBoolAdjmat::operator^=(const VecBoolAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices; ++i) {
    matrix[i] = matrix[i] ^ other.matrix[i];
  }
  return *this;
}

inline VecBoolAdjmat &
VecBoolAdjmat::operator&=(const VecBoolAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices; ++i) {
    matrix[i] = matrix[i] && other.matrix[i];
  }
  return *this;
}

inline VecBoolAdjmat &
VecBoolAdjmat::operator|=(const VecBoolAdjmat &other) noexcept {
  for (std::size_t i = 0; i < num_vertices; ++i) {
    matrix[i] = matrix[i] || other.matrix[i];
  }
  return *this;
}

inline bool VecBoolAdjmat::get(std::size_t i, std::size_t j) const noexcept {
  return matrix[flat_index(i, j)];
}

inline void VecBoolAdjmat::set(std::size_t i, std::size_t j,
                               bool val) noexcept {
  matrix[flat_index(i, j)] = val;
}

inline std::ostream &operator<<(std::ostream &os,
                                const VecBoolAdjmat &m) noexcept {
  for (std::size_t i = 0; i < m.num_vertices; ++i) {
    for (std::size_t j = 0; j < m.num_vertices; ++j) {
      if (j != 0) {
        os << ' ';
      }

      os << m.matrix[m.flat_index(i, j)];
    }
    os << "\n";
  }
  return os;
}

inline bool operator==(const VecBoolAdjmat &lhs,
                       const VecBoolAdjmat &rhs) noexcept {
  return lhs.matrix == rhs.matrix;
}

inline bool operator!=(const VecBoolAdjmat &lhs,
                       const VecBoolAdjmat &rhs) noexcept {
  return !(lhs == rhs);
}

inline std::size_t VecBoolAdjmat::flat_index(std::size_t i,
                                             std::size_t j) const noexcept {
  return i * num_vertices + j;
}

} // namespace gl

} // namespace utils
