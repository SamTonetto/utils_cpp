#pragma once

#include <array>
#include <cstddef>

namespace utils {

/**
 * @brief Create a read-only flat-map from an enum value to a value of another
 * type at compile-time.
 */
template <typename E, typename T, std::size_t N>
class enum_map {
public:
  explicit constexpr enum_map(const std::array<T, N> &arr) noexcept
      : arr_{arr} {}

  constexpr const T &operator[](E e) const {
    return arr_[static_cast<std::size_t>(e)];
  }

private:
  std::array<T, N> arr_;
};

// Somehow having a separate `make_enum_map` function helps with type deduction.
// For example, you can write `make_enum_map<Enum, int>{...}` but you have to
// write e.g. enum_map<Enum, int, 5> { ... }.
template <typename E, typename T, std::size_t N>
constexpr enum_map<E, T, N> make_enum_map(std::pair<E, T> (&&a)[N]) {
  std::array<T, N> result;
  for (const auto &[key, value] : a) {
    result[static_cast<std::size_t>(key)] = value;
  }
  return enum_map<E, T, N>{result};
};

} // namespace utils
