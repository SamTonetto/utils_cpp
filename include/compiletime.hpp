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

namespace detail {

template <template <typename...> typename Container, typename T>
struct is_same_container_types : std::false_type {};

template <template <typename...> typename Container, typename... Args>
struct is_same_container_types<Container, Container<Args...>> : std::true_type {
};

template <template <typename, std::size_t> typename Container, typename T>
struct is_instance_of_variant1 : std::false_type {};

template <template <typename, std::size_t> typename Container, typename T,
          std::size_t N>
struct is_instance_of_variant1<Container, Container<T, N>> : std::true_type {};

template <template <std::size_t> typename Container, typename T>
struct is_instance_of_variant2 : std::false_type {};

template <template <std::size_t> typename Container, std::size_t N>
struct is_instance_of_variant2<Container, Container<N>> : std::true_type {};

} // namespace detail

/**
 * is_instance_of can be used to check if two containers are the same,
 * without reference to what they contain.
 * @example
 * std::vector<int> v;
 * std::unordered_map<int, std::string> m;
 *
 * static_assert(is_instance_of<std::vector, decltype(v)>);
 * static_assert(is_instance_of<std::unordered_map, decltype(m)>);
 *
 */
template <template <typename...> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_same_container_types<Container, T>::value;
}

template <template <typename, std::size_t> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_instance_of_variant1<Container, T>::value;
}

template <template <std::size_t> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_instance_of_variant2<Container, T>::value;
}

template <typename T>
constexpr bool is_stl_container() {
  return is_instantiation<std::span, T>() || is_instantiation<std::pair, T>() ||
         is_instantiation<std::tuple, T>() ||
         is_instantiation<std::vector, T>() ||
         is_instantiation<std::array, T>() ||
         is_instantiation<std::bitset, T>() ||
         is_instantiation<std::valarray, T>() ||
         is_instantiation<std::deque, T>() ||
         is_instantiation<std::list, T>() ||
         is_instantiation<std::forward_list, T>() ||
         is_instantiation<std::set, T>() ||
         is_instantiation<std::unordered_set, T>() ||
         is_instantiation<std::map, T>() ||
         is_instantiation<std::unordered_map, T>() ||
         is_instantiation<std::multiset, T>() ||
         is_instantiation<std::multimap, T>() ||
         is_instantiation<std::unordered_multiset, T>() ||
         is_instantiation<std::unordered_multimap, T>() ||
         is_instantiation<std::stack, T>() ||
         is_instantiation<std::queue, T>() ||
         is_instantiation<std::priority_queue, T>();
};

} // namespace utils
