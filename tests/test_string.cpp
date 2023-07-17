#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <string>
#include <vector>

#include "string.hpp"

TEST_CASE("split") {
  CHECK(utils::split("Hello,World", ",") ==
        std::vector<std::string>{"Hello", "World"});
  CHECK(utils::split("One-Two-Three-Four", "-") ==
        std::vector<std::string>{"One", "Two", "Three", "Four"});
  CHECK(utils::split("a,b,c,d,e,f,g", ",") ==
        std::vector<std::string>{"a", "b", "c", "d", "e", "f", "g"});
  CHECK(utils::split("no delimiters here") ==
        std::vector<std::string>{"no", "delimiters", "here"});
  CHECK(utils::split("", ",") == std::vector<std::string>{});
}

TEST_CASE("join") {
  CHECK(utils::join(std::vector<std::string>{"Hello", "World"}, ", ") ==
        "Hello, World");
  CHECK(utils::join(std::vector<std::string>{"One", "Two", "Three", "Four"},
                    " ") == "One Two Three Four");
  CHECK(utils::join(std::vector<std::string>{"a", "b", "c", "d", "e", "f", "g"},
                    ",") == "a,b,c,d,e,f,g");
  CHECK(utils::join(std::vector<std::string>{"single element"}, ",") ==
        "single element");

  std::cout << "asdf" << std::endl;
  std::cout << utils::join(std::vector<std::string>{}, ",") << std::endl;
  std::cout << "fdsa" << std::endl;

  CHECK(utils::join(std::vector<std::string>{}, ",") == "");
}
