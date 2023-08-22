/**********************************************************************
 * @brief Various numpy features, C++-ified.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <cstddef>
#include <stdexcept>

namespace utils {

/**
 * Just create a basic range [start,stop) with optional step size, like
 * np.arange. Start, stop, and step must be the same type. In the examples,
 * aggregate initialization is used, but you can also use parentheses.
 * @example
 * for (auto i : arange{10}) { ... } // 0,1,2,3,4,5,6,7,8,9
 * for (auto i : arange{1, 10}) { ... } // 1,2,3,4,5,6,7,8,9
 * for (auto i : arange{1, 10, 2}) { ... } // 1,3,5,7,9
 * for (auto i : arange{0, 10, 2}) { ... } // 0,2,4,6,8
 * for (auto i : arange{1.5, 6.0, 1.5}) { ... } // 1.5,3.0,4.5
 * @example
 * auto range = arange{1.0, 10.0, 0.5}; // OK - double
 * auto range = arange{1, 10, 1};       // OK - int
 * auto range = arange{1, 10, 0.5};     // ERROR - different types
 */
template <typename T>
struct arange {
  T start;
  T stop;
  T step;

  std::size_t size;

  arange(T start, T stop, T step = 1)
      : start{start}, stop{stop}, step{step},
        size{static_cast<std::size_t>((stop - start) / step)} {

    if (step == 0) {
      throw std::invalid_argument("arange step cannot be zero");
    }
  }

  arange(T stop)
      : start{0}, stop{stop}, step{1},
        size{static_cast<std::size_t>((stop - start) / step)} {

    if (step == 0) {
      throw std::invalid_argument("arange step cannot be zero");
    }
  }

  T operator[](std::size_t i) const {
    if (i >= size) {
      throw std::out_of_range("arange index out of range");
    }
    return start + i * step;
  }

  struct iterator {
    T value, step;

    T operator*() const { return value; }
    iterator &operator++() {
      value += step;
      return *this;
    }
    bool operator!=(const iterator &other) const {
      return (step > 0) ? (value < other.value) : (value > other.value);
    }
  };

  iterator begin() const { return {start, step}; }
  iterator end() const { return {stop, step}; }
};

} // namespace utils
