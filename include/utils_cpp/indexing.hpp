/**********************************************************************
 * @brief Various utility functions for indexing
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <unordered_map>
#include <vector>

namespace utils {

/**
 * Applies an index-mapping to an arbitrarily nested vector, returning the new
 * vector.
 */
inline std::vector<std::size_t>
apply_index_mapping(const std::vector<std::size_t> &vec,
                    std::unordered_map<std::size_t, std::size_t> &mapping) {

  std::vector<std::size_t> new_vec(vec.size());
  for (std::size_t i = 0; i < vec.size(); ++i) {
    new_vec[i] = mapping[vec[i]];
  }

  return new_vec;
}

template <typename T>
std::vector<T>
apply_index_mapping(const std::vector<T> &vec,
                    std::unordered_map<std::size_t, std::size_t> &mapping) {

  std::vector<T> result(vec.size());
  std::size_t i = 0;
  for (const T &subvec : vec) {
    result[i] = apply_index_mapping(subvec, mapping);
    ++i;
  }
  return result;
}

} // namespace utils
