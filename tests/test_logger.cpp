#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "logger.hpp"

#include <sstream>

ENABLE_LOG_GROUPS(A1)
CREATE_LOG_GROUPS(A1, A2)

TEST_CASE("compile_time_logging") {

  std::stringstream ss;
  Logger::set_log_stream(ss);

  SUBCASE("Groups that are enabled") {
    LOG(A1) << "Hello from Group A1";
    CHECK(ss.str() == "[A1]: Hello from Group A1\n");
  }

  SUBCASE("Groups that are disabled") {
    LOG(A2) << "Hello from Group A2";
    CHECK(ss.str().empty() == true);
  }
}

ENABLE_LOG_GROUPS(B1, B2, B3)
CREATE_LOG_GROUPS(B1, B2, B3)

TEST_CASE("Test different log variants") {

  std::stringstream ss;
  Logger::set_log_stream(ss);

  SUBCASE("no newline") {
    LOG_NN(B1) << "Hello from Group B1";
    CHECK(ss.str() == "[B1]: Hello from Group B1");
  }

  SUBCASE("no prefix") {
    LOG_NP(B2) << "Hello from Group B2";
    CHECK(ss.str() == "Hello from Group B2\n");
  }

  SUBCASE("no newline or prefix") {
    LOG_NNP(B3) << "Hello from Group B3";
    CHECK(ss.str() == "Hello from Group B3");
  }
}
