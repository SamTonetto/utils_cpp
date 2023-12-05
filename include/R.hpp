/**********************************************************************
 * @brief Replication of some of the basic features of R.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

#include "metaprogramming/is_instantiation.hpp"

namespace utils {

/**
 * @brief Create a sequence of values from start to end inclusive, increasing by
 * by.
 */
template <typename T>
  requires std::is_arithmetic_v<T>
std::vector<T> seq(T start, T end, T by = static_cast<T>(1)) {

  if (by == 0 || (start < end && by < 0) || (start > end && by > 0)) {
    throw std::invalid_argument("Invalid arguments for seq.");
  }

  std::vector<T> result;
  result.reserve((end - start) / by + 1);

  if (by > 0) {
    for (T i = start; i <= end; i += by) {
      result.push_back(i);
    }
  } else {
    // Note: If T is unsigned, by > 0 should always be true.
    for (T i = start; i >= end; i += by) {
      result.push_back(i);
    }
  }
  return result;
}

/**
 * @brief If 'value' is a scalar, return a vector of length 'times * each' with
 * each element equal to 'value'. If 'value' is a vector, return a new vector
 * where each element of 'value' has been repeated 'each' times, and the
 * resulting vector has been repeated 'times' times.
 */
template <typename T>
auto rep(const T &v, std::size_t times, std::size_t each) {
  if constexpr (is_instantiation<std::vector, T>()) {
    std::vector<typename T::value_type> result;
    result.reserve(v.size() * times * each);
    for (std::size_t i = 0; i < times; ++i) {
      for (std::size_t j = 0; j < each; ++j) {
        for (const auto &value : v) {
          result.push_back(value);
        }
      }
    }
    return result;
  } else {
    std::vector<T> result(times * each, v);
    return result;
  }
}

/**
 * @brief Applies a function 'func' to each element of a vector 'vec'.
 * The depth parameter specifies how deeply the function should be applied, if
 * the vector is nested.
 *
 * @param vec The vector to apply the function to.
 * @param func The function to apply.
 * @param depth The depth to apply the function to. If depth is 0, the function
 * is applied to the vector itself. If depth is 1, the function is applied to
 * each element of the vector. If depth is N, the function is applied to the Nth
 * level of nesting. If depth is -1, the function is applied to the deepest
 * level of nesting.
 */
template <typename T, typename Function>
std::vector<T> fapply(const std::vector<T> &vec, Function func) {

  std::vector<T> result(vec.size());
  for (std::size_t i = 0; i < vec.size(); ++i) {
    if constexpr (is_instantiation<std::vector, T>()) {
      result[i] = fapply<typename T::value_type, Function>(vec[i], func);
    } else {
      result[i] = func(vec[i]);
    }
  }
  return result;
}

} // namespace utils
