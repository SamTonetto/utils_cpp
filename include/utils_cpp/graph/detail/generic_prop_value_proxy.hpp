#pragma once

#include "utils_cpp/metaprogramming/custom_concepts.hpp"
#include "utils_cpp/metaprogramming/overload.hpp"

#include <iostream>
#include <string>
#include <variant>

#include "utils_cpp/print.hpp"

namespace utils {

namespace gl {

template <template <typename...> typename MapType, typename GraphType>
struct GenericProp;

template <typename Key, template <typename...> typename MapType,
          typename GraphType>
struct ConstGenericPropValueProxy;

template <typename Key, template <typename...> typename MapType,
          typename GraphType>
struct GenericPropValueProxy {

  Key k;
  GenericProp<MapType, GraphType> &gp;

  GenericPropValueProxy(const Key &k, GenericProp<MapType, GraphType> &gp)
      : k{k}, gp{gp} {}

  template <Arithmetic T>
  GenericPropValueProxy &operator=(const T &n) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = static_cast<double>(n);
  }

  template <Arithmetic T>
  GenericPropValueProxy &operator=(const std::vector<T> &v) {
    std::vector<double> tmp;
    tmp.reserve(v.size());
    for (const auto &e : v) {
      tmp.push_back(static_cast<double>(e));
    }
    std::get<MapType<std::vector<double>, GraphType>>(gp.p)[k] = std::move(tmp);
    return *this;
  }

  GenericPropValueProxy(const GenericPropValueProxy &rhs)
      : k{rhs.k}, gp{rhs.gp} {}

  GenericPropValueProxy &operator=(const std::string &s) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = s;
    return *this;
  }

  GenericPropValueProxy &operator=(const char *s) {
    std::get<MapType<std::string, GraphType>>(gp.p)[k] = std::string(s);
    return *this;
  }

  std::size_t size() const {

    return std::visit(
        overload{
            [](const MapType<std::string, GraphType> &m) { return m.size(); },
            [](const MapType<double, GraphType> &m) { return m.size(); },
            [](const MapType<std::vector<double>, GraphType> &m) {
              return m.size();
            },
        },
        gp.p);
  }

  GenericPropValueProxy &operator=(const GenericPropValueProxy &rhs) {
    if (this == &rhs) {
      return *this;
    }

    if (std::holds_alternative<MapType<std::string, GraphType>>(gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<std::string, GraphType>>();
      }

      std::get<MapType<std::string, GraphType>>(gp.p)[k] =
          std::get<MapType<std::string, GraphType>>(rhs.gp.p).at(rhs.k);

    } else if (std::holds_alternative<MapType<double, GraphType>>(gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<double, GraphType>>();
      }

      std::get<MapType<double, GraphType>>(gp.p)[k] =
          std::get<MapType<double, GraphType>>(rhs.gp.p).at(rhs.k);

    } else if (std::holds_alternative<MapType<std::vector<double>, GraphType>>(
                   gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<std::vector<double>, GraphType>>();
      }

      std::get<MapType<std::vector<double>, GraphType>>(gp.p)[k] =
          std::get<MapType<std::vector<double>, GraphType>>(rhs.gp.p).at(rhs.k);
    }

    return *this;
  }

  template <typename K, template <typename...> typename M, typename G>
  GenericPropValueProxy &
  operator=(const ConstGenericPropValueProxy<K, M, G> &rhs) {

    if (std::holds_alternative<M<std::string, G>>(rhs.gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<std::string, GraphType>>();
      }

      std::get<M<std::string, G>>(gp.p)[k] =
          std::get<M<std::string, G>>(rhs.gp.p).at(rhs.k);

    } else if (std::holds_alternative<M<double, G>>(rhs.gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<double, GraphType>>();
      }

      std::get<M<double, G>>(gp.p)[k] =
          std::get<M<double, G>>(rhs.gp.p).at(rhs.k);

    } else if (std::holds_alternative<M<std::vector<double>, G>>(rhs.gp.p)) {

      if (size() == 0) {
        gp.template emplace<MapType<std::vector<double>, GraphType>>();
      }

      std::get<M<std::vector<double>, G>>(gp.p)[k] =
          std::get<M<std::vector<double>, G>>(rhs.gp.p).at(rhs.k);
    }

    return *this;
  }

