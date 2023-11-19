/**
 * Segment Tree datastructure.
 * !!!
 * UNFINISHED
 * !!!
 */

#pragma once

#include <cstddef>
#include <vector>

namespace utils {

/**
 * A Segment Tree is an efficient data structure for performing range queries
 * (e.g. what is the min, max, mean etc. of the elements in range [l,r)) for a
 * fixed size array. The time complexity for both point updates and range
 * queries is O(log n).
 *
 * Normally, Segment Trees are implemented using binary trees, but here we use
 * an array and bitwise operations, which is more efficient in terms of both
 * memory and speed.
 */
template <typename T, typename Fn>
class SegmentTree {

  Fn combine;
  std::size_t n; // size of original array
  std::vector<T> arr;

public:
  SegmentTree() = default;

  SegmentTree(const std::vector<T> &arr)
      : n{arr.size()}, arr{std::vector<T>(2 * n)} {

    for (int i = 0; i < n; ++i) {
      arr[i + n] = arr[i];
    }

    for (int i = n - 1; i > 0; --i) {
      arr[i] = combine(arr[i << 1], arr[i << 1 | 1]);
    }
  }

  /**
   * Updates an index of the original array, according to the combine function.
   */
  void modify(std::size_t i, const T &value) {
    i += n;
    arr[i] = value;

    while (i > 1) {
      i >>= 1;
      arr[i] = combine(arr[i << 1], arr[i << 1 | 1]);
    }
  }

  /**
   * Performs a query on the index range [l, r) of the original array,
   * according to the combine function.
   */
  T query(std::size_t l, std::size_t r) {
    T resl, resr;
    for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
      if (l & 1) {
        resl = combine(resl, arr[l]);
        ++l;
      }
      if (r & 1) {
        --r;
        resl = combine(arr[r], resr);
      }
    }
    return combine(resl, resr);
  }
};

} // namespace utils
