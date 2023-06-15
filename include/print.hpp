/**********************************************************************
 * @brief Library for printing common data structures, usually for debugging
 *purposes.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <iostream>
#include <vector>

namespace utils {
namespace print {

template <typename T>
std::ostream &v(const std::vector<T> &vec, std::ostream &os = std::cout,
                std::string prefix = "") {
  for (const auto &x : vec) {
    os << prefix << x << " ";
  }
}

template <typename T>
std::ostream &vv(const std::vector<std::vector<T>> &mat,
                 std::ostream &os = std::cout, std::string prefix = "") {
  for (const auto &x : mat) {
    os << prefix << v(x) << std::endl;
  }
}

template <typename T>
std::ostream &vvv(const std::vector<std::vector<std::vector<T>>> &tensor,
                  std::ostream &os = std::cout, std::string prefix = "") {

  for (const auto &x : tensor) {
    os << prefix << vv(x) << std::endl;
  }
}

} // namespace print
} // namespace utils