  GenericPropValueProxy() = default;

  template <typename T>
  operator T &() {
    return std::get<MapType<T, GraphType>>(gp.p)[k];
  }

  template <typename K, template <typename...> typename M, typename G>
  friend std::ostream &operator<<(std::ostream &os,
                                  const GenericPropValueProxy<K, M, G> &gp);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const GenericPropValueProxy<K, M, G> &lhs,
                         const T &rhs);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const GenericPropValueProxy<K, M, G> &lhs,
                         const GenericPropValueProxy<K, M, G> &rhs);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const GenericPropValueProxy<K, M, G> &lhs,
                         const ConstGenericPropValueProxy<K, M, G> &rhs);
};

template <typename K, template <typename...> typename M, typename G>
std::ostream &operator<<(std::ostream &os,
                         const GenericPropValueProxy<K, M, G> &gp) {
  if (std::holds_alternative<M<std::string, G>>(gp.gp.p)) {
    os << std::get<M<std::string, G>>(gp.gp.p).at(gp.k);
  } else if (std::holds_alternative<M<double, G>>(gp.gp.p)) {
    os << std::get<M<double, G>>(gp.gp.p).at(gp.k);
  } else if (std::holds_alternative<M<std::vector<double>, G>>(gp.gp.p)) {
    utils::operator<<(os,
                      std::get<M<std::vector<double>, G>>(gp.gp.p).at(gp.k));
  }

  return os;
}

template <typename K, template <typename...> typename M, typename G, typename T>
bool operator==(const GenericPropValueProxy<K, M, G> &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T, GenericPropValueProxy<K, M, G>>) { // RHS is
                                                                     // generic

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];

  } else if constexpr (std::is_same_v<T,
                                      M<std::string, G>>) { // RHS is string map

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<double, G>>) {

    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<std::vector<double>, G>>) {

    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else {
    return false;
  }
}

template <typename K, template <typename...> typename M, typename G>
bool operator==(const GenericPropValueProxy<K, M, G> &lhs,
                const GenericPropValueProxy<K, M, G> &rhs) {

  if (lhs.gp.p.index() != rhs.gp.p.index()) {
    return false;
  }

  if (std::holds_alternative<M<std::string, G>>(lhs.gp.p)) {
    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<double, G>>(lhs.gp.p)) {
    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<double, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<std::vector<double>, G>>(lhs.gp.p)) {
    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::vector<double>, G>>(rhs.gp.p)[rhs.k];
  }
}

template <typename K, template <typename...> typename M, typename G>
bool operator==(const GenericPropValueProxy<K, M, G> &lhs,
                const ConstGenericPropValueProxy<K, M, G> &rhs) {

  if (lhs.gp.p.index() != rhs.gp.p.index()) {
    return false;
  }

  if (std::holds_alternative<M<std::string, G>>(lhs.gp.p)) {
    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<double, G>>(lhs.gp.p)) {
    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<double, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<std::vector<double>, G>>(lhs.gp.p)) {
    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::vector<double>, G>>(rhs.gp.p)[rhs.k];
  }
}

// Const version

template <typename Key, template <typename...> typename MapType,
          typename GraphType>
struct ConstGenericPropValueProxy {

  Key k;
  const GenericProp<MapType, GraphType> &gp;

  ConstGenericPropValueProxy(const Key &k,
                             const GenericProp<MapType, GraphType> &gp)
      : k{k}, gp{gp} {}

  ConstGenericPropValueProxy(const ConstGenericPropValueProxy &rhs)
      : k{rhs.k}, gp{rhs.gp} {}

  std::size_t size() const {

    return std::visit(
        overload{
            [](const MapType<std::string, GraphType> &m) { return m.size(); },
            [](const MapType<double, GraphType> &m) { return m.size(); },
            [](const MapType<std::vector<double>, GraphType> &m) {
              return m.size();
            },
        },
        gp.p);
  }

