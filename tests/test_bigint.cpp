#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "bigint.hpp"

using namespace utils;

TEST_CASE("bigint_from_string") {
  std::vector<std::string> data = {
      "-963833671479914415300607941028",
      "-9",
      "-1",
      "0",
      "1",
      "10",
      "4026621640054138",
      "120467396067168722125296345372",
      "446180939327639349573495823498572345924110849597495"};

  std::vector<BigInt> bigints;
  for (std::size_t i = 0; i < data.size(); ++i)
    bigints.emplace_back(data[i]);

  std::vector<std::string> results(data.size());
  for (std::size_t i = 0; i < data.size(); ++i) {
    std::stringstream ss;
    ss << bigints[i];
    results[i] = ss.str();
  }

  CHECK(results == data);
}

TEST_CASE("bigint_from_longlong") {
  std::vector<long long> data = {-9223372036854775807ll, -1ll, 0ll, 1ll,
                                 9223372036854775807ll};

  std::vector<BigInt> bigints;
  for (std::size_t i = 0; i < data.size(); ++i) {
    bigints.emplace_back(data[i]);
  }

  std::vector<std::string> results(data.size());
  for (std::size_t i = 0; i < data.size(); ++i) {
    std::stringstream ss;
    ss << bigints[i];
    results[i] = ss.str();
  }

  std::vector<std::string> answer = {"-9223372036854775807", "-1", "0", "1",
                                     "9223372036854775807"};

  CHECK(results == answer);
}
