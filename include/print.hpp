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

template <typename T, typename Compare>
std::ostream &operator<<(std::ostream &os,
                         const std::set<T, Compare> &container) {

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

template <typename T, typename Hash, typename KeyEqual>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_set<T, Hash, KeyEqual> &container) {

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

template <typename K, typename V, typename Compare>
std::ostream &operator<<(std::ostream &os,
                         const std::map<K, V, Compare> &container) {

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

template <typename K, typename V, typename Hash, typename KeyEqual>
std::ostream &
operator<<(std::ostream &os,
           const std::unordered_map<K, V, Hash, KeyEqual> &container) {
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

template <typename T, typename Container, typename Compare>
std::ostream &
operator<<(std::ostream &os,
           const std::priority_queue<T, Container, Compare> &container) {
  std::priority_queue<T> temp = container;
  std::vector<T> vec;
  while (!temp.empty()) {
    vec.push_back(temp.top());
    temp.pop();
  }
  os << vec;
  return os;
}
