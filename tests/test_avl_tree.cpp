#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "avl_tree.hpp"

TEST_CASE("Testing AVLTree functionality") {
  utils::AVLTree<int> tree;

  SUBCASE("insert and contains") {
    tree.insert(3);
    tree.insert(1);
    tree.insert(4);
    tree.insert(1);
    tree.insert(5);
    tree.insert(9);
    tree.insert(2);
    tree.insert(6);

    CHECK(tree.contains(3) == true);
    CHECK(tree.contains(4) == true);
    CHECK(tree.contains(9) == true);
    CHECK(tree.contains(7) == false);
  }

  SUBCASE("height and balance") {
    tree.insert(3);
    tree.insert(2);
    tree.insert(1);
    tree.insert(4);
    tree.insert(5);

    CHECK(tree.height() >= 1);
    CHECK(tree.height() <= 3); // Ensuring the tree is balanced
    CHECK((tree.balance() >= -1 && tree.balance() <= 1));
  }

  SUBCASE("erase") {
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);

    tree.erase(3);
    CHECK(tree.contains(3) == false);

    tree.erase(5);
    CHECK(tree.contains(5) == false);
  }

  SUBCASE("clone") {
    tree.insert(1);
    tree.insert(3);
    tree.insert(2);

    auto cloned = tree.clone();
    CHECK(cloned.contains(1) == true);
    CHECK(cloned.contains(3) == true);
    CHECK(cloned.contains(2) == true);

    cloned.erase(2);
    CHECK(cloned.contains(2) == false);
    CHECK(tree.contains(2) == true); // Ensuring original tree is unaffected
  }

  SUBCASE("forward iteration") {
    tree.insert(1);
    tree.insert(3);
    tree.insert(2);

    auto it = tree.begin();
    CHECK(it->key == 1);
    ++it;
    CHECK(it->key == 2);
    ++it;
    CHECK(it->key == 3);
    ++it;
    CHECK(it == tree.end());
  }
}
