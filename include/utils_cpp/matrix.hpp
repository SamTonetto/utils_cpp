#pragma once

#include <iostream>
#include <vector>

namespace utils {

/**
 * @brief Just a simple Matrix class that uses a 1D vector container so that
 * data in the matrix is completely contiguous. The elements are stored in
 * "row-major" order, so that all the elements of a single row are next to each
 * other in memory.
 */
template <typename T>
class Matrix {
public:
  /**
   * Helper class that allows for operator[] access to rows.
   */
  class Row {

  public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    friend class Matrix;

    constexpr Row(Matrix &parent, std::size_t row) noexcept;
    constexpr T &operator[](std::size_t col) noexcept;
    constexpr const T &operator[](std::size_t col) const noexcept;

    constexpr std::size_t size() const noexcept;

    constexpr iterator begin() noexcept;
    constexpr iterator end() noexcept;
    constexpr const_iterator begin() const noexcept;
    constexpr const_iterator end() const noexcept;

  private:
    iterator start;
    iterator finish;
  };

  constexpr Row operator[](std::size_t row) noexcept;
  constexpr const Row operator[](std::size_t row) const noexcept;

public:
  struct iterator {
  public:
    constexpr iterator(Matrix &parent, std::size_t row_index) noexcept;
    constexpr Row operator*() noexcept;
    constexpr iterator &operator++() noexcept;

    constexpr bool operator==(const iterator &other) const noexcept;
    constexpr bool operator!=(const iterator &other) const noexcept;

  private:
    Matrix &parent;
    std::size_t row_index;
  };

  struct const_iterator {
  public:
    constexpr const_iterator(const Matrix &parent,
                             std::size_t row_index) noexcept;
    constexpr const Row operator*() const noexcept;
    constexpr const_iterator &operator++() noexcept;

    constexpr bool operator==(const const_iterator &other) const noexcept;
    constexpr bool operator!=(const const_iterator &other) const noexcept;

  private:
    const Matrix &parent;
    std::size_t row_index;
  };

  constexpr iterator begin() noexcept;
  constexpr iterator end() noexcept;
  constexpr const_iterator begin() const noexcept;
  constexpr const_iterator end() const noexcept;

public:
  Matrix() = default;

  constexpr Matrix(std::size_t num_rows, std::size_t num_columns,
                   const T &initial_value = T());

  /**
   * Resizes the matrix to have the given number of rows and columns.
   * If this involves increasing the size of the matrix, the new elements
   * are initialized to the `initial_value` parameter.
   * If this involves decreasing the size of the matrix, the elements
   * that are removed are lost.
   * @param num_rows The new number of rows
   * @param num_columns The new number of columns
   * @param initial_value The value to initialize new elements to.
   * @return void
   */
  constexpr void resize(std::size_t num_rows, std::size_t num_columns,
                        const T &initial_value = T());

  /**
   * Reshapes the matrix to have the given number of rows and columns.
   * The total number of elements in the matrix must remain the same.
   * @param num_rows The new number of rows
   * @param num_columns The new number of columns
   * @return void
   */
  constexpr void reshape(std::size_t num_rows, std::size_t num_columns);

  constexpr T &operator()(std::size_t row, std::size_t col) noexcept;

  constexpr const T &operator()(std::size_t row,
                                std::size_t col) const noexcept;

  /**
   * Returns the number of rows and columns in the matrix.
   * @return A pair containing the number of rows and columns in the matrix.
   */
  constexpr std::pair<std::size_t, std::size_t> shape() const noexcept;

  constexpr std::size_t size() const noexcept;

  /**
   * Returns an iterator pointing to the beginning of the underlying vector.
   */
  constexpr typename std::vector<T>::const_iterator data() const noexcept;

  /**
   * Iterator to beginning of row corresponding to given row index.
   */
  constexpr typename std::vector<T>::const_iterator
  data(std::size_t row_index) const noexcept;

