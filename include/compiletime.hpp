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
#include <variant>
#include <vector>

namespace utils {

// =============================================================
// Utility to check if type is instantiation of a template class
// =============================================================

namespace detail {

// General case

template <template <typename...> typename Container, typename T>
struct is_instantiation_general : std::false_type {};

template <template <typename...> typename Container, typename... Args>
struct is_instantiation_general<Container, Container<Args...>>
    : std::true_type {};

//  Case for template<typename T, std::size_t N>, e.g. std::array or std::span

template <template <typename, std::size_t> typename Container, typename T>
struct is_instantiation_type_nontype : std::false_type {};

template <template <typename, std::size_t> typename Container, typename T,
          std::size_t N>
struct is_instantiation_type_nontype<Container, Container<T, N>>
    : std::true_type {};

// Case for template<std::size_t N>, e.g. std::bitset

template <template <std::size_t> typename Container, typename T>
struct is_instantiation_nontype : std::false_type {};

template <template <std::size_t> typename Container, std::size_t N>
struct is_instantiation_nontype<Container, Container<N>> : std::true_type {};

} // namespace detail

/**
 * is_instiation can be used to check if two containers are the same,
 * without reference to what they contain.
 * @example
 * std::vector<int> v;
 * std::unordered_map<int, std::string> m;
 *
 * static_assert(is_instantiation<std::vector, decltype(v)>);
 * static_assert(is_instantiation<std::unordered_map, decltype(m)>);
 *
 */
template <template <typename...> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_instantiation_general<Container, T>::value;
}

template <template <typename, std::size_t> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_instantiation_type_nontype<Container, T>::value;
}

template <template <std::size_t> typename Container, typename T>
constexpr bool is_instantiation() {
  return detail::is_instantiation_nontype<Container, T>::value;
}

// =============================================================
// Utility to check if type is STL container
// =============================================================

template <typename T>
constexpr bool is_stl_container() {

  // or-statement roughly ordered by popularity.

  return is_instantiation<std::vector, T>() ||
         is_instantiation<std::map, T>() ||
         is_instantiation<std::unordered_map, T>() ||
         is_instantiation<std::set, T>() ||
         is_instantiation<std::unordered_set, T>() ||
         is_instantiation<std::pair, T>() ||
         is_instantiation<std::tuple, T>() ||
         is_instantiation<std::array, T>() ||
         is_instantiation<std::stack, T>() ||
         is_instantiation<std::queue, T>() ||
         is_instantiation<std::priority_queue, T>() ||
         is_instantiation<std::deque, T>() ||
         is_instantiation<std::variant, T>() ||
         is_instantiation<std::list, T>() || is_instantiation<std::span, T>() ||
         is_instantiation<std::bitset, T>() ||
         is_instantiation<std::valarray, T>() ||
         is_instantiation<std::forward_list, T>() ||
         is_instantiation<std::multiset, T>() ||
         is_instantiation<std::multimap, T>() ||
         is_instantiation<std::unordered_multiset, T>() ||
         is_instantiation<std::unordered_multimap, T>();
}

} // namespace utils
