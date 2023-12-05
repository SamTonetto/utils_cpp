#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "python.hpp"

#include <sstream>
#include <vector>

void foo(std::stringstream &ss, const std::vector<std::string> &v) {

  for (const auto &[i, s] : utils::enumerate(v)) {
    ss << i << " " << s.get() << "\n";
  }
}

TEST_CASE("test enumerate") {

  std::vector<std::string> v = {"a", "b", "c"};

  std::stringstream ss;

  for (const auto &[i, s] : utils::enumerate(v)) {
    ss << i << " " << s.get() << "\n";
  }

  // 'const auto' version has warning about creating copy, and due to -Werror
  // it fails to compile, but we don't really care about this warning anyway
  //
  //  for (const auto [i, s] : utils::enumerate(v)) {
  //    std::string s2 = s;
  //    ss << i << " " << s2 << "\n";
  //  }

  for (auto [i, s] : utils::enumerate(v)) {
    ss << i << " " << s.get() << "\n";
  }

  for (auto &[i, s] : utils::enumerate(v)) {
    ss << i << " " << s.get() << "\n";
  }

  foo(ss, v);

  CHECK(ss.str() ==
        "0 a\n1 b\n2 c\n0 a\n1 b\n2 c\n0 a\n1 b\n2 c\n0 a\n1 b\n2 c\n");
}
