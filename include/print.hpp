/**********************************************************************
 * @brief Library for printing common data structures, usually for debugging
 *purposes.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ========= Forward Declarations ===========

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &container);

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_set<T> &container);

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os, const std::map<K, V> &container);

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_map<K, V> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::stack<T> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::queue<T> &container);

template <typename T>
std::ostream &operator<<(std::ostream &os,
                         const std::priority_queue<T> &container);

// ========= Implementations ===========

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &container) {

  os << '[';
  int index = 0;
  for (const auto &x : container) {
    os << x;
    if (index++ < container.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &container) {
  os << '(' << container.first << ", " << container.second << ')';
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &container) {

  os << '{';
  int index = 0;
  for (const auto &x : container) {
    os << x;
    if (index++ < container.size() - 1) {
      os << ", ";
    }
  }
  os << "}";
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_set<T> &container) {

  os << '{';
  int index = 0;
  for (const auto &x : container) {
    os << x;
    if (index++ < container.size() - 1) {
      os << ", ";
    }
  }
  os << "}";
  return os;
}

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os, const std::map<K, V> &container) {
  os << "{";
  int index = 0;
  for (const auto &[key, value] : container) {
    os << key << ": " << value;
    if (index++ < container.size() - 1) {
      os << ", ";
    }
  }
  os << "}";
  return os;
}

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os,
                         const std::unordered_map<K, V> &container) {
  os << "{";
  int index = 0;
  for (const auto &[key, value] : container) {
    os << key << ": " << value;
    if (index++ < container.size() - 1) {
      os << ", ";
    }
  }
  os << "}";
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::stack<T> &container) {
  std::stack<T> temp = container;
  std::vector<T> vec;
  while (!temp.empty()) {
    vec.push_back(temp.top());
    temp.pop();
  }
  os << vec;
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::queue<T> &container) {
  std::queue<T> temp = container;
  std::vector<T> vec;
  while (!temp.empty()) {
    vec.push_back(temp.front());
    temp.pop();
  }
  os << vec;
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os,
                         const std::priority_queue<T> &container) {
  std::priority_queue<T> temp = container;
  std::vector<T> vec;
  while (!temp.empty()) {
    vec.push_back(temp.top());
    temp.pop();
  }
  os << vec;
  return os;
}
