/**********************************************************************
 * @brief Timing utilities
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <chrono>
#include <future>
#include <optional>
#include <utility>

namespace utils {

/**
 * Timing decorator. You can wrap any function in it and it will return a pair,
 * where the first element is an std::optional containing the result of the
 * function (optional to handle potentially void functions) and the second
 * element is the time it took to execute the function in nanoseconds.
 *
 * Note that you have to define the wrapper function as constexpr if you want it
 * to be compile-time.
 *
 * @example
 * int increment(int x) { return x + 1; }
 * int main() {
 *  constexpr auto timed_increment = time(increment);
 *  auto [result, duration] =
 *  timed_increment(1);
 *
 *  std::cout << "Result: " << result.value() << std::endl;
 *  std::cout << "Duration: " << duration << std::endl;
 * }
 *
 *
 */
template <typename F>
constexpr auto time(const F &func) {

  return [func](auto &&...args) {
    auto start = std::chrono::high_resolution_clock::now();

    using func_return_type =
        decltype(func(std::forward<decltype(args)>(args)...));

    std::optional<typename std::conditional<std::is_void_v<func_return_type>,
                                            int, func_return_type>::type>
        optional_result;

    if constexpr (std::is_same_v<func_return_type, void>) {
      func(std::forward<decltype(args)>(args)...);
      auto end = std::chrono::high_resolution_clock::now();

      auto duration_ns =
          std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count();

      double duration_seconds = static_cast<double>(duration_ns) / 1e9;

      return std::make_pair(optional_result, duration_seconds);
    } else {
      optional_result = func(std::forward<decltype(args)>(args)...);

      auto end = std::chrono::high_resolution_clock::now();

      auto duration_ns =
          std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count();

      double duration_seconds = static_cast<double>(duration_ns) / 1e9;
      return std::make_pair(optional_result, duration_seconds);
    }
  };
}

} // namespace utils
