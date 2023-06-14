/**********************************************************************
 * @brief Custom random utilities that aim to produce consistent results
 * independent of platform (unlike e.g. STL distributions).
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <random>

namespace utils {

/**
 * @brief Generates a random integer in the range [0, 1] *inclusive*.
 */
inline int random_int(std::mt19937 &gen) { return gen() & 1; }

/**
 * @brief Generates a random double in the range [0, 1] *inclusive*.
 */
inline double random_double(std::mt19937 &gen) {
  return static_cast<double>(gen()) / static_cast<double>(gen.max() + 1.0);
}

/**
 * @brief Generates a random integer in the range [min, max) (i.e. min is
 * included, max is excluded). Uses rejection sampling to ensure a uniform
 * distribution.
 */
inline int random_int(std::mt19937 &gen, int min, int max) {

  int range_size = max - min + 1;
  int max_value = gen.max() - gen.max() % range_size;

  int sampled_int;
  do {
    sampled_int = gen();
  } while (sampled_int >= max_value);

  return min + sampled_int % range_size;
}

/**
 * @brief Generates a random double in the range [min, max] *inclusive*.
 */
inline double random_double(std::mt19937 &gen, double min, double max) {
  return min + random_double(gen) * (max - min);
}

} // namespace utils
