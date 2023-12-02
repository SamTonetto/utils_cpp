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
#include <unordered_map>

namespace utils {

/**
 * @brief Generates a random integer in the range [0, 1] *inclusive*.
 */
inline int random_int(std::mt19937_64 &gen) { return gen() & 1; }

/**
 * @brief Generates a random double in the range [0, 1] *inclusive*.
 */
inline double random_double(std::mt19937_64 &gen) {
  return static_cast<double>(gen()) / static_cast<double>(gen.max() + 1.0);
}

/**
 * @brief Generates a random integer in the range [min, max] (i.e. min and max
 * are both included).
 *
 * @details Uses rejection sampling to ensure a uniform distribution. First, the
 * range [a,b] is shifted to [0, b-a]. Then, this range is tiled as many times
 * as can fit in the range of the random generator, up to some max value.
 * Finally, a random integer is sampled until it lands in the range [0,
 * max_value) and the result is shifted back to the original range.
 */
template <typename T>
  requires std::is_integral_v<T>
T random_int(T min, T max, std::mt19937_64 &gen) {

  using ULL = unsigned long long;

  ULL range_size = max - min + 1; // if this overflows... too bad
  ULL max_value = gen.max() - gen.max() % range_size;

  ULL sampled_int;
  do {
    sampled_int = gen();
  } while (sampled_int >= max_value);

  return min + static_cast<T>(sampled_int % range_size);
}

/**
 * @brief Generates a random double in the range [min, max] *inclusive*.
 */
inline double random_double(double min, double max, std::mt19937_64 &gen) {
  return min + random_double(gen) * (max - min);
}

/**
 * @brief Selects a random element from a vector of elements.
 */
template <typename T>
T random_choice(const std::vector<T> &data, std::mt19937_64 &gen) {

  if (data.empty()) {
    throw std::invalid_argument(
        "Cannot select a random element from an empty vector.");
  }

  return data[random_int(0, data.size() - 1, gen)];
}

/**
 * @brief Consistently shuffles a vector of elements using Durstenfeld's
 * implementation of the Fisher-Yates algorithm.
 */
template <typename T>
void random_shuffle(std::vector<T> &data, std::mt19937_64 &gen) {
  if (data.empty()) {
    return;
  }
  for (std::size_t i = data.size() - 1; i >= 1; --i) {
    std::size_t j = random_int(0ul, i, gen);
    if (i != j) {
      std::swap(data[i], data[j]);
    }
  }
}

/**
 * @brief Consistently shuffles a vector of elements using Durstenfeld's
 * implementation of the Fisher-Yates algorithm.
 */
template <typename RandomAccessIterator>
void random_shuffle(RandomAccessIterator begin, RandomAccessIterator end,
                    std::mt19937_64 &gen) {
  if (begin == end) {
    return;
  }
  for (std::size_t i = end - begin - 1; i >= 1; --i) {
    std::size_t j = random_int(0ul, i, gen);
    if (i != j) {
      std::iter_swap(begin + j, begin + i);
    }
  }
}

/**
 * @brief Constructs and returns a shuffled vector of contiguous indices in the
 * range [0, size).
 */
inline std::vector<std::size_t> shuffled_iota(std::size_t size,
                                              std::mt19937_64 &gen) {

  std::vector<std::size_t> indices(size);
  std::iota(indices.begin(), indices.end(), 0);
  utils::random_shuffle(indices.begin(), indices.end(), gen);
  return indices;
}

/**
 * @brief Selects a random subset of indices up to a given container size
 * without replacement.
 */
std::vector<std::size_t> inline sample_indices_without_replacement(
    std::size_t size, std::size_t num_samples, std::mt19937_64 &gen) {

  if (num_samples > size) {
    throw std::invalid_argument(
        "Cannot sample more indices than the size of the container.");
  }

  auto indices = shuffled_iota(size, gen);
  return std::vector<std::size_t>(indices.begin(),
                                  indices.begin() + num_samples);
}

/**
 * @brief Selects a random subset of elements from a vector of elements without
 * replacement.
 */
template <typename T>
std::vector<T> sample_without_replacement(const std::vector<T> &data,
                                          std::size_t num_samples,
                                          std::mt19937_64 &gen) {

  if (num_samples > data.size()) {
    throw std::invalid_argument(
        "Cannot sample more elements than the size of the vector.");
  }

  auto indices = shuffled_iota(data.size(), gen);
  std::vector<T> samples;
  samples.reserve(num_samples);
  for (std::size_t i = 0; i < num_samples; ++i) {
    samples.push_back(data[indices[i]]);
  }

  return samples;
}

/**
 * @brief Constructs and returns a shuffled mapping from contiguous indices in
 * the range [0, size) to contiguous indices in the range [0, size).
 */
std::unordered_map<std::size_t, std::size_t> inline shuffled_mapping(
    std::size_t size, std::mt19937_64 &gen) {

  auto shuffled_indices = shuffled_iota(size, gen);
  std::unordered_map<std::size_t, std::size_t> mapping;
  for (std::size_t i = 0; i < size; ++i) {
    mapping[i] = shuffled_indices[i];
  }
  return mapping;
}

} // namespace utils
