#pragma once

#include <array>
#include <source_location>
#include <string_view>
#include <utility>

namespace utils {

// Max number of values supported in an enum (for compile-time functions to
// work)
inline constexpr std::size_t ENUM_MAX = 256;

/**
 * @brief Compile-time check to see if an enum value is valid.
 * @detail Exploits the fact that the source string for a valid enum has the
 * form [e = Name::Value], while an invalid enum has the form [e = (Name)Value].
 */
template <auto e>
consteval bool valid_enum() noexcept {
  const auto func_name =
      std::string_view{std::source_location::current().function_name()};
  const auto split = func_name.substr(0, func_name.size() - 1);
  const auto enum_str = split.substr(split.find("e = ") + 4);

  return enum_str.find("(") == enum_str.npos;
}

/**
 * @brief Returns the field-name of an enum value at compile-time.
 */
template <auto e>
consteval auto get_enum_field_str_view() noexcept {
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

} // namespace utils
