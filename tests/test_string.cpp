#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <string>
#include <vector>

#include "print.hpp"
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

  CHECK(utils::split("double  spaced  words   ") ==
        std::vector<std::string>{"double", "spaced", "words"});

  CHECK_THROWS(utils::split("double  spaced  word", ""));
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

  CHECK(utils::join(std::vector<std::string>{}, ",") == "");
}

TEST_CASE("convertible to long double") {

  CHECK(utils::convertible_to_double("42"));
  CHECK(utils::convertible_to_double("-42.0"));
  CHECK(utils::convertible_to_double("42.0"));
  CHECK(utils::convertible_to_double("42."));
  CHECK(utils::convertible_to_double(".1"));
  CHECK(utils::convertible_to_double(".01"));
  CHECK(utils::convertible_to_double("0.5"));

  CHECK_FALSE(utils::convertible_to_double("."));
  CHECK_FALSE(utils::convertible_to_double(" 0.2"));
  CHECK_FALSE(utils::convertible_to_double(" 0.2 "));
  CHECK_FALSE(utils::convertible_to_double("0.2 "));
  CHECK_FALSE(utils::convertible_to_double("0. 5"));
  CHECK_FALSE(utils::convertible_to_double("02.5"));
}

TEST_CASE("convertible to long long") {

  CHECK(utils::convertible_to_long_long("-42"));
  CHECK(utils::convertible_to_long_long("42"));
  CHECK(utils::convertible_to_long_long("40"));

  CHECK_FALSE(utils::convertible_to_long_long("1.0"));
  CHECK_FALSE(utils::convertible_to_long_long("01"));
  CHECK_FALSE(utils::convertible_to_long_long("42.0"));
  CHECK_FALSE(utils::convertible_to_long_long("42."));
  CHECK_FALSE(utils::convertible_to_long_long(".1"));
  CHECK_FALSE(utils::convertible_to_long_long(".01"));
  CHECK_FALSE(utils::convertible_to_long_long("0.5"));
  CHECK_FALSE(utils::convertible_to_long_long("."));
  CHECK_FALSE(utils::convertible_to_long_long(" 0.2"));
  CHECK_FALSE(utils::convertible_to_long_long(" 0.2 "));
  CHECK_FALSE(utils::convertible_to_long_long("0.2 "));
  CHECK_FALSE(utils::convertible_to_long_long("0. 5"));
  CHECK_FALSE(utils::convertible_to_long_long("02.5"));
}

TEST_CASE("test startswith") {

  CHECK(utils::startswith("Hello, World", "Hello"));
  CHECK(utils::startswith("Hello, World", "Hello,"));
  CHECK(utils::startswith("Hello, World", "Hello, "));
  CHECK(utils::startswith("Hello, World", "Hello, W"));

  CHECK(utils::startswith("Hello, World", "h") == false);
}
