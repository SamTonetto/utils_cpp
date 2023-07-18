/**********************************************************************
 * @brief Compile-time utilities
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <bitset>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

namespace utils {

template <template <typename...> typename Target, typename T>
struct is_same_container : std::false_type {};

template <template <typename...> typename Target, typename... Args>
struct is_same_container<Target, Target<Args...>> : std::true_type {};

/**
 * is_same_container_v can be used to check if two containers are the same,
 * without reference to what they contain.
 * @example
 * std::vector<int> v;
 * std::unordered_map<int, std::string> m;
 *
 * static_assert(is_same_container_v<std::vector, decltype(v)>);
 * static_assert(is_same_container_v<std::unordered_map, decltype(m)>);
 *
 */
template <template <typename...> typename Target, typename T>
inline constexpr bool is_same_container_v = is_same_container<Target, T>::value;

template <typename T>
struct is_stl_container {
  static constexpr bool value =

      is_same_container_v<std::span, T> || is_same_container_v<std::pair, T> ||
      is_same_container_v<std::tuple, T> ||
      is_same_container_v<std::vector, T> ||
      is_same_container_v<std::array, T> ||
      is_same_container_v<std::bitset, T> ||
      is_same_container_v<std::valarray, T> ||
      is_same_container_v<std::deque, T> || is_same_container_v<std::list, T> ||
      is_same_container_v<std::forward_list, T> ||
      is_same_container_v<std::set, T> ||
      is_same_container_v<std::unordered_set, T> ||
      is_same_container_v<std::map, T> ||
      is_same_container_v<std::unordered_map, T> ||
      is_same_container_v<std::multiset, T> ||
      is_same_container_v<std::multimap, T> ||
      is_same_container_v<std::unordered_multiset, T> ||
      is_same_container_v<std::unordered_multimap, T> ||
      is_same_container_v<std::stack, T> || is_same_container_v<std::queue, T>;
};

/**
 * is_stl_container_v can be used to check if a type is an STL container.
 * @example
 * static_assert(is_stl_container_v<std::vector<int>>);
 * static_assert(is_stl_container_v<std::unordered_map<int, std::string>>);
 *
 */
template <class T>
inline constexpr bool is_stl_container_v = is_stl_container<T>::value;

} // namespace utils