  /**
   * Writes a representation of the matrix to the given output stream.
   * @param os The output stream to write to.
   * @return The output stream.
   */
  std::ostream &repr(std::ostream &os) const;

  /**
   * Writes the matrix to the given output stream, in 2D form.
   * @param os The output stream to write to.
   * @param m The matrix to write.
   * @return The output stream.
   */
  template <typename U>
  friend constexpr std::ostream &operator<<(std::ostream &os,
                                            const Matrix<U> &m) noexcept;

  template <typename U, typename V>
  friend constexpr bool operator==(const Matrix<U> &lhs,
                                   const Matrix<V> &rhs) noexcept;

  template <typename U, typename V>
  friend constexpr bool operator!=(const Matrix<U> &lhs,
                                   const Matrix<V> &rhs) noexcept;

private:
  std::vector<T> data_;
  std::size_t num_columns;
};

// =========== IMPLEMENTATION ===========

// =========== Matrix Row ===============
template <typename T>
constexpr Matrix<T>::Row::Row(Matrix &parent, std::size_t row) noexcept
    : start{parent.data_.begin() + row * parent.num_columns},
      finish{start + parent.num_columns} {}

template <typename T>
constexpr T &Matrix<T>::Row::operator[](std::size_t col) noexcept {
  return *(start + col);
}

template <typename T>
constexpr const T &Matrix<T>::Row::operator[](std::size_t col) const noexcept {
  return *(start + col);
}

template <typename T>
constexpr std::size_t Matrix<T>::Row::size() const noexcept {
  return finish - start;
}

template <typename T>
constexpr typename Matrix<T>::Row::iterator Matrix<T>::Row::begin() noexcept {
  return start;
}

template <typename T>
constexpr typename Matrix<T>::Row::iterator Matrix<T>::Row::end() noexcept {
  return finish;
}

template <typename T>
constexpr typename Matrix<T>::Row::const_iterator
Matrix<T>::Row::begin() const noexcept {
  return start;
}

template <typename T>
constexpr typename Matrix<T>::Row::const_iterator
Matrix<T>::Row::end() const noexcept {
  return finish;
}

// =========== Matrix row iterator ======

template <typename T>
constexpr Matrix<T>::iterator::iterator(Matrix &parent,
                                        std::size_t row_index) noexcept
    : parent{parent}, row_index{row_index} {}

template <typename T>
constexpr typename Matrix<T>::Row Matrix<T>::iterator::operator*() noexcept {
  return Row(parent, row_index);
}

template <typename T>
constexpr typename Matrix<T>::iterator &
Matrix<T>::iterator::operator++() noexcept {
  ++row_index;
  return *this;
}

template <typename T>
constexpr bool
Matrix<T>::iterator::operator==(const iterator &other) const noexcept {
  return row_index == other.row_index && &parent == &other.parent;
}

template <typename T>
constexpr bool
Matrix<T>::iterator::operator!=(const iterator &other) const noexcept {
  return !(*this == other);
}

template <typename T>
constexpr typename Matrix<T>::iterator Matrix<T>::begin() noexcept {
  return iterator(*this, 0);
}

template <typename T>
constexpr typename Matrix<T>::iterator Matrix<T>::end() noexcept {
  return iterator(*this, size());
}

// =========== Matrix const row iterator ======

template <typename T>
constexpr Matrix<T>::const_iterator::const_iterator(
    const Matrix &parent, std::size_t row_index) noexcept
    : parent{parent}, row_index{row_index} {}

template <typename T>
constexpr const typename Matrix<T>::Row
Matrix<T>::const_iterator::operator*() const noexcept {
  return Row(parent, row_index);
}

template <typename T>
constexpr typename Matrix<T>::const_iterator &
Matrix<T>::const_iterator::operator++() noexcept {
  ++row_index;
  return *this;
}

template <typename T>
constexpr bool Matrix<T>::const_iterator::operator==(
    const const_iterator &other) const noexcept {
  return row_index == other.row_index && &parent == &other.parent;
}

template <typename T>
constexpr bool Matrix<T>::const_iterator::operator!=(
    const const_iterator &other) const noexcept {
  return !(*this == other);
}

template <typename T>
constexpr typename Matrix<T>::const_iterator Matrix<T>::begin() const noexcept {
  return const_iterator(*this, 0);
}

template <typename T>
constexpr typename Matrix<T>::const_iterator Matrix<T>::end() const noexcept {
  return const_iterator(*this, size());
}

// =========== Matrix ===================
template <typename T>
constexpr typename Matrix<T>::Row
Matrix<T>::operator[](std::size_t row) noexcept {
  return Row(*this, row);
}

template <typename T>
constexpr const typename Matrix<T>::Row
Matrix<T>::operator[](std::size_t row) const noexcept {
  return Row(*this, row);
}

template <typename T>
constexpr Matrix<T>::Matrix(std::size_t num_rows, std::size_t num_columns,
                            const T &initial_value)
    : data_{std::vector<T>(num_rows * num_columns, initial_value)},
      num_columns{num_columns} {}

template <typename T>
constexpr void Matrix<T>::resize(std::size_t num_rows, std::size_t num_columns,
                                 const T &initial_value) {
  data_.resize(num_rows * num_columns, initial_value);
  this->num_columns = num_columns;
}

template <typename T>
constexpr void Matrix<T>::reshape(std::size_t num_rows,
                                  std::size_t num_columns) {
  if (num_rows * num_columns != data_.size()) {
    throw std::runtime_error("Cannot reshape matrix to different size");
  }
  this->num_columns = num_columns;
}

template <typename T>
constexpr T &Matrix<T>::operator()(std::size_t row, std::size_t col) noexcept {
  return data_[row * num_columns + col];
}

template <typename T>
constexpr const T &Matrix<T>::operator()(std::size_t row,
                                         std::size_t col) const noexcept {
  return data_[row * num_columns + col];
}

template <typename T>
constexpr std::pair<std::size_t, std::size_t>
Matrix<T>::shape() const noexcept {
  return {data_.size() / num_columns, num_columns};
}

template <typename T>
constexpr std::size_t Matrix<T>::size() const noexcept {
  return data_.size() / num_columns;
}

template <typename T>
constexpr typename std::vector<T>::const_iterator
Matrix<T>::data() const noexcept {
  return data_.begin();
}

template <typename T>
constexpr typename std::vector<T>::const_iterator
Matrix<T>::data(std::size_t row_index) const noexcept {
  return data_.begin() + num_columns * row_index;
}

template <typename T>
std::ostream &Matrix<T>::repr(std::ostream &os) const {
  auto [num_rows, num_columns] = shape();
  os << "Matrix(\n\t";
  os << "num_columns: " << num_columns << "\n\t";
  os << "data_: [";
  for (std::size_t i = 0; i < data_.size(); ++i) {
    if (i != 0)
      os << ' ';
    os << data_[i];
  }
  os << "]\n";

  return os;
}

template <typename T>
constexpr std::ostream &operator<<(std::ostream &os,
                                   const Matrix<T> &m) noexcept {
  auto [num_rows, num_columns] = m.shape();
  for (std::size_t i = 0; i < num_rows; ++i) {
    for (std::size_t j = 0; j < num_columns; ++j) {
      os << m(i, j) << " ";
    }
    os << "\n";
  }
  return os;
}

template <typename U, typename V>
constexpr bool operator==(const Matrix<U> &lhs, const Matrix<V> &rhs) noexcept {
  return lhs.data_ == rhs.data_ && lhs.num_columns == rhs.num_columns;
}

template <typename U, typename V>
constexpr bool operator!=(const Matrix<U> &lhs, const Matrix<V> &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace utils
