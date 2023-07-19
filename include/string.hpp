/**********************************************************************
 * @brief Utilities for string (and other) parsing.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace utils {

/**
 * A python-like split function.
 */
std::vector<std::string> split(std::string_view strv,
                               std::string_view delimiter = " ");

/**
 * A python-like join function.
 */
std::string join(const std::vector<std::string> &strings,
                 std::string_view delimiter);

/**
 * Check if a string is convertible to a double. It is quite strict - whitespace
 * is not allowed and the string must be fully consumed.
 */
bool convertible_to_double(const std::string &s);

} // namespace utils
