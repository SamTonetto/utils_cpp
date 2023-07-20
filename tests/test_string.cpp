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

  CHECK(utils::join(std::vector<std::string>{}, ",") == "");
}

TEST_CASE("convertible to double") {

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

TEST_CASE("convertible_to_int") {

  CHECK(utils::convertible_to_int("-42"));
  CHECK(utils::convertible_to_int("42"));
  CHECK(utils::convertible_to_int("40"));
  CHECK(utils::convertible_to_int("2147483647"));
  CHECK(utils::convertible_to_int("-2147483648"));

  CHECK_FALSE(utils::convertible_to_int("2147483648"));
  CHECK_FALSE(utils::convertible_to_int("-2147483649"));
  CHECK_FALSE(utils::convertible_to_int("01"));
  CHECK_FALSE(utils::convertible_to_int("42.0"));
  CHECK_FALSE(utils::convertible_to_int("42."));
  CHECK_FALSE(utils::convertible_to_int(".1"));
  CHECK_FALSE(utils::convertible_to_int(".01"));
  CHECK_FALSE(utils::convertible_to_int("0.5"));
  CHECK_FALSE(utils::convertible_to_int("."));
  CHECK_FALSE(utils::convertible_to_int(" 0.2"));
  CHECK_FALSE(utils::convertible_to_int(" 0.2 "));
  CHECK_FALSE(utils::convertible_to_int("0.2 "));
  CHECK_FALSE(utils::convertible_to_int("0. 5"));
  CHECK_FALSE(utils::convertible_to_int("02.5"));
}
