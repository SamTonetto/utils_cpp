/**********************************************************************
 * @brief Hash functions for various data types
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace utils {

/**
 * This is basically boost::hash_combine
 */
template <typename T>
inline void hash_combine(std::size_t &seed, const T &v) {
  seed ^= std::hash<T>(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * @brief Hash function for std::pair
 */
template <typename T1, typename T2>
struct pair_hash {
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    std::size_t seed = 0;
    hash_combine(seed, p.first);
    hash_combine(seed, p.second);
    return seed;
  }
};

/**
 * @brief Hash function for std::vector
 */
template <typename T>
struct vector_hash {
  std::size_t operator()(const std::vector<T> &v) const {
    std::size_t seed = 0;
    for (const auto &elem : v) {
      hash_combine(seed, elem);
    }
    return seed;
  }
};

/**
 * @brief Permutation-invariant hash function for std::set
 * types.
 */
template <typename T>
struct set_hash {
  std::size_t operator()(const std::set<T> &s) const {
    std::size_t total = 0;
    for (const auto &elem : s) {
      total += std::hash<T>(elem);
    }

    return total;
  }
};

/**
 * @brief Permutation-invariant hash function for std::unordered_set
 * types.
 */
template <typename T>
struct unordered_set_hash {
  std::size_t operator()(const std::unordered_set<T> &s) const {
    std::size_t total = 0;
    for (const auto &elem : s) {
      total += std::hash<T>(elem);
    }

    return total;
  }
};

/**
 * @brief Permutation-invariant hash function for std::map
 */
template <typename Key, typename T>
struct map_hash {

  std::size_t operator()(const std::map<Key, T> &m) const {
    std::size_t total = 0;
    for (const auto &elem : m) {
      total +=
          hash_combine(std::hash<Key>(elem.first), std::hash<T>(elem.second));
    }

    return total;
  }
};

/**
 * @brief Permutation-invariant hash function for std::unordered_map
 */
template <typename Key, typename T>
struct unordered_map_hash {

  std::size_t operator()(const std::unordered_map<Key, T> &m) const {
    std::size_t total = 0;
    for (const auto &elem : m) {
      total +=
          hash_combine(std::hash<Key>(elem.first), std::hash<T>(elem.second));
    }

    return total;
  }
};

} // namespace utils
