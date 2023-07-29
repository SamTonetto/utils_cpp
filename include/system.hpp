/**********************************************************************
 * @brief Utilities for getting system information. These functions are mostly
 *untested since I only develop on Mac (usually).
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

/**
 * WARNING: Most of these functions are untested - don't be surprised if they
 * break.
 *
 */

#pragma once

#include <iostream>
#include <string>

namespace utils {

namespace system {

/**
 * Get current operating system.
 * Untested.
 */
inline std::string os() {
#ifdef _WIN32
  return "Windows";
#elif __APPLE__
  return "OSX";
#elif __linux__
  return "Linux";
#else
  return "Unknown";
#endif
}

} // namespace system
} // namespace utils
