/**
 * Compile-time logger.
 * @example
 *
 * // myfunc.hpp:
 * #pragma once
 * #include <logger.hpp>
 *
 * CREATE_LOG_GROUPS(groupA, groupB, groupC)
 *
 * inline void myfunc() {
 *     LOG(GroupA) << "GroupA";
 *     LOG(GroupB) << "GroupB";
 *     LOG(GroupC) << "GroupC";
 *  }
 *
 *
 * // main.cpp:
 * #include <logger.hpp>
 *
 * // Enabling groups goes after including logger, but
 * // before including files that create the groups.
 * ENABLE_LOG_GROUPS(groupA, groupB)
 *
 * #include "myfunc.hpp"
 *
 * int main() {
 *
 *   // Set some logger settings
 *   Logger::set_log_stream(std::cout);
 *   Logger::show_time = false;    // prepends timestamp; defaults to false
 *   Logger::show_group = true;    // prepends group; defaults to true
 *   Logger::newline = true;       // prepends newline; defaults to true
 *
 *   myfunc();  // Only GroupA and GroupB will be logged.
 * }
 *
 * // Note, if you have a function such as print(std::ostream& os), you can also
 * // use it to write to a log group with print(LOG(groupA)), for example.
 */

#pragma once

#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

template <typename T> struct _logger_impl_group_enabled {
  static constexpr bool value = false;
};

#define CREATE_LOG_GROUP(group_name)                                           \
  struct _logger_impl_##group_name {                                           \
    static constexpr const char *name = #group_name;                           \
  };

#define ENABLE_LOG_GROUP(group_name)                                           \
  struct _logger_impl_##group_name;                                            \
  template <> struct _logger_impl_group_enabled<_logger_impl_##group_name> {   \
    static constexpr bool value = true;                                        \
  };

// Variadic alternatives
#define ENABLE_LOG_GROUPS(...) MAP(ENABLE_LOG_GROUP, __VA_ARGS__)
#define CREATE_LOG_GROUPS(...) MAP(CREATE_LOG_GROUP, __VA_ARGS__)

// LOG is now a macro that uses LogStream
#define LOG(group_name)                                                        \
  if constexpr (!_logger_impl_group_enabled<                                   \
                    _logger_impl_##group_name>::value) {                       \
  } else                                                                       \
    LogStream<_logger_impl_##group_name>()

// LOG_NN is LOG with "No Newline" at the end.
#define LOG_NN(group_name)                                                     \
  if constexpr (!_logger_impl_group_enabled<                                   \
                    _logger_impl_##group_name>::value) {                       \
  } else                                                                       \
    LogStream<_logger_impl_##group_name>(Logger::show_time,                    \
                                         Logger::show_group, false)

// LOG_NP is LOG with "No Prefix" at the beginning (that is, neither show_time
// nor show_group).
#define LOG_NP(group_name)                                                     \
  if constexpr (!_logger_impl_group_enabled<                                   \
                    _logger_impl_##group_name>::value) {                       \
  } else                                                                       \
    LogStream<_logger_impl_##group_name>(false, false, Logger::newline)

// LOG_NNP is LOG with "No Newline" and "No Prefix".
#define LOG_NNP(group_name)                                                    \
  if constexpr (!_logger_impl_group_enabled<                                   \
                    _logger_impl_##group_name>::value) {                       \
  } else                                                                       \
    LogStream<_logger_impl_##group_name>(false, false, false)

// ========== Variadic Macro Map ===========
// This is taken pretty much verbatim from the amazing solution by William
// Swanson: https://github.com/swansontec/map-macro Stackoverflow post here:
// https://stackoverflow.com/questions/6707148/foreach-macro-on-macros-arguments#answer-13459454
//
// I have added some annotations.

// The following ensure that macros are expanded to depth 365.
#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

// The next few lines deal with terminating the recursion..
//
// MAP_NEXT((), next)
//   -> MAP_NEXT1 (MAP_GET_END (), next)
//   -> MAP_NEXT1 (0, MAP_END, next)
//   -> MAP_NEXT0 (0, MAP_END, 0)
//   -> MAP_END MAP_OUT
//   -> MAP_END

// MAP_NEXT("x", next)
//   -> MAP_NEXT1 (MAP_GET_END "x", next)
//   -> MAP_NEXT0 (MAP_GET_END "x", next, 0)
//   -> next MAP_OUT
//   -> next
// So, if 'item' is '()', we terminate, else we move to the next item.
#define MAP_END(...)
#define MAP_OUT
#define MAP_GET_END() 0, MAP_END
#define MAP_NEXT(item, next) MAP_NEXT1(MAP_GET_END item, next)
#define MAP_NEXT0(item, next, ...) next MAP_OUT
#define MAP_NEXT1(item, next, ...) MAP_NEXT0(item, next, 0)

// Example:
//
// MAP(f, "a", "b", "c")
//   -> EVAL(MAP1 (f, "a", "b", "c", (), 0))
//   -> EVAL(f("a") NEXT ("b", MAP0) (f, "b", "c", (), 0))
//   -> EVAL(f("a") MAP0 (f, "b", "c", (), 0))
//   -> EVAL(f("a") f("b") NEXT ("c", MAP1) (f, "c", (), 0))
//   -> EVAL(f("a") f("b") MAP1 (f, "c", (), 0))
//   -> EVAL(f("a") f("b") f("c") NEXT ((), MAP0) (f, (), 0))
//   -> EVAL(f("a") f("b") f("c") END (f, (), 0))
//   -> EVAL(f("a") f("b") f("c"))
//   -> f("a") f("b") f("c")
#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)
#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, (), 0))
// ==========================================

inline std::string get_current_time() {
  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);
  auto tm = std::localtime(&now_time_t);
  std::stringstream time_stream;
  time_stream << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
  return time_stream.str();
}

/**
 * This is mostly just a struct with logging settings.
 */
struct Logger {
  inline static std::mutex log_mutex = std::mutex{};
  inline static std::ostream *log_stream = &std::clog;
  inline static bool show_time = false;
  inline static bool show_group = true;
  inline static bool newline = true;
  static void set_log_stream(std::ostream &s) { log_stream = &s; }
};

/**
 * This class does the actual logging. It is created and destroyed with each
 * call to LOG macro.
 */
template <typename Group> class LogStream {

  std::ostringstream ss;

  bool show_time = Logger::show_time;
  bool show_group = Logger::show_group;
  bool newline = Logger::newline;

public:
  LogStream() = default;

  LogStream(bool show_time, bool show_group, bool newline)
      : ss{}, show_time{show_time}, show_group{show_group}, newline{newline} {}

  template <typename T> LogStream &operator<<(const T &value) {
    ss << value;
    return *this;
  }

  ~LogStream() {
    std::scoped_lock<std::mutex> guard(Logger::log_mutex);
    if (show_time) {
      *Logger::log_stream << get_current_time() << " -- ";
    }
    if (show_group) {
      *Logger::log_stream << "[" << Group::name << "]: ";
    }
    *Logger::log_stream << ss.str();
    if (newline) {
      *Logger::log_stream << '\n';
    }
  }
};
