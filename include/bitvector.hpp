/**********************************************************************
 * @brief A bit vector class with a 1s iterator
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <bit>
#include <iostream>
#include <vector>

namespace utils {

class BitVector {
public:
  constexpr static std::size_t N = 64;

  using iter = std::vector<uint64_t>::const_iterator;

  struct ones_iterator {
    iter it;
    iter finish;

    uint64_t uint64_copy;

    std::size_t uint64_index;

    ones_iterator(iter it, iter finish) noexcept
        : it{it}, finish{finish}, uint64_copy{*it}, uint64_index{0} {

      while (uint64_copy == 0 && it != finish) {
        ++it;
        ++uint64_index;
        uint64_copy = *it;
      }
    }

    ones_iterator &operator++() noexcept {
      uint64_copy &= ~(1ULL << std::countr_zero(uint64_copy));

      while (uint64_copy == 0 && it != finish) {
        ++it;
        ++uint64_index;
        uint64_copy = *it;
      }
      return *this;
    }

    std::size_t operator*() const noexcept {
      return std::countr_zero(uint64_copy) + N * uint64_index;
    }

    bool operator==(const ones_iterator &other) const noexcept {
      return it == other.it;
    }

    bool operator!=(const ones_iterator &other) const noexcept {
      return it != other.it;
    }
  };

  ones_iterator begin() const noexcept {
    return ones_iterator{data_.begin(), data_.end()};
  }

  ones_iterator end() const noexcept {
    return ones_iterator{data_.end(), data_.end()};
  }

  struct bit_proxy {

    BitVector &bv;
    std::size_t index;

    bit_proxy &operator=(bool value) noexcept {
      bv.set(index, value);
      return *this;
    }
  };

  bit_proxy operator[](std::size_t index) noexcept {
    return bit_proxy{*this, index};
  }

  BitVector(std::size_t size)
      : size_{size}, data_{std::vector<uint64_t>((size + N - 1) / N, 0)} {}

  void set(std::size_t index) noexcept {
    data_[index / N] |= 1ULL << (index % N);
  }

  void reset() noexcept {
    for (auto &i : data_) {
      i = 0;
    }
  }

  void reset(std::size_t index) noexcept {
    data_[index / N] &= ~(1ULL << (index % N));
  }

  void set(std::size_t index, bool value) noexcept {
    data_[index / N] = (data_[index / N] & ~(1ULL << (index % N))) |
                       (static_cast<std::size_t>(value) << (index % N));
  }

  bool get(std::size_t index) const noexcept {
    return (data_[index / N] >> (index % N)) & 1ULL;
  }

  std::size_t popcount() const noexcept {
    std::size_t count = 0;
    for (auto i : data_) {
      count += std::popcount(i);
    }
    return count;
  }

  std::size_t size() const noexcept { return size_; }

private:
  std::size_t size_;
  std::vector<uint64_t> data_;
};

inline std::ostream &operator<<(std::ostream &os,
                                const BitVector::bit_proxy &p) noexcept {
  os << p.bv.get(p.index);
  return os;
}

} // namespace utils
