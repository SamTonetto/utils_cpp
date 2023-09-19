#pragma once

/**
 * Compile-time logger.
 * @example
 *
 * // myheader.hpp
 *
 * #include "logger.hpp"
 *
 * inline void foo() {
 *   logger::log("foo_group") << "hello foo";
 * }
 * inline void bar() {
 *   logger::log("bar_group") << "hello bar";
 * }
 *
 * // main.cpp
 *
 * #include "myheader.hpp"
 *
 * int main() {
 *  logger::enable("foo_group");
 *  foo(); // prints "hello foo"
 *  bar(); // does not print anything
 * }

 * Previously, had two separate stages: creating groups, and enabling groups.
 * Now, only one stage : enable groups.
 * - Creating groups is done by enabling them.
 * - Should be able to directly call log() without having to call enable()
 first.
 *
 * Enabling a group corresponds to placing it in the vector.
 * - DON'T throw compilation error if enabling a group that doesn't exist.
 * - Want maximum flexibility of where to put enabling/creating calls
 * - Want to be able to enable groups that are created later on, or enable
 groups that have already been created.
 *
 *
 * Decision of what to log: Just log groups whose names are in the vector of
 groupnames.
 *
 * Can just define function:
 * utils::log("group_name") { if constexpr (register.contains("group_name")) {
 *  //  } }
 *
 * NO MORE PREPROCESSOR MACROS!
 */

//

inline constexpr std::array < std::string register =
    std::array<std::string, 0>{};

auto enable() {}
