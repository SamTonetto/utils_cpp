/**********************************************************************
 * @brief Check if object is instantiation of a template class.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <type_traits>

namespace utils {

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
} // namespace utils
