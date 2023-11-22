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
                               std::string_view delimiter);

/**
 * A python-like startswith function.
 */
bool startswith(std::string_view strv, std::string_view prefix);

/**
 * A python-like join function.
 */
std::string join(const std::vector<std::string> &strings,
                 std::string_view delimiter);

/**
 * Check if a string is convertible to a double. It is quite strict -
 * whitespace is not allowed, string must be fully consumed, no extraneous
 * leading zeros.
 */
bool convertible_to_double(const std::string &s);

/**
 * Check if a string is convertible to a long long. No whitespace is allowed,
 * no decimal points (unless decimal part is zero), and no extraneous leading
 * zeros.
 */
bool convertible_to_long_long(const std::string &s);

// ==============================
// ======= Implementation =======
// ==============================

inline std::vector<std::string> split(std::string_view strv,
                                      std::string_view delimiter = " ") {

  if (delimiter.empty()) {
    throw std::invalid_argument("Delimiter cannot be an empty string.");
  }

  std::vector<std::string> result;
  size_t pos;

  if (delimiter.size() == 1 && std::isspace(delimiter[0])) {

    std::size_t start = 0;
    while ((pos = strv.find_first_not_of(" \t\n\r\f\v", start)) !=
           std::string_view::npos) {
      start = strv.find_first_of(" \t\n\r\f\v", pos);
      result.emplace_back(strv.substr(pos, start - pos));
    }

  } else {

    while ((pos = strv.find(delimiter)) != std::string_view::npos) {
      result.emplace_back(strv.substr(0, pos));
      strv.remove_prefix(pos + delimiter.size()); // remove delimiter itself
    }

    // add what remains
    if (!strv.empty()) {
      result.emplace_back(strv);
    }
  }

  return result;
}

inline bool startswith(std::string_view strv, std::string_view prefix) {

  if (strv.size() < prefix.size())
    return false;

  for (size_t i = 0; i < prefix.size(); ++i) {
    if (strv[i] != prefix[i]) {
      return false;
    }
  }

  return true;
}

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

inline bool convertible_to_double(const std::string &s) {
  // no extraneous leading zeros
  if ((s.size() > 1 && s[0] == '0' && s[1] != '.') ||
      (s.size() > 2 && s[0] == '-' && s[1] == '0' && s[1] != '.'))
    return false;

  std::istringstream iss(s);
  double num;
  iss >> std::noskipws >> num;
  return iss.eof() && !iss.fail();
}

inline bool convertible_to_long_long(const std::string &s) {

  if ((s.size() > 1 && s[0] == '0') ||
      (s.size() > 2 && s[0] == '-' && s[1] == '0'))
    return false;

  std::istringstream iss(s);
  long long num;
  iss >> std::noskipws >> num;
  return iss.eof() && !iss.fail();
}

} // namespace utils
