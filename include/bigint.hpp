#pragma once

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace utils {
/// Class handling operations on arbitrary-precision integers.
///
/// Adapted from https://gist.github.com/ar-pa/957297fb3f88996ead11 with
/// slight modifications.

/// Basic idea is to store the big integer in a vector where every element
/// is base 1000000000, such that the least-significant value is the 0th
/// element. Any arithmetic operations are performed on this vector.
///
/// Some changes:
/// - Overflow protection converting to and from long long
/// - Implementing + in terms of += instead of the other way around (saves
/// copying)
class BigInt {
public:
  /// Convert an 'a' vector from one base to another.
  ///
  /// @param a The vector-representation to be converted.
  /// @param old_digits The max-digits in old base.
  /// @param new_digits The max-digits in new base.
  static std::vector<int> convert_base(const std::vector<int> &a,
                                       std::size_t old_digits,
                                       std::size_t new_digits);

public:
  using vll = std::vector<long long>;

  /// Zero-initialize.
  BigInt();

  /// Initialize from long long.
  BigInt(long long v);

  /// Initialize from numerical string.
  BigInt(const std::string &s);

  int size() const;

  bool is_zero() const;
  bool is_even() const;
  bool is_odd() const;
  BigInt abs() const;
  BigInt operator-() const;

  std::string to_string();

  /// From binary vector where the most-significant bit is 0th index.
  void from_long_long(long long v);
  void from_binary_vector(const std::vector<int> &v);
  void from_string(const std::string &s);

  /// Convert to long long.
  ///
  /// @warning There are asserts to check when this is impossible, but
  /// make sure the number can fit inside a long long anyway!
  long long to_ll() const;

  /// Map the 'a' vector to a vector-representation with the 0th index
  /// being the most-significant bit instead of the least-significant, and
  /// in a new base.
  ///
  /// For example,
  ///   auto v = this->to_big_endian_vector(2);
  /// would map 'a' to a base-2 vector-representation that would read the
  /// same as a normal binary number.
  std::vector<int> to_big_endian_vector(int new_base) const;

  /// Pop off redundant zero elements from the 'a' vector, reducing its
  /// size in the process.
  void trim();

  BigInt &operator=(long long v);
  BigInt &operator=(const std::string &s);
  BigInt &operator+=(const BigInt &v);
  BigInt &operator-=(const BigInt &v);
  BigInt &operator*=(const BigInt &v);
  BigInt &operator*=(int v);
  BigInt &operator*=(long long v);
  BigInt &operator/=(const BigInt &v);
  BigInt &operator/=(int v);

  friend BigInt operator+(const BigInt &u, const BigInt &v);
  friend BigInt operator-(const BigInt &u, const BigInt &v);
  friend BigInt operator*(const BigInt &u, const BigInt &v);
  friend BigInt operator*(const BigInt &u, long long v);
  friend BigInt operator/(const BigInt &u, const BigInt &v);
  friend BigInt operator/(const BigInt &u, int v);
  friend BigInt operator%(const BigInt &u, const BigInt &v);

  /// Remainder after division by integer.
  friend int operator%(const BigInt &u, int v);

  /// Exponentiation of one BigInt by another.
  friend BigInt operator^(const BigInt &u, const BigInt &v);

  friend bool operator<(const BigInt &u, const BigInt &v);
  friend bool operator>(const BigInt &u, const BigInt &v);
  friend bool operator<=(const BigInt &u, const BigInt &v);
  friend bool operator>=(const BigInt &u, const BigInt &v);

  friend bool operator==(const BigInt &u, const BigInt &v);
  friend bool operator!=(const BigInt &u, const BigInt &v);

  friend std::ostream &operator<<(std::ostream &os, const BigInt &v);
  friend std::istream &operator>>(std::istream &is, BigInt &v);

  /// Find the greatest common divisor of two BigInts.
  friend BigInt gcd(const BigInt &u, const BigInt &v);

  /// Find the least common multiple of two BigInts.
  friend BigInt lcm(const BigInt &u, const BigInt &v);

  /// Find the quotient and remainder upon division of BigInt a1 by BigInt
  /// b1.
  ///
  /// The output is a pair of the form (quotient, remainder).
  friend std::pair<BigInt, BigInt> divmod(const BigInt &a1, const BigInt &b1);

  /// Efficient algorithm for multiplying two large numbers encoded in
  /// 'a'-like vector-representations.
  ///
  /// @param a vector-representation of number with 0th index being the
  /// least-significant bit.
  /// @param b vector-representation of number with 0th index being the
  /// least-significant digit.
  /// @note Used as a subroutine for operator*(const BigInt&, const
  /// BigInt&).
  /// @note The resultant vector may not 'fit' inside base limits. This is
  /// why in operator*(const BigInt&, const BigInt&), the 'a' vectors are
  /// first converted to base 1-million so that multiplication won't
  /// overflow.
  static vll karatsuba_multiply(const vll &a, const vll &b);

  /// A base-'Bigint::base' representation of a large number.
  ///
  /// The least significant bit is the 0th index, so it is read in the
  /// opposite direction to how numbers are normally written.
  std::vector<int> a;

  /// The sign of the number, either +1 or -1.
  int sign;

  /// The base of numbers of 'a' (e.g. where base 10 is decimal).
  ///
  /// A higher base means more efficient memory usage, since storing large
  /// numbers will require fewer 32-bit integers.
  /// @warning Make sure base is a multiple of 10, or bad things could
  /// happen.
  static constexpr int base = 1000000000;

  /// This is simply the maximum number of digits any element can take in
  /// the given base.
  ///
  /// @warning Ensure that if base = 100,1000,10000,100000,...,
  /// base_digits = 2,3,4,5,...
  static constexpr int base_digits = 9;
};

// IMPLEMENTATION

inline BigInt::BigInt() : sign{1} {}

inline BigInt::BigInt(const std::string &s) { from_string(s); }

inline BigInt::BigInt(long long v) { from_long_long(v); }

inline int BigInt::size() const {
  if (a.empty())
    return 0;

  int ans = (a.size() - 1) * base_digits;
  int ca = a.back();

  while (ca) {
    ans++;
    ca /= 10;
  }

  return ans;
}

inline bool BigInt::is_zero() const {
  return a.empty() || (a.size() == 1 && !a[0]);
}

inline bool BigInt::is_even() const { return !(a[0] % 2); }

inline bool BigInt::is_odd() const { return (a[0] % 2); }

inline BigInt BigInt::abs() const {
  BigInt res = *this;
  res.sign *= res.sign;
  return res;
}

inline BigInt BigInt::operator-() const {
  BigInt res = *this;
  res.sign = -sign;
  return res;
}

inline std::string BigInt::to_string() {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

inline void BigInt::from_long_long(long long v) {
  if (v == std::numeric_limits<long long>::min()) {
    sign = -1;
    a = {854775808, 223372036, 9};
  } else {
    sign = 1;
    a.clear();

    if (v < 0) {
      sign = -1;
      v = -v;
    }

    for (; v > 0; v = v / base)
      a.push_back(v % base);
  }
}

inline void BigInt::from_binary_vector(const std::vector<int> &v) {
  from_long_long(0);

  BigInt power_of_2(1);

  for (std::size_t i = v.size() - 1; i < v.size(); --i) {
    if (v[i] == 1)
      *this += power_of_2;
    power_of_2 *= 2;
  }
}

inline void BigInt::from_string(const std::string &s) {
  sign = 1;
  a.clear();

  std::size_t pos = 0;
  while (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) {
    if (s[pos] == '-')
      sign = -sign;
    pos++;
  }

  // backwards iteration
  for (std::size_t i = s.size() - 1; i >= pos && i < s.size();
       i -= base_digits) {
    int x = 0;

    std::size_t j_start =
        (i + 1 < base_digits) ? pos : std::max(pos, i - base_digits + 1);

    for (std::size_t j = j_start; j <= i; j++) {
      x = x * 10 + s[j] - '0';
    }

    a.push_back(x);
  }

  trim();
}

inline long long BigInt::to_ll() const {
  long long res = 0;

  // backwards iteration
  for (std::size_t i = a.size() - 1; i < a.size(); i--) {
#ifndef NDEBUG
    long long overflow_test;
    assert(!__builtin_smulll_overflow(res, base, &overflow_test));
#endif
    res *= base;
#ifndef NDEBUG
    assert(!__builtin_saddll_overflow(res, base, &overflow_test));
#endif
    res += a[i];
  }
  return res * sign;
}

inline std::vector<int> BigInt::to_big_endian_vector(int new_base) const {
  if (is_zero())
    return {0};

  std::vector<int> res;
  BigInt a_copy = *this;

  while (!a_copy.is_zero()) {
    res.emplace_back(a_copy % new_base);
    a_copy /= new_base;
  }
  std::reverse(res.begin(), res.end());
  return res;
}

inline void BigInt::trim() {
  while (!a.empty() && !a.back())
    a.pop_back();
  if (a.empty())
    sign = 1;
}

inline BigInt &BigInt::operator=(long long v) {
  BigInt temp(v);
  std::swap(*this, temp);
  return *this;
}

inline BigInt &BigInt::operator=(const std::string &s) {
  BigInt temp(s);
  std::swap(*this, temp);
  return *this;
}

inline BigInt &BigInt::operator+=(const BigInt &v) {
  if (sign == v.sign) {
    for (std::size_t i = 0, carry = 0;
         i < std::max(a.size(), v.a.size()) || carry; ++i) {
      if (i == a.size())
        a.push_back(0);

      a[i] += carry + (i < v.a.size() ? v.a[i] : 0);
      carry = a[i] >= base;
      if (carry)
        a[i] -= base;
    }
  } else {
    *this -= (-v);
  }
  return *this;
}

inline BigInt &BigInt::operator-=(const BigInt &v) {
  if (sign == v.sign) {
    if (abs() >= v.abs()) {
      for (std::size_t i = 0, carry = 0; i < v.a.size() || carry; ++i) {
        a[i] -= carry + (i < v.a.size() ? v.a[i] : 0);
        carry = (a[i] < 0);
        if (carry)
          a[i] += base;
      }
      trim();
    } else {
      for (std::size_t i = 0, carry = 0; i < v.a.size() || carry; ++i) {
        if (i == a.size())
          a.push_back(0);
        a[i] = v.a[i] - carry - a[i];
        carry = a[i] < 0;
        if (carry)
          a[i] += base;
      }
      sign = -sign;
    }
  } else {
    *this += (-v);
  }
  return *this;
}

inline BigInt &BigInt::operator*=(const BigInt &v) {
  *this = (*this) * v;
  return *this;
}

inline BigInt &BigInt::operator*=(int v) {
  if (v < 0) {
    sign = -sign;
    v = -v;
  }
  for (std::size_t i = 0, carry = 0; i < a.size() || carry; ++i) {
    if (i == a.size())
      a.push_back(0);
    long long cur = a[i] * static_cast<long long>(v) + carry;
    carry = static_cast<std::size_t>(cur / base);
    a[i] = static_cast<int>(cur % base);
  }
  trim();
  return *this;
}

inline BigInt &BigInt::operator*=(long long v) {
  if (v < 0) {
    sign = -sign;
    v = -v;
  }
  if (v > base) {
    *this = (*this) * (v / base) * base + (*this) * (v % base);
    return *this;
  }
  for (std::size_t i = 0, carry = 0; i < a.size() || carry; ++i) {
    if (i == a.size())
      a.push_back(0);
    long long cur = a[i] * v + carry;
    carry = static_cast<std::size_t>(cur / base);
    a[i] = static_cast<int>(cur % base);
  }
  trim();
  return *this;
}

inline BigInt &BigInt::operator/=(const BigInt &v) {
  *this = (*this) / v;
  return *this;
}

inline BigInt &BigInt::operator/=(int v) {
  if (v < 0) {
    sign = -sign;
    v = -v;
  }
  for (int i = static_cast<int>(a.size()) - 1, rem = 0; i >= 0; --i) {
    long long cur = a[i] + rem * static_cast<long long>(base);
    a[i] = static_cast<int>(cur / v);
    rem = static_cast<int>(cur % v);
  }
  trim();
  return *this;
}

inline BigInt operator+(const BigInt &u, const BigInt &v) {
  BigInt res = u;
  res += v;
  return res;
}

inline BigInt operator-(const BigInt &u, const BigInt &v) {
  BigInt res = u;
  res -= v;
  return res;
}

inline BigInt operator*(const BigInt &u, const BigInt &v) {
  // Convert to base-6 vectors so that basic integer multiplication won't
  // overflow.
  std::vector<int> a6 = BigInt::convert_base(u.a, BigInt::base_digits, 6);
  std::vector<int> b6 = BigInt::convert_base(v.a, BigInt::base_digits, 6);

  BigInt::vll a(a6.begin(), a6.end());
  BigInt::vll b(b6.begin(), b6.end());

  // Make sure both vectors are the same size.
  while (a.size() < b.size())
    a.push_back(0);
  while (b.size() < a.size())
    b.push_back(0);
  while (a.size() & (a.size() - 1)) {
    a.push_back(0);
    b.push_back(0);
  }

  BigInt::vll c = BigInt::karatsuba_multiply(a, b);
  BigInt res;
  res.sign = u.sign * v.sign;
  for (std::size_t i = 0, carry = 0; i < c.size(); ++i) {
    long long cur = c[i] + carry;
    res.a.push_back(static_cast<int>(cur % 1000000));
    carry = static_cast<std::size_t>(cur / 1000000);
  }
  res.a = BigInt::convert_base(res.a, 6, BigInt::base_digits);
  res.trim();
  return res;
}

inline BigInt operator*(const BigInt &u, long long v) {
  BigInt res = u;
  res *= v;
  return res;
}

inline BigInt operator/(const BigInt &u, const BigInt &v) {
  return divmod(u, v).first;
}

inline BigInt operator/(const BigInt &u, int v) {
  BigInt res = u;
  res /= v;
  return res;
}

inline BigInt operator%(const BigInt &u, const BigInt &v) {
  return divmod(u, v).second;
}

inline int operator%(const BigInt &u, int v) {
  if (v < 0)
    v = -v;
  int m = 0;

  // backwards iteration
  for (std::size_t i = u.a.size() - 1; i < u.a.size(); --i)
    m = (u.a[i] + m * static_cast<long long>(BigInt::base)) % v;
  return m * u.sign;
}

inline BigInt operator^(const BigInt &u, const BigInt &v) {
  BigInt ans = 1, a = u, b = v;
  while (!b.is_zero()) {
    if (b % 2 == 0)
      ans *= a;
    a *= a;
    b /= 2;
  }
  return ans;
}

inline bool operator<(const BigInt &u, const BigInt &v) {
  if (u.sign != v.sign)
    return u.sign < v.sign;
  if (u.a.size() != v.a.size())
    return u.a.size() * u.sign < v.a.size() * v.sign;

  // backwards iteration
  for (std::size_t i = u.a.size() - 1; i < u.a.size(); i--)
    if (u.a[i] != v.a[i])
      return u.a[i] * u.sign < v.a[i] * v.sign;
  return false;
}

inline bool operator>(const BigInt &u, const BigInt &v) { return v < u; }

inline bool operator<=(const BigInt &u, const BigInt &v) { return !(u > v); }

inline bool operator>=(const BigInt &u, const BigInt &v) { return !(u < v); }

inline bool operator==(const BigInt &u, const BigInt &v) {
  return !(u < v) && !(u > v);
}

inline bool operator!=(const BigInt &u, const BigInt &v) {
  return (u < v) || (u > v);
}

inline std::ostream &operator<<(std::ostream &os, const BigInt &v) {
  if (v.sign == -1)
    os << '-';
  os << (v.a.empty() ? 0 : v.a.back());

  // backwards iteration
  for (std::size_t i = v.a.size() - 2; i < v.a.size(); --i)
    os << std::setw(BigInt::base_digits) << std::setfill('0') << v.a[i];
  return os;
}

inline std::istream &operator>>(std::istream &is, BigInt &v) {
  std::string s;
  is >> s;
  v.from_string(s);
  return is;
}

inline BigInt gcd(const BigInt &u, const BigInt &v) {
  return v.is_zero() ? u : gcd(v, u % v);
}

inline BigInt lcm(const BigInt &u, const BigInt &v) {
  return u / gcd(u, v) * v;
}

inline std::vector<int> BigInt::convert_base(const std::vector<int> &a,
                                             std::size_t old_digits,
                                             std::size_t new_digits) {
  std::vector<long long> p(std::max(old_digits, new_digits) + 1);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
  p[0] = 1;
#pragma GCC diagnostic pop

  for (std::size_t i = 1; i < p.size(); ++i)
    p[i] = p[i - 1] * 10;

  std::vector<int> res;
  long long cur = 0;
  std::size_t cur_digits = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    cur += a[i] * p[cur_digits];
    cur_digits += old_digits;
    while (cur_digits >= new_digits) {
      res.push_back(static_cast<int>(cur % p[new_digits]));
      cur /= p[new_digits];
      cur_digits -= new_digits;
    }
  }
  res.push_back(static_cast<int>(cur));
  while (!res.empty() && !res.back())
    res.pop_back();
  return res;
}

inline std::pair<BigInt, BigInt> divmod(const BigInt &a1, const BigInt &b1) {
  int norm = BigInt::base / (b1.a.back() + 1);

  BigInt a = a1.abs() * norm;
  BigInt b = b1.abs() * norm;
  BigInt q, r;
  q.a.resize(a.a.size());

  // backwards iteration
  for (std::size_t i = a.a.size() - 1; i < a.a.size(); --i) {
    r *= BigInt::base;
    r += a.a[i];
    int s1 = r.a.size() <= b.a.size() ? 0 : r.a[b.a.size()];
    int s2 = r.a.size() <= b.a.size() - 1 ? 0 : r.a[b.a.size() - 1];
    int d = (static_cast<long long>(BigInt::base) * s1 + s2) / b.a.back();
    r -= b * d;
    while (r < 0) {
      r += b;
      --d;
    }
    q.a[i] = d;
  }

  q.sign = a1.sign * b1.sign;
  r.sign = a1.sign;
  q.trim();
  r.trim();

  return std::make_pair(q, r / norm);
}

inline BigInt::vll BigInt::karatsuba_multiply(const BigInt::vll &a,
                                              const BigInt::vll &b) {
  std::size_t n = a.size();
  BigInt::vll res(n + n);

  if (n <= 32) {
    for (std::size_t i = 0; i < n; ++i)
      for (std::size_t j = 0; j < n; ++j)
        res[i + j] += a[i] * b[j];
    return res;
  }

  std::size_t k = (n >> 1);
  BigInt::vll a1(a.begin(), a.begin() + k);
  BigInt::vll a2(a.begin() + k, a.end());
  BigInt::vll b1(b.begin(), b.begin() + k);
  BigInt::vll b2(b.begin() + k, b.end());

  BigInt::vll a1b1 = karatsuba_multiply(a1, b1);
  BigInt::vll a2b2 = karatsuba_multiply(a2, b2);

  for (std::size_t i = 0; i < k; ++i)
    a2[i] += a1[i];
  for (std::size_t i = 0; i < k; ++i)
    b2[i] += b1[i];

  BigInt::vll r = karatsuba_multiply(a2, b2);
  for (std::size_t i = 0; i < a1b1.size(); ++i)
    r[i] -= a1b1[i];
  for (std::size_t i = 0; i < a2b2.size(); ++i)
    r[i] -= a2b2[i];

  for (std::size_t i = 0; i < r.size(); ++i)
    res[i + k] += r[i];
  for (std::size_t i = 0; i < a1b1.size(); ++i)
    res[i] += a1b1[i];
  for (std::size_t i = 0; i < a2b2.size(); ++i)
    res[i + n] += a2b2[i];

  return res;
}

} // namespace utils
