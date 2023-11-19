#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "interval_tree.hpp"

TEST_CASE("test insert") {

  utils::IntervalTree<int> tree;

  tree.insert({0, 3});
  tree.insert({5, 8});
  tree.insert({6, 10});
  tree.insert({8, 9});

  CHECK(tree.contains(0));
  CHECK(tree.contains(1));
  CHECK(tree.contains(2));
  CHECK(tree.contains(6));

  CHECK_FALSE(tree.contains(4));
  CHECK_FALSE(tree.contains(11));

  CHECK((tree.balance() >= -1 && tree.balance() <= 1));
}
