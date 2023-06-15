/**********************************************************************
 * @brief Compile-time utilities
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace utils {

template <typename T>
struct is_pair {
  static constexpr bool value = false;
};

template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> {
  static constexpr bool value = true;
};

template <typename T>
struct is_vector {
  static constexpr bool value = false;
};

template <typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> {
  static constexpr bool value = true;
};

template <typename T>
struct is_set {
  static constexpr bool value = false;
};

template <typename Key, typename Compare, typename Allocator>
struct is_set<std::set<Key, Compare, Allocator>> {
  static constexpr bool value = true;
};

template <typename T>
struct is_unordered_set {
  static constexpr bool value = false;
};

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct is_unordered_set<std::unordered_set<Key, Hash, KeyEqual, Allocator>> {
  static constexpr bool value = true;
};

template <typename T>
struct is_map {
  static constexpr bool value = false;
};

template <typename Key, typename T, typename Compare, typename Allocator>
struct is_map<std::map<Key, T, Compare, Allocator>> {
  static constexpr bool value = true;
};

template <typename T>
struct is_unordered_map {
  static constexpr bool value = false;
};

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator>
struct is_unordered_map<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>> {
  static constexpr bool value = true;
};

} // namespace utils
