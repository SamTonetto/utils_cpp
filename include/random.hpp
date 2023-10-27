/**********************************************************************
 * @brief Custom random utilities that aim to produce consistent results
 * independent of platform (unlike e.g. STL distributions).
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <cstdint>
#include <iostream>
#include <random>
#include <unordered_map>

namespace utils {

/**
 * @brief Generates a random integer in the range [0, 1] *inclusive*.
 */
template <typename RandomGenerator>
int random_int(RandomGenerator &&gen) {
  return gen() & 1;
}

/**
 * @brief Generates a random double in the range [0, 1] *inclusive*.
 */
template <typename RandomGenerator>
double random_double(RandomGenerator &&gen) {
  return static_cast<double>(gen()) / static_cast<double>(gen.max() + 1.0);
}

/**
 * @brief Generates a random integer in the range [min, max] (i.e. min and max
 * are both included). Uses rejection sampling to ensure a uniform distribution.
 */
template <typename RandomGenerator>
int random_int(int min, int max, RandomGenerator &&gen) {

  int range_size = max - min + 1;
  unsigned max_value = gen.max() - gen.max() % range_size;

  unsigned sampled_int;
  do {
    sampled_int = gen();
  } while (sampled_int >= max_value);

  return min + static_cast<int>(sampled_int % range_size);
}

/**
 * @brief Generates a random double in the range [min, max] *inclusive*.
 */
template <typename RandomGenerator>
double random_double(double min, double max, RandomGenerator &&gen) {
  return min + random_double(gen) * (max - min);
}

/**
 * @brief Consistently shuffles a vector of elements using Durstenfeld's
 * implementation of the Fisher-Yates algorithm.
 */
template <typename T, typename RandomGenerator>
void random_shuffle(std::vector<T> &data, RandomGenerator &&gen) {
  if (data.empty()) {
    return;
  }
  for (std::size_t i = data.end() - data.begin() - 1; i >= 1; --i) {
    int j = random_int(0, i, gen);
    if (!std::cmp_equal(i, j)) {
      std::swap(data[i], data[j]);
    }
  }
}

/**
 * @brief Consistently shuffles a vector of elements using Durstenfeld's
 * implementation of the Fisher-Yates algorithm.
 */
template <typename RandomAccessIterator, typename RandomGenerator>
void random_shuffle(RandomAccessIterator begin, RandomAccessIterator end,
                    RandomGenerator &&gen) {
  if (begin == end) {
    return;
  }
  for (std::size_t i = end - begin - 1; i >= 1; --i) {
    int j = random_int(0, i, gen);
    if (!std::cmp_equal(i, j)) {
      std::iter_swap(begin + j, begin + i);
    }
  }
}

/**
 * @brief Constructs and returns a shuffled vector of contiguous indices in the
 * range [0, size).
 */
template <typename RandomGenerator>
std::vector<std::size_t> shuffled_iota(std::size_t size,
                                       RandomGenerator &&gen) {

  std::vector<std::size_t> indices(size);
  std::iota(indices.begin(), indices.end(), 0);
  utils::random_shuffle(indices.begin(), indices.end(), gen);
  return indices;
}

template <typename RandomGenerator>
std::unordered_map<std::size_t, std::size_t>
shuffled_mapping(std::size_t size, RandomGenerator &&gen) {

  auto shuffled_indices = shuffled_iota(size, gen);
  std::unordered_map<std::size_t, std::size_t> mapping;
  for (std::size_t i = 0; i < size; ++i) {
    mapping[i] = shuffled_indices[i];
  }
  return mapping;
}

} // namespace utils
