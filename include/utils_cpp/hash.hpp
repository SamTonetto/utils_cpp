/**********************************************************************
 * @brief Hash functions for various data types
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <array>
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
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/** taken from python's frozenset:
 * https://stackoverflow.com/questions/20832279/python-frozenset-hashing-algorithm-implementation
 *
 * I benchmarked this against sorting then using hash_combine and it was faster,
 * with fewer collisions, for the case of inserting all the edges of a complete
 * undirected graph.
 */
template <typename T>
inline void symmetric_hash_combine(std::size_t &seed, const T &v) {
  seed ^= (v ^ (v << 16) ^ 89869747UL) * 3644798167UL;
}

/**
 * @brief hash function for std::pair
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
 * @brief Symmetric hash function for std::pair
 */
template <typename T1, typename T2>
struct symmetric_pair_hash {

  std::size_t operator()(const std::pair<T1, T2> &pair) const {
    std::size_t seed = 0;
    symmetric_hash_combine(seed, pair.first);
    symmetric_hash_combine(seed, pair.second);
    return seed;
  }
};

/**
 * @brief Necessary for KeyEquals template parameter of unordered_set and
 * unordered_map.
 *
 */
template <typename T1, typename T2>
struct symmetric_pair_equal {
  bool operator()(const std::pair<T1, T2> &p1,
                  const std::pair<T1, T2> &p2) const {
    return (p1.first == p2.first && p1.second == p2.second) ||
           (p1.first == p2.second && p1.second == p2.first);
  }
};

/**
 * @brief hash function for std::vector
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
 * @brief Permutation-invariant hash function for an std::vector.
 */
template <typename T>
struct symmetric_vector_hash {
  std::size_t operator()(const std::vector<T> &v) const {
    std::size_t seed = 0;
    for (const auto &elem : v) {
      symmetric_hash_combine(seed, elem);
    }
    return seed;
  }
};

/**
 * @brief Necessary for KeyEquals template parameter of unordered_set and
 * unordered_map.
 *
 */
template <typename T>
struct symmetric_vector_equal {
  bool operator()(const std::vector<T> &v1, const std::vector<T> &v2) const {

    if (v1.size() != v2.size())
      return false;

    std::unordered_map<T, std::size_t> counts;
    for (const auto &x : v1) {
      counts[x]++;
    }
    for (const auto &y : v2) {
      if (!counts.contains(y)) {
        return false;
      } else {
        counts[y]--;
        if (counts[y] == 0)
          counts.erase(y);
      }
    }
    return true;
  }
};

/**
 * @brief Permutation-invariant hash function for an std::array
 */
template <typename T, std::size_t N>
struct symmetric_array_hash {
  std::size_t operator()(const std::array<T, N> &a) const {
    std::size_t seed = 0;
    for (const auto &elem : a) {
      symmetric_hash_combine(seed, elem);
    }
    return seed;
  }
};

/**
 * @brief Necessary for KeyEquals template parameter of unordered_set and
 * unordered_map.
 *
 */
template <typename T, std::size_t N>
struct symmetric_array_equal {
  bool operator()(const std::array<T, N> &a1,
                  const std::array<T, N> &a2) const {

    std::unordered_map<T, std::size_t> counts;
    for (const auto &x : a1) {
      counts[x]++;
    }
    for (const auto &y : a2) {
      if (!counts.contains(y)) {
        return false;
      } else {
        counts[y]--;
        if (counts[y] == 0)
          counts.erase(y);
      }
    }
    return true;
  }
};

/**
 * @brief permutation-invariant hash function for std::set
 * types.
 */
template <typename T>
struct set_hash {
  std::size_t operator()(const std::set<T> &s) const {
    std::size_t seed = 0;
    for (const auto &elem : s) {
      symmetric_hash_combine(seed, elem);
    }
    return seed;
  }
};

/**
 * @brief Permutation-invariant hash function for std::unordered_set
 * types.
 */
template <typename T>
struct unordered_set_hash {
  std::size_t operator()(const std::unordered_set<T> &s) const {
    std::size_t seed = 0;
    for (const auto &elem : s) {
      symmetric_hash_combine(seed, elem);
    }
    return seed;
  }
};

/**
 * @brief Permutation-invariant hash function for std::map
 */
template <typename Key, typename V>
struct map_hash {

  std::size_t operator()(const std::map<Key, V> &m) const {
    std::size_t seed = 0;
    for (const auto &elem : m) {
      symmetric_hash_combine(seed, symmetric_pair_hash<Key, V>{}(elem));
    }
    return seed;
  }
};

/**
 * @brief Permutation-invariant hash function for std::unordered_map
 */
template <typename Key, typename V>
struct unordered_map_hash {

  std::size_t operator()(const std::unordered_map<Key, V> &m) const {
    std::size_t seed = 0;
    for (const auto &elem : m) {
      symmetric_hash_combine(seed, symmetric_pair_hash<Key, V>{}(elem));
    }
    return seed;
  }
};

/**
 * @brief array hash
 */
template <typename T, size_t N>
struct array_hash {
  std::size_t operator()(const std::array<T, N> &a) const {
    std::size_t seed = 0;
    for (const auto &elem : a) {
      hash_combine(seed, elem);
    }
    return seed;
  }
};

} // namespace utils