  ConstGenericPropValueProxy &operator=(const ConstGenericPropValueProxy &rhs) {
    if (this == &rhs) {
      return *this;
    }

    std::visit(
        overload{
            [&](MapType<std::string, GraphType> &m) {
              m[k] = std::get<MapType<std::string, GraphType>>(rhs.gp.p)[rhs.k];
            },
            [&](MapType<double, GraphType> &m) {
              m[k] = std::get<MapType<double, GraphType>>(rhs.gp.p)[rhs.k];
            },
            [&](MapType<std::vector<double>, GraphType> &m) {
              m[k] = std::get<MapType<std::vector<double>, GraphType>>(
                  rhs.gp.p)[rhs.k];
            },
        },
        rhs.gp.p);

    return *this;
  }

  ConstGenericPropValueProxy() = default;

  template <typename T>
  operator const T &() const {
    std::get<MapType<T, GraphType>>(gp.p)[k];
  }

  template <typename K, template <typename...> typename M, typename G>
  friend std::ostream &
  operator<<(std::ostream &os, const ConstGenericPropValueProxy<K, M, G> &gp);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs,
                         const T &rhs);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs,
                         const ConstGenericPropValueProxy<K, M, G> &rhs);

  template <typename K, template <typename...> typename M, typename G,
            typename T>
  friend bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs,
                         const GenericPropValueProxy<K, M, G> &rhs);
};

template <typename K, template <typename...> typename M, typename G>
std::ostream &operator<<(std::ostream &os,
                         const ConstGenericPropValueProxy<K, M, G> &gp) {
  if (std::holds_alternative<M<std::string, G>>(gp.gp.p)) {
    os << std::get<M<std::string, G>>(gp.gp.p).at(gp.k);
  } else if (std::holds_alternative<M<double, G>>(gp.gp.p)) {
    os << std::get<M<double, G>>(gp.gp.p).at(gp.k);
  } else if (std::holds_alternative<M<std::vector<double>, G>>(gp.gp.p)) {
    utils::operator<<(os,
                      std::get<M<std::vector<double>, G>>(gp.gp.p).at(gp.k));
  }

  return os;
}

template <typename K, template <typename...> typename M, typename G, typename T>
bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T,
                               ConstGenericPropValueProxy<K, M, G>>) { // RHS is
    // generic

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];

  } else if constexpr (std::is_same_v<T,
                                      M<std::string, G>>) { // RHS is string map

    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<double, G>>) {

    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else if constexpr (std::is_same_v<T, M<std::vector<double>, G>>) {

    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] == rhs;

  } else {
    return false;
  }
}

template <typename K, template <typename...> typename M, typename G>
bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs,
                const ConstGenericPropValueProxy<K, M, G> &rhs) {

  if (lhs.gp.p.index() != rhs.gp.p.index()) {
    return false;
  }

  if (std::holds_alternative<M<std::string, G>>(lhs.gp.p)) {
    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<double, G>>(lhs.gp.p)) {
    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<double, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<std::vector<double>, G>>(lhs.gp.p)) {
    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::vector<double>, G>>(rhs.gp.p)[rhs.k];
  }
}

template <typename K, template <typename...> typename M, typename G>
bool operator==(const ConstGenericPropValueProxy<K, M, G> &lhs,
                const GenericPropValueProxy<K, M, G> &rhs) {

  if (lhs.gp.p.index() != rhs.gp.p.index()) {
    return false;
  }

  if (std::holds_alternative<M<std::string, G>>(lhs.gp.p)) {
    return std::get<M<std::string, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::string, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<double, G>>(lhs.gp.p)) {
    return std::get<M<double, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<double, G>>(rhs.gp.p)[rhs.k];
  } else if (std::holds_alternative<M<std::vector<double>, G>>(lhs.gp.p)) {
    return std::get<M<std::vector<double>, G>>(lhs.gp.p)[lhs.k] ==
           std::get<M<std::vector<double>, G>>(rhs.gp.p)[rhs.k];
  }
}

} // namespace gl
} // namespace utils
