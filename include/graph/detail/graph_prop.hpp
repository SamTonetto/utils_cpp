#pragma once

#include "metaprogramming/custom_concepts.hpp"
#include "metaprogramming/overload.hpp"
#include "print.hpp"

#include <string>
#include <variant>

namespace utils {
namespace gl {

struct GraphProp {

  std::variant<std::string, double, std::vector<double>,
               std::vector<std::vector<double>>>
      p;

  GraphProp(const std::string &s) : p{s} {}

  GraphProp(const char *s) : p{std::string{s}} {}

  template <Arithmetic T>
  GraphProp(const T &n) {
    p.template emplace<double>(static_cast<double>(n));
  }

  template <Arithmetic T>
  GraphProp(const std::vector<T> &v) {
    p.template emplace<std::vector<double>>();
    for (const auto &e : v) {
      std::get<std::vector<double>>(p).push_back(static_cast<double>(e));
    }
  }

  template <Arithmetic T>
  GraphProp(const std::vector<std::vector<T>> &v) {
    p.template emplace<std::vector<std::vector<double>>>();
    for (const auto &e : v) {
      std::vector<double> tmp;
      tmp.reserve(e.size());
      for (const auto &ee : e) {
        tmp.push_back(static_cast<double>(ee));
      }
      std::get<std::vector<std::vector<double>>>(p).push_back(tmp);
    }
  }

  GraphProp() = default;

  template <typename T>
  operator const T &() const {
    return std::get<T>(p);
  }

  template <typename T>
  operator T &() {
    return std::get<T>(p);
  }

  std::ostream &print(std::ostream &os = std::cout) const {
    if (std::holds_alternative<std::string>(p)) {
      os << std::get<std::string>(p);
    } else if (std::holds_alternative<double>(p)) {
      os << std::get<double>(p);
    } else if (std::holds_alternative<std::vector<double>>(p)) {
      ::operator<<(os, std::get<std::vector<double>>(p));
    } else if (std::holds_alternative<std::vector<std::vector<double>>>(p)) {
      ::operator<<(os, std::get<std::vector<std::vector<double>>>(p));
    }

    return os;
  }

  template <typename T>
  friend bool operator==(const GraphProp &lhs, const T &rhs);
};

template <typename T>
bool operator==(const GraphProp &lhs, const T &rhs) {
  if constexpr (std::is_same_v<T, GraphProp>) { // RHS is GraphPropp

    return lhs.p == rhs.p;

  } else if constexpr (std::is_same_v<T, double>) { // RHS is arithmetic
    return std::get<double>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, std::vector<double>>) {
    return std::get<std::vector<double>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
    return std::get<std::vector<std::vector<double>>>(lhs.p) == rhs;
  } else if constexpr (std::is_same_v<T, std::string> ||
                       (std::is_array_v<std::remove_reference_t<T>> &&
                        std::is_same_v<
                            std::remove_extent_t<std::remove_reference_t<T>>,
                            char>)) { // RHS is string or string literal

    return std::get<std::string>(lhs.p) == rhs;
  } else {

    return false;
  }
}

} // namespace gl

} // namespace utils
