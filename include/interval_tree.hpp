/**********************************************************************
 * @brief Interval Tree implementation
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

namespace utils {

/**
 * Interval trees allow you to efficiently check if a value is contained in an
 * interval, or to check overlap between an interval and other intervals (not
 * yet implemented).
 */
template <typename T, typename Compare = std::less<T>>
class IntervalTree {

public:
  struct IntervalNode {

    T low, high;
    int balance;
    IntervalNode *left, *right, *parent;

    T max_high; // tracks maximum 'high' value in the subtree starting at this
                // node.

    IntervalNode(const T &low, const T &high, IntervalNode *parent = nullptr)
        : low{low}, high{high}, balance{0}, left{nullptr}, right{nullptr},
          parent{parent}, max_high{high} {}

    ~IntervalNode() {
      delete left;
      delete right;
    }
  };

private:
  IntervalNode *root;
  Compare cmp;

public:
  IntervalTree() : root{nullptr} {}
  ~IntervalTree() { delete root; }

  IntervalTree(const IntervalTree &) = delete;
  IntervalTree &operator=(const IntervalTree &) = delete;
  IntervalTree(IntervalTree &&other) noexcept : root{std::move(other.root)} {}

  IntervalTree &operator=(IntervalTree &&other) noexcept {
    root = std::move(other.root);
    return *this;
  }

  bool contains(const T &point) const { return contains(root, point); }
  bool insert(const T &low, const T &high);

  bool insert(const std::pair<T, T> &interval) {
    return insert(interval.first, interval.second);
  }

  void erase(const T &low, const T &high);

  std::size_t height() const { return height(root); }
  int balance() const { return root->balance; }

private:
  bool contains(IntervalNode *node, const T &point) const;

  void rotate_left(IntervalNode *node);
  void rotate_right(IntervalNode *node);

  std::size_t height(IntervalNode *node) const {
    if (!node) {
      return -1;
    }
    return 1 + std::max(height(node->left), height(node->right));
  }

  void set_balance(IntervalNode *node) {
    node->balance = height(node->right) - height(node->left);
  }

  void rebalance(IntervalNode *node);

  void set_max_high(IntervalNode *node);
};

template <typename T, typename Compare>
bool IntervalTree<T, Compare>::contains(IntervalNode *node,
                                        const T &point) const {
  if (!node) {
    return false;
  }

  if (!(cmp(point, node->low)) && !(cmp(node->high, point))) {
    return true;
  }

  if (node->left && !(cmp(node->left->max_high, point))) {
    return contains(node->left, point);
  }

  return contains(node->right, point);
}

template <typename T, typename Compare>
bool IntervalTree<T, Compare>::insert(const T &low, const T &high) {
  if (!root) {
    root = new IntervalNode(low, high);
    return true;
  }

  IntervalNode *node = root;
  IntervalNode *parent;

  while (true) {

    // duplicate
    if (node->low == low && node->high == high) {
      return false;
    }

    parent = node;

    bool go_left = cmp(low, node->low);
    node = go_left ? node->left : node->right;

    if (!node) {
      if (go_left) {
        parent->left = new IntervalNode(low, high, parent);
      } else {
        parent->right = new IntervalNode(low, high, parent);
      }

      rebalance(parent);
      break;
    }
  }

  return true;
}

template <typename T, typename Compare>
void IntervalTree<T, Compare>::set_max_high(IntervalNode *node) {

  if (node) {
    node->max_high = node->high;
    if (node->left) {
      node->max_high = std::max(node->max_high, node->left->max_high);
    }
    if (node->right) {
      node->max_high = std::max(node->max_high, node->right->max_high);
    }
  } else {
    node->max_high = std::numeric_limits<T>::min();
  }
}

template <typename T, typename Compare>
void IntervalTree<T, Compare>::rotate_right(IntervalNode *node) {

  IntervalNode *tmp = node->left;
  tmp->parent = node->parent;
  node->left = tmp->right;

  if (node->left) {
    node->left->parent = node;
  }

  tmp->right = node;
  node->parent = tmp;

  if (tmp->parent) {
    if (tmp->parent->right == node) {
      tmp->parent->right = tmp;
    } else {
      tmp->parent->left = tmp;
    }
  }

  set_balance(node);
  set_balance(tmp);

  // tmp becomes parent of node, so we must update node's max_high first.
  // at least I think so...
  set_max_high(node);
  set_max_high(tmp);
}

template <typename T, typename Compare>
void IntervalTree<T, Compare>::rotate_left(IntervalNode *node) {
  IntervalNode *tmp = node->right;
  tmp->parent = node->parent;
  node->right = tmp->left;

  if (node->right) {
    node->right->parent = node;
  }

  tmp->left = node;
  node->parent = tmp;

  if (tmp->parent) {
    if (tmp->parent->right == node) {
      tmp->parent->right = tmp;
    } else {
      tmp->parent->left = tmp;
    }
  }

  set_balance(node);
  set_balance(tmp);

  // tmp becomes parent of node, so we must update node's max_high first
  // ... at least I think so
  set_max_high(node);
  set_max_high(tmp);
}

template <typename T, typename Compare>
void IntervalTree<T, Compare>::rebalance(IntervalNode *node) {

  set_balance(node);

  if (node->balance == -2) {
    if (height(node->left->left) >= height(node->left->right)) {
      rotate_right(node);
    } else {
      rotate_left(node->left);
      rotate_right(node);
    }
  } else if (node->balance == 2) {
    if (height(node->right->right) >= height(node->right->left)) {
      rotate_left(node);
    } else {
      rotate_right(node->right);
      rotate_left(node);
    }
  }

  if (node->parent) {
    rebalance(node->parent);
  } else {
    root = std::move(node);
  }
}

} // namespace utils
