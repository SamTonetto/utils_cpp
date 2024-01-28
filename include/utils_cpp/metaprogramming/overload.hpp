/**********************************************************************
 * @brief Just contains the common c++ overload pattern
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

namespace utils {

/**
 * Example usage:
 * std:variant<int, float, std::string> var;
 *
 * std::visit(utils::overload{
 *   [](int i) { std::cout << "int: " << i << '\n'; },
 *   [](float f) { std::cout << "float: " << f << '\n'; },
 *   [](const std::string& s) { std::cout << "string: " << s << '\n'; },
 * }, var);
 */
template <typename... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

} // namespace utils
