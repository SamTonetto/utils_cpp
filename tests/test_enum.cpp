#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "utils_cpp/enum/enum_map.hpp"

enum class TestEnum {
  A,
  B,
  C,
  D,
};

constexpr auto test_map = utils::make_enum_map<TestEnum, int>(
    {{TestEnum::A, 1}, {TestEnum::B, 2}, {TestEnum::C, 3}, {TestEnum::D, 4}});

TEST_CASE("test enum map") {

  CHECK(test_map[TestEnum::A] == 1);
  CHECK(test_map[TestEnum::B] == 2);
  CHECK(test_map[TestEnum::C] == 3);
  CHECK(test_map[TestEnum::D] == 4);
}
