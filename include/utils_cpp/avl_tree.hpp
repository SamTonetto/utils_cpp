/**********************************************************************
 * @brief AVL Tree
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <algorithm>
#include <functional>

namespace utils {

/**
 * An AVL Tree is a binary tree that is self-balancing, which means that
 * the height of the left and right subtrees of the root differ by at most 1.
 * This leads to a worst-case time complexity of O(log n) for lookups,
 * insertions, and deletions.
 */
template <typename T, typename Compare = std::less<T>>
class AVLTree {

public:
  AVLTree() : root{nullptr} {}
  ~AVLTree() { delete root; }
  AVLTree(const AVLTree &) = delete;
  AVLTree &operator=(const AVLTree &) = delete;
  AVLTree(AVLTree &&other) noexcept : root(std::move(other.root)) {}

  AVLTree &operator=(AVLTree &&other) noexcept {
    root = std::move(other.root);
    return *this;
  }

  AVLTree clone() const;

  bool insert(const T &key);

  bool contains(const T &key) const { return contains(root, key); }
  void erase(const T &key);

  std::size_t height() const { return height(root); }
  int balance() const { return root->balance; }

  T min() const;
  T max() const;

protected:
  struct Node {

    T key;
    int balance;
    Node *left, *right, *parent;

    Node(const T &key, Node *parent = nullptr)
        : key{key}, balance{0}, left{nullptr}, right{nullptr}, parent{parent} {}

    ~Node() {
      delete left;
      delete right;
    }
  };

  template <bool IsConst>
  class forward_iterator {

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using reference =
        typename std::conditional<IsConst, const Node &, Node &>::type;
    using pointer =
        typename std::conditional<IsConst, const Node *, Node *>::type;

    forward_iterator(pointer node) : node{node} {}

    reference operator*() const { return *node; }
    pointer operator->() const { return node; }

    forward_iterator &operator++() {
      if (node->right) {
        node = node->right;
        while (node->left) {
          node = node->left;
        }
      } else {
        pointer tmp = node->parent;
        while (tmp && node == tmp->right) {
          node = tmp;
          tmp = tmp->parent;
        }
        node = tmp;
      }
      return *this;
    }

    forward_iterator operator++(int) {
      forward_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const forward_iterator &other) const {
      return node == other.node;
    }
    bool operator!=(const forward_iterator &other) const {
      return node != other.node;
    }

  private:
    pointer node;
  };

  Node *root;
  Compare cmp;

public:
  using iterator = forward_iterator<false>;
  using const_iterator = forward_iterator<true>;

  iterator begin() {
    Node *node = root;
    while (node->left) {
      node = node->left;
    }
    return iterator(node);
  }

  iterator end() { return iterator(nullptr); }

  const_iterator begin() const {
    Node *node = root;
    while (node->left) {
      node = node->left;
    }
    return const_iterator(node);
  }

  const_iterator end() const { return const_iterator(nullptr); }

private:
  bool contains(Node *node, const T &key) const;

  Node *clone_node(const Node *node) const;

  /**
   * Example, with 'node' being '3' in the left tree.
   *
   *         3 (-2)                        2 (0)
   *        /                             / \
   *       2 (-1)       --------->       /   \
   *      /           Right rotation    1 (0) 3 (0)
   *     1 (0)
   *
   */
  void rotate_right(Node *node);

  /**
   * Example, with 'node' being '1' in the left tree.
   *
   *     1 (2)                             2 (0)
   *      \                               / \
   *       2 (1)       --------->        /   \
   *        \         Left rotation     1 (0) 3 (0)
   *         3 (0)
   *
   */
  void rotate_left(Node *node);

  std::size_t height(Node *node) const {
    if (!node) {
      return -1;
    }
    return 1 + std::max(height(node->left), height(node->right));
  }

  void set_balance(Node *node) {
    node->balance = height(node->right) - height(node->left);
  }

  void rebalance(Node *node);
};

// ==============
// IMPLEMENTATION
// ==============

template <typename T, typename Compare>
AVLTree<T, Compare> AVLTree<T, Compare>::clone() const {
  AVLTree new_tree;
  new_tree.root = clone_node(root);
  return new_tree;
}

template <typename T, typename Compare>
bool AVLTree<T, Compare>::insert(const T &key) {
  if (!root) {
    root = new Node(key);
    return true;
  }

  Node *node = root;
  Node *parent;

  while (true) {
    if (node->key == key) {
      return false;
    }

    parent = node;

    bool go_left = cmp(key, node->key);
    node = go_left ? node->left : node->right;

    if (!node) {
      if (go_left) {
        parent->left = new Node(key, parent);
      } else {
        parent->right = new Node(key, parent);
      }

      rebalance(parent);
      break;
    }
  }

  return true;
}

template <typename T, typename Compare>
bool AVLTree<T, Compare>::contains(Node *node, const T &key) const {

  if (!node) {
    return false;
  }

  if (cmp(key, node->key)) {
    return contains(node->left, key);
  } else if (cmp(node->key, key)) {
    return contains(node->right, key);
  } else {
    return true;
  }
}

template <typename T, typename Compare>
AVLTree<T, Compare>::Node *
AVLTree<T, Compare>::clone_node(const Node *node) const {
  if (!node) {
    return nullptr;
  }

  Node *new_node = new Node(node->key);
  new_node->left = clone_node(node->left);
  new_node->right = clone_node(node->right);

  if (new_node->left) {
    new_node->left->parent = new_node;
  }
  if (new_node->right) {
    new_node->right->parent = new_node;
  }

  return new_node;
}

template <typename T, typename Compare>
void AVLTree<T, Compare>::rotate_right(Node *node) {

  Node *tmp = node->left;
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
}

template <typename T, typename Compare>
void AVLTree<T, Compare>::rotate_left(Node *node) {
  Node *tmp = node->right;
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
}

template <typename T, typename Compare>
void AVLTree<T, Compare>::rebalance(Node *node) {

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
    root = node;
  }
}

template <typename T, typename Compare>
void AVLTree<T, Compare>::erase(const T &key) {
  if (!root) {
    return;
  }

  AVLTree<T, Compare>::Node *node = root, *parent = root,
                            *node_to_delete = nullptr, *child = root;

  while (child) {
    parent = node;
    node = child;

    child = cmp(key, node->key) ? node->left : node->right;
    if (key == node->key) {
      node_to_delete = node;
    }
  }

  if (node_to_delete) {
    node_to_delete->key = node->key;

    child = (node->left != nullptr) ? node->left : node->right;

    if (root->key == key) {
      root = child;
    } else {

      if (parent->left == node) {
        parent->left = child;
      } else {
        parent->right = child;
      }

      rebalance(parent);
    }
  }
}

template <typename T, typename Compare>
T AVLTree<T, Compare>::min() const {
  Node *node = root;
  while (node->left) {
    node = node->left;
  }
  return node->key;
}

template <typename T, typename Compare>
T AVLTree<T, Compare>::max() const {
  Node *node = root;
  while (node->right) {
    node = node->right;
  }
  return node->key;
}

} // namespace utils
