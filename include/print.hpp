/**********************************************************************
 * @brief Library for printing common data structures, usually for debugging
 *purposes.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <array>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "enum/enum_print.hpp"

// ========= IO manipulation ===========

namespace utils {

inline int newline_flag_index = std::ios_base::xalloc();

// unfortunately xalloc iwords always default to zero, so we imagine that a 0
// tab width actually corresponds to a default of 2.
inline int tab_width_index = std::ios_base::xalloc();

// Only used if os.iword(newline_flag_index) is true
inline thread_local std::size_t indent_amount = 0;

class escape_special_chars_buf : public std::streambuf {

protected:
  char process_char(char c) {
    switch (c) {
    case '\n':
      return 'n';
    case '\t':
      return 't';
    default:
      return 0;
    }
  }

  virtual int_type overflow(int_type c) {
    if (c != EOF) {
      char special = process_char(c);
      if (special) {
        original_buf->sputc('\\');
        original_buf->sputc(special);
      } else {
        original_buf->sputc(c);
      }
    }
    return c;
  }

  virtual int sync() { return original_buf->pubsync(); }

public:
  explicit escape_special_chars_buf(std::streambuf *buf) : original_buf(buf) {}
  std::streambuf *original_buf;
};

struct escape_special_chars {
  bool state;
  escape_special_chars(bool state) : state(state) {}

  friend std::ostream &operator<<(std::ostream &os,
                                  const escape_special_chars &esc) {
    if (esc.state) {
      static escape_special_chars_buf escape_buf(os.rdbuf());
      os.rdbuf(&escape_buf);
    } else {
      os.rdbuf(
          static_cast<escape_special_chars_buf *>(os.rdbuf())->original_buf);
    }
    return os;
  }
};

struct prettyprint {
  bool state;
  prettyprint(bool state) : state(state) {}

  friend std::ostream &operator<<(std::ostream &os, const prettyprint &pp) {
    os.iword(newline_flag_index) = pp.state;
    return os;
  }
};

// Set the with of one indentation level
// Since the default width of 2 is represented as 0, we have to subtract 2 from
// the user-inputted width.
inline std::ostream &tabwidth(std::ostream &os, std::size_t width) {
  os.iword(tab_width_index) = width - 2;
  return os;
}

template <typename T>
std::string to_binary_string(T x) {

  std::size_t nbits = sizeof(T) * 8;
  std::string result;
  result.reserve(nbits);
  for (std::size_t i = 0; i < nbits; ++i) {
    if (x & (static_cast<T>(1) << (nbits - i - 1))) {
      result += '1';
    } else {
      result += '0';
    }
  }
  return result;
}

} // namespace utils

// ========= Forward Declarations ===========

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &container);

template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &container);

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &container);

template <typename T, typename Compare = std::less<T>>
std::ostream &operator<<(std::ostream &os,
                         const std::set<T, Compare> &container);

template <typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<T>>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_set<T, Hash, KeyEqual> &container);

template <typename K, typename V, typename Compare = std::less<K>>
std::ostream &operator<<(std::ostream &os,
                         const std::map<K, V, Compare> &container);

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_map<K, V, Hash, KeyEqual> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::stack<T> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::queue<T> &container);

template <typename T, typename Container = std::vector<T>,
          typename Compare = std::less<typename Container::value_type>>
std::ostream &
operator<<(std::ostream &os,
           const std::priority_queue<T, Container, Compare> &container);

template <typename... T>
std::ostream &operator<<(std::ostream &os, const std::tuple<T...> &tup);

// ========= Implementations ===========

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "[]";
    return os;
  }

  os << indent << '[';

  std::size_t index = 0;
  for (const auto &x : container) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << x;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << x;
        }
      }
    } else {

      if (index == 0) {
        os << x;
      } else {
        os << ", " << x;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "]";
    } else {
      os << "]";
    }
  } else {
    os << "]";
  }

  return os;
}

template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "[]";
    return os;
  }

  os << indent << '[';

  std::size_t index = 0;
  for (const auto &x : container) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << x;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << x;
        }
      }
    } else {

      if (index == 0) {
        os << x;
      } else {
        os << ", " << x;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "]";
    } else {
      os << "]";
    }
  } else {
    os << "]";
  }

  return os;
}

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &container) {
  os << '(' << container.first << ", " << container.second << ')';
  return os;
}

template <typename TupleType, std::size_t... Index>
std::ostream &print_tuple_helper(std::ostream &os, const TupleType &tup,
                                 std::index_sequence<Index...>) {
  os << "(";
  (..., (os << (Index == 0 ? "" : ", ") << std::get<Index>(tup)));
  os << ")";
  return os;
}

template <typename... T>
std::ostream &operator<<(std::ostream &os, const std::tuple<T...> &tup) {
  return print_tuple_helper(os, tup, std::index_sequence_for<T...>{});
}

template <typename T, typename Compare>
std::ostream &operator<<(std::ostream &os,
                         const std::set<T, Compare> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "{}";
    return os;
  }

  os << indent << '{';
  std::size_t index = 0;
  for (const auto &x : container) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << x;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << x;
        }
      }
    } else {

      if (index == 0) {
        os << x;
      } else {
        os << ", " << x;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "}";
    } else {
      os << "}";
    }
  } else {
    os << "}";
  }

  return os;
}

template <typename T, typename Hash, typename KeyEqual>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_set<T, Hash, KeyEqual> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "{}";
    return os;
  }

  os << indent << '{';
  std::size_t index = 0;
  for (const auto &x : container) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << x;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << x;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << x;
        }
      }
    } else {

      if (index == 0) {
        os << x;
      } else {
        os << ", " << x;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "}";
    } else {
      os << "}";
    }
  } else {
    os << "}";
  }

  return os;
}

template <typename K, typename V, typename Compare>
std::ostream &operator<<(std::ostream &os,
                         const std::map<K, V, Compare> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "{}";
    return os;
  }

  os << indent << '{';
  std::size_t index = 0;
  for (const auto &[key, value] : container) {
    if constexpr (!std::is_scalar_v<K> || !std::is_scalar_v<V>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << key << ": " << value;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << key << ": " << value;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << key << ": " << value;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << key << ": " << value;
        }
      }
    } else {

      if (index == 0) {
        os << key << ": " << value;
      } else {
        os << ", " << key << ": " << value;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<K> || !std::is_scalar_v<V>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "}";
    } else {
      os << "}";
    }
  } else {
    os << "}";
  }

  return os;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_map<K, V, Hash, KeyEqual> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "{}";
    return os;
  }

  os << indent << '{';
  std::size_t index = 0;
  for (const auto &[key, value] : container) {
    if constexpr (!std::is_scalar_v<K> || !std::is_scalar_v<V>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << key << ": " << value;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << key << ": " << value;
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << key << ": " << value;
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << key << ": " << value;
        }
      }
    } else {

      if (index == 0) {
        os << key << ": " << value;
      } else {
        os << ", " << key << ": " << value;
      }
    }
    ++index;
  }

  if constexpr (!std::is_scalar_v<K> || !std::is_scalar_v<V>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "}";
    } else {
      os << "}";
    }
  } else {
    os << "}";
  }

  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::stack<T> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "[]";
    return os;
  }

  os << indent << '[';
  std::size_t index = 0;

  std::stack<T> temp(container);
  while (!temp.empty()) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.top();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << temp.top();
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.top();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << temp.top();
        }
      }
    } else {

      if (index == 0) {
        os << temp.top();
      } else {
        os << ", " << temp.top();
      }
    }
    temp.pop();
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "]";
    } else {
      os << "]";
    }
  } else {
    os << "]";
  }

  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::queue<T> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "[]";
    return os;
  }

  os << indent << '[';
  std::size_t index = 0;

  std::queue<T> temp(container);
  while (!temp.empty()) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.front();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << temp.front();
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.front();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << temp.front();
        }
      }
    } else {

      if (index == 0) {
        os << temp.front();
      } else {
        os << ", " << temp.front();
      }
    }
    temp.pop();
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "]";
    } else {
      os << "]";
    }
  } else {
    os << "]";
  }

  return os;
}

template <typename T, typename Container, typename Compare>
std::ostream &
operator<<(std::ostream &os,
           const std::priority_queue<T, Container, Compare> &container) {

  std::string indent = std::string(utils::indent_amount, ' ');

  if (container.size() == 0) {
    os << indent << "[]";
    return os;
  }

  os << indent << '[';
  std::size_t index = 0;

  std::priority_queue<T, Container, Compare> temp(container);

  while (!temp.empty()) {
    if constexpr (!std::is_scalar_v<T>) {

      if (index == 0) {
        if (os.iword(utils::newline_flag_index)) {
          os << "\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.top();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << temp.top();
        }
      } else {
        if (os.iword(utils::newline_flag_index)) {
          os << ",\n";
          utils::indent_amount += os.iword(utils::tab_width_index) + 2;
          os << indent << temp.top();
          utils::indent_amount -= os.iword(utils::tab_width_index) + 2;
        } else {
          os << ", " << temp.top();
        }
      }
    } else {

      if (index == 0) {
        os << temp.top();
      } else {
        os << ", " << temp.top();
      }
    }
    temp.pop();
    ++index;
  }

  if constexpr (!std::is_scalar_v<T>) {
    if (os.iword(utils::newline_flag_index)) {
      os << "\n" << indent << "]";
    } else {
      os << "]";
    }
  } else {
    os << "]";
  }

  return os;
}
