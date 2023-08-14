
#include <iostream>

#include "parallel/thread_pool.hpp"

using namespace utils;

// A function that takes arguments and returns a value.
int add(int a, int b) { return a + b; }

// A function that takes arguments and does not return a value.
void print_sum(int a, int b) { std::cout << "Sum: " << a + b << std::endl; }

// A function that does not take arguments and returns a value.
int get_magic_number() { return 42; }

// A function that does not take arguments and does not return a value.
void print_hello() { std::cout << "Hello, World!" << std::endl; }

int main() {
  parallel::thread_pool pool;

  // Submitting a function that takes arguments and returns a value.
  auto future1 = pool.submit([]() { return add(2, 3); });
  std::cout << "Result of add: " << future1.get() << std::endl;

  // Submitting a function that takes arguments and does not return a value.
  pool.submit([]() { print_sum(2, 3); });

  // Submitting a function that does not take arguments and returns a value.
  auto future2 = pool.submit(get_magic_number);
  std::cout << "Magic number: " << future2.get() << std::endl;

  // Submitting a function that does not take arguments and does not return a
  // value.
  pool.submit(print_hello);

  // Capturing arguments:

  int a = 5;
  int b = 7;

  // Here, 'a' and 'b' are captured by the lambda by value
  auto future3 = pool.submit([=]() { return add(a, b); });
  std::cout << "Result of add: " << future3.get() << std::endl;

  // Here, 'a' and 'b' are captured by the lambda by reference
  pool.submit([&]() { print_sum(a, b); });

  return 0;
}
