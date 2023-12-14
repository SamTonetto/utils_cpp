/**********************************************************************
 * @brief operator<< overload for automatic enum value printing
 * @details Inspired by magic_enum
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <array>
#include <source_location>
#include <string_view>

namespace utils {

namespace detail {

inline constexpr std::size_t ENUM_MAX = 256;

template <auto e>
consteval auto get_enum_name_str_view() noexcept {
  const auto func_name =
      std::string_view{std::source_location::current().function_name()};
  auto split = func_name.substr(0, func_name.size() - 1);
  split = split.substr(split.find("e = ") + 4);

  // ensure that if we get e.g. A::B::Enum::Value,
  // it only gets Enum::Value

  std::size_t last_scope = split.rfind("::");
  std::size_t second_last_scope = split.rfind("::", last_scope - 1);

  if (second_last_scope == std::string_view::npos) {
    return split;
  }

  return split.substr(second_last_scope + 2);
}

template <typename E, std::size_t... I>
consteval auto get_enum_fields_array(std::index_sequence<I...>) noexcept {
  return std::array<std::string_view, sizeof...(I)> {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif

    get_enum_name_str_view<static_cast<E>(I)>()...
  };

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

template <typename E>
constexpr auto enum_fields_array_v =
    get_enum_fields_array<E>(std::make_index_sequence<ENUM_MAX>{});

} // namespace detail

template <typename E>
  requires std::is_enum_v<E>
constexpr std::string_view enum_name(E e) noexcept {
  using D = std::decay_t<E>;
  return detail::enum_fields_array_v<D>[static_cast<std::size_t>(e)];
}

} // namespace utils

// In C++23, replace with std::is_scoped_enum_v
template <typename E>
  requires(std::is_enum_v<E> &&
           !std::is_convertible_v<E, std::underlying_type_t<E>>)
std::ostream &operator<<(std::ostream &os, E e) {
  return os << utils::enum_name(e);
}
