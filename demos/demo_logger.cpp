#include "utils_cpp/logger.hpp"

ENABLE_LOG_GROUP(foo)

// The following could be included code from e.g.
// #include "foobarheader.hpp"
// -----------------------
CREATE_LOG_GROUPS(foo, bar)

void foo() {
  LOG(foo) << "hello from foo group, with prefix and newline by default";
  LOG_NN(foo) << "hello from foo group, with prefix but no newline";
  LOG_NP(foo) << "hello from foo group, with newline but no prefix";
  LOG_NNP(foo) << "hello from foo group, with no prefix or newline";
}

void bar() {
  LOG(bar) << "hello from bar group, with prefix and newline by default";
  LOG_NN(bar) << "hello from bar group, with prefix but no newline";
  LOG_NP(bar) << "hello from bar group, with newline but no prefix";
  LOG_NNP(bar) << "hello from bar group, with no prefix or newline";
}
// -----------------------

int main() {

  foo(); // logged
  bar(); // not logged
}
