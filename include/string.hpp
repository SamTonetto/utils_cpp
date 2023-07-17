/**********************************************************************
 * @brief Utilities for string (and other) parsing.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <string>
#include <vector>

namespace utils {

/**
 * A python-like split function.
 */
inline std::vector<std::string> split(std::string_view strv,
                                      std::string_view delimiter = " ") {

  std::vector<std::string> result;
  size_t pos;

  while ((pos = strv.find(delimiter)) != std::string_view::npos) {
    result.emplace_back(strv.substr(0, pos));
    strv.remove_prefix(pos + delimiter.size()); // remove delimiter itself
  }

  // add what remains
  if (!strv.empty()) {
    result.emplace_back(strv);
  }

  return result;
}

/**
 * A python-like join function.
 */
inline std::string join(const std::vector<std::string> &strings,
                        std::string_view delimiter) {
  if (strings.empty()) {
    return "";
  }

  std::string result;

  size_t total_length = 0;
  for (const auto &str : strings) {
    total_length += str.size();
  }
  total_length += delimiter.size() * (strings.size() - 1);

  result.reserve(total_length);

  if (!strings.empty()) {
    result += strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
      result += std::string(delimiter) + strings[i];
    }
  }
  return result;
}

} // namespace utils
