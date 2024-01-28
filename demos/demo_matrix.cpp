#include "utils_cpp/matrix.hpp"

#include <iostream>

using namespace utils;

int main() {

  // Create a matrix. Note that template type is inferred from the value of
  // initial value. If an initial value is not supplied, the template parameter
  // may need to be specified, and the type will be default-constructed.
  int num_rows = 3;
  int num_cols = 4;
  int initial_value = 0;
  Matrix mat(num_rows, num_cols, initial_value);

  // size() returns the number of rows:
  std::cout << mat.size() << std::endl;

  // while shape() returns a pair (num_rows, num_cols), like in pytorch.
  auto shape = mat.shape();
  std::cout << shape.first << ", " << shape.second << std::endl;

  // You can access the (i,j) elements directly using parentheses.
  mat(0, 0) = 1;
  mat(1, 1) = 2;

  // You can also access using [i][j].
  mat[0][0] = 3;
  mat[1][1] = 4;

  // The operator[] access method allows you to first access a row before
  // accessing columns. However, if you want to just directly access a
  // particular element, using parentheses is more direct, so it may be faster.
  auto row_zero = mat[0];

  // You can get the number of columns in the row with size()
  std::cout << row_zero.size() << std::endl;

  // You can get iterators to the beginning or end of the row.
  for (auto it = row_zero.begin(); it != row_zero.end(); ++it) {
    std::cout << *it << std::endl;
  }

  // You can also do a range-based for through the row:
  for (auto &elem : row_zero) {
    std::cout << elem << std::endl;
  }

  // Or a normal for-loop:
  for (std::size_t i = 0; i < row_zero.size(); ++i) {
    std::cout << row_zero[i] << std::endl;
  }

  // You can also iterate through the rows themselves:
  // Note that mat.begin() returns a new "Row" object that only contains
  // iterators, so it is fairly lightweight.
  for (auto row : mat) {
    for (auto &elem : row) {
      std::cout << elem << std::endl;
    }
  }

  // .resize(num_rows, num_cols, fill_value) will resize the matrix to have the
  // given number of rows and columns. If that is more than the current number
  // of rows and columns, the new elements will be filled with the given
  // fill_value. If it is less, the matrix will be truncated.
  mat.resize(2, 2, 0);

  // .reshape(num_rows, num_cols) will reshape the matrix to have the given
  // number of columns and rows. The total number of elements must remain the
  // ssame.
  mat.reshape(4, 1);
}
