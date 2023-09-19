#pragma once

#include <concepts>
#include <type_traits>

namespace utils {

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

}
