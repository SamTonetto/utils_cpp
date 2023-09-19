/**********************************************************************
 * @brief Generic vertex or edge property
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include "graph/detail/generic_prop_value_proxy.hpp"

#include "metaprogramming/custom_concepts.hpp"

#include "print.hpp"

namespace utils {
namespace gl {

/**
 * MapType can be VertexMap or EdgeMap.
 */
template <template <typename...> typename MapType, typename GraphType>
struct GenericProp {

  std::variant<MapType<std::string, GraphType>, MapType<double, GraphType>,
               MapType<std::vector<double>, GraphType>>
      p;

  GenericProp(const MapType<std::string, GraphType> &m) : p(m) {}

  template <Arithmetic T>
  GenericProp(const MapType<T, GraphType> &m) {

    if constexpr (std::is_same_v<T, double>) {
      p = m;
    } else {
      p.template emplace<MapType<double, GraphType>>();
      for (const auto &[k, v] : m) {
        std::get<MapType<double, GraphType>>(p)[k] = static_cast<double>(v);
      }
    }
  }

  template <Arithmetic T>
  GenericProp(const MapType<std::vector<T>, GraphType> &m) {

    if constexpr (std::is_same_v<T, double>) {
      p = m;
    } else {

      p.template emplace<MapType<std::vector<double>, GraphType>>();
      for (const auto &[k, v] : m) {
        std::vector<double> tmp;
        tmp.reserve(v.size());
        for (const auto &e : v) {
          tmp.push_back(static_cast<double>(e));
        }
        std::get<MapType<std::vector<double>, GraphType>>(p)[k] = tmp;
      }
    }
  }

  GenericProp() = default;

  // variant emplace
  template <typename T>
  void emplace() {
    p.template emplace<T>();
  }

  template <typename Key>
  GenericPropValueProxy<Key, MapType, GraphType> operator[](Key &&k) {
    return GenericPropValueProxy<Key, MapType, GraphType>{std::forward<Key>(k),
                                                          *this};
  }

  template <typename Key>
  ConstGenericPropValueProxy<Key, MapType, GraphType>
  operator[](Key &&k) const {
    return ConstGenericPropValueProxy<Key, MapType, GraphType>{
        std::forward<Key>(k), *this};
  }

  template <typename T>
  operator const MapType<T, GraphType> &() const {
    return std::get<MapType<T, GraphType>>(p);
  }

  template <typename T>
  operator MapType<T, GraphType> &() {
    return std::get<MapType<T, GraphType>>(p);
  }

  template <template <typename...> typename M, typename G, typename T>
  friend bool operator==(const GenericProp<M, G> &lhs, const T &rhs);
};

template <template <typename...> typename M, typename G, typename T>
bool operator==(const GenericProp<M, G> &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T, GenericProp<M, G>>) { // RHS is generic prop
    return lhs.p == rhs.p;
  } else if constexpr (std::is_same_v<T,
                                      M<std::string, G>>) { // RHS is string map
    return std::get<M<std::string, G>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, M<double, G>>) {
    return std::get<M<double, G>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, M<std::vector<double>, G>>) {
    return std::get<M<std::vector<double>, G>>(lhs.p) == rhs;
  } else {
    return false;
  }
}

template <template <typename...> typename M, typename G>
std::ostream &operator<<(std::ostream &os,
                         const utils::gl::GenericProp<M, G> &gp) {

  if (std::holds_alternative<M<std::string, G>>(gp.p)) {
    ::operator<<(os, std::get<M<std::string, G>>(gp.p));
  } else if (std::holds_alternative<M<double, G>>(gp.p)) {
    ::operator<<(os, std::get<M<double, G>>(gp.p));
  } else {
    ::operator<<(os, std::get<M<std::vector<double>, G>>(gp.p));
  }
  return os;
}
} // namespace gl
} // namespace utils
