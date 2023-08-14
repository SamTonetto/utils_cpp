/**********************************************************************
 * @brief A(n increasing less) lightweight JSON writer (and now parser)
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>

#include "compiletime.hpp"
#include "string.hpp"

namespace utils {

class Json;
class JsonParser;

using JsonNull = std::nullptr_t;
using JsonBool = bool;
using JsonString = std::string;
using JsonArray = std::vector<Json>;
using JsonObject = std::map<JsonString, Json>;

/* Json does not distinguish different types of numbers, and there are no
 * limits on number size. We will simply use long double to store all JSON
 * representations of numbers. A "double" has 64-bits and is able to represent
 * any integer from -2^53 to 2^53 exactly, which is usually more than enough.
 *
 * Originally I chose JsonNumber to be `long double`, but then I learned that
 * ARM Macs don't actually properly support long double, so I changed it to
 * `double`.
 *
 * If you want to handle `long double`s, you probably need to use a different
 * implementation.
 */
using JsonNumber = double;

/**
 * Parse a string formatted as JSON.
 * Currently, all numbers are parsed as doubles.
 * @param json_str The string to parse.
 */
Json parse(std::string_view json_str);

/**
 * Parse a JSON file.
 * Arrays [] are stored as JsonArrays, objects {} as JsonObjects.
 */
Json parse_file(const std::string &filename);

/**
 * Minify a JSON string.
 */
std::string minify(std::string_view json_str);

/**
 * Prettify a JSON string.
 */
std::string prettify(std::string_view json_str);

/**
 * Wrap a JSON file's contents in a top-level object with a string key,
 * defaulting to "data".
 *
 * Does nothing if file doesn't exist or is empty.
 * Doesn't parse file for validity, just loads it as a string and wraps it in an
 * object.
 */
void wrap_in_object(const std::string &filename,
                    const std::string &key = "data");

/**
 * Wrap a JSON file's contents in a top-level array with a string key,
 * defaulting to "data".
 *
 * Does nothing if file doesn't exist or is empty.
 * Doesn't parse file for validity, just loads it as a string and wraps it in an
 * array.
 */
void wrap_in_array(const std::string &filename);

struct json_iterator_wrapper;
struct json_iterator_value;

/**
 * A light-weight Json class for writing to Json files.
 * Does not support parsing Json strings. Can be done in the future.
 */
class Json {

public:
  // Variant unwrapper based on implementation here
  // https://stackoverflow.com/a/72955535/2520385, and extended to recursively
  // unwrap nested Json.
  template <typename... Ts>
  struct variant_unwrapper {
    const std::variant<Ts...> &var;

    template <typename T>
    operator T() {
      if constexpr (is_instantiation<std::vector, T>()) {
        T vec_result;
        vec_result.reserve(std::get<JsonArray>(var).size());
        for (const auto &elem : std::get<JsonArray>(var)) {
          vec_result.emplace_back(variant_unwrapper{elem.value_});
        }
        return vec_result;

      } else if constexpr (is_instantiation<std::map, T>()) {
        T map_result;
        for (const auto &[key, val] : std::get<JsonObject>(var)) {
          map_result.emplace(key, variant_unwrapper{val.value_});
        }
        return map_result;

      } else {
        return std::get<T>(var);
      }
    }
  };

  // deduction guide for compiler
  template <typename... Ts>
  variant_unwrapper(const std::variant<Ts...> &) -> variant_unwrapper<Ts...>;

private:
  friend JsonParser;

  using ValueType = std::variant<JsonObject, JsonArray, JsonNumber, JsonString,
                                 JsonBool, JsonNull>;

  ValueType value_;

public:
  Json() : value_{JsonObject{}} {}

  Json(std::string_view value) : value_{std::string(value)} {}
  constexpr Json(bool value) : value_{value} {}

  constexpr Json(unsigned char value)
      : value_{static_cast<JsonNumber>(value)} {}
  constexpr Json(char value) : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(unsigned short value)
      : value_{static_cast<JsonNumber>(value)} {}
  constexpr Json(short value) : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(unsigned int value) : value_{static_cast<JsonNumber>(value)} {}
  constexpr Json(int value) : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(unsigned long value)
      : value_{static_cast<JsonNumber>(value)} {}
  constexpr Json(long value) : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(unsigned long long value)
      : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(long long value) : value_{static_cast<JsonNumber>(value)} {}

  constexpr Json(float value) : value_{static_cast<JsonNumber>(value)} {}
  constexpr Json(double value) : value_{static_cast<JsonNumber>(value)} {}

  std::string dump() const;
  std::string pretty_dump(std::size_t tab_size = 2) const;

  /**
   * Write to file, overwriting anything existing.
   * Creates the file if it doesn't exist.
   */
  void write_to_file(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
      outfile << this->dump();
      outfile.close();
    }
  }

  /**
   * Load from a JSON file into the current object.
   */
  void load_from_file(const std::string &filename) {
    *this = parse_file(filename);
  }

  /**
   * Performs the function of `append_to_file`, but without parsing first to
   * check the validity of the file - to be valid the file must basically have
   * an array as the top-level element.
   */
  void unsafe_append_to_file(const std::string &filename) {

    if (!std::filesystem::exists(filename)) {

      Json json;
      json.value_.emplace<JsonArray>();
      std::get<JsonArray>(json.value_).push_back(*this);

      std::ofstream outfilestream(filename, std::ios_base::app);
      outfilestream << json.dump();
      outfilestream.close();
    } else {
      // Look for last occurrence of "]" in file, replace it with ",". Also,
      // read file in reverse for better performance.
      std::fstream filestream(filename,
                              std::ios_base::binary | std::ios_base::ate |
                                  std::ios_base::in | std::ios_base::out);

      for (long long i = filestream.tellg(); i >= 0; --i) {
        filestream.clear();

        filestream.seekg(i, std::ios_base::beg);
        char c = filestream.peek();

        if (!std::isspace(static_cast<unsigned char>(c))) {
          if (c == ']') {
            filestream.seekp(i, std::ios_base::beg);
            filestream.put(',');
            break;
          }
        }
      }
      filestream.close();

      std::ofstream outfilestream(filename, std::ios_base::app);
      outfilestream << this->dump() << ']';
      outfilestream.close();
    }
  }

  /**
   * Append to a file as a new array element.
   * Only works if either
   * 1. The top-level element of the JSON file is an array, or
   * 2. The file is empty (in which case *this is written as 0th element of an
   array), or
   * 3. The file doesn't exist, in which case the file is created then step 2
   is performed.
   */
  void append_to_file(const std::string &filename) {

    if (!std::filesystem::exists(filename)) {

      Json json;
      json.value_.emplace<JsonArray>();
      std::get<JsonArray>(json.value_).push_back(*this);

      std::ofstream outfilestream(filename, std::ios_base::app);
      outfilestream << json.dump();
      outfilestream.close();

    } else {

      Json json = parse_file(filename);

      if (json.empty()) {
        json.value_.emplace<JsonArray>();
        std::get<JsonArray>(json.value_).push_back(*this);
        std::ofstream outfilestream(filename, std::ios_base::app);
        outfilestream << json.dump();
        outfilestream.close();

      } else if (json.is_array()) {
        // Look for last occurrence of "]" in file, replace it with ",". Also,
        // read file in reverse for better performance.
        std::fstream filestream(filename,
                                std::ios_base::binary | std::ios_base::ate |
                                    std::ios_base::in | std::ios_base::out);

        for (long long i = static_cast<long long>(filestream.tellg()); i >= 0;
             --i) {

          filestream.clear();

          filestream.seekg(i, std::ios_base::beg);
          char c = filestream.peek();

          if (!std::isspace(static_cast<unsigned char>(c))) {
            if (c == ']') {
              filestream.seekp(i, std::ios_base::beg);
              filestream.put(',');
              break;
            }
          }
        }
        filestream.close();

        std::ofstream outfilestream(filename, std::ios_base::app);
        outfilestream << this->dump() << ']';
        outfilestream.close();

      } else {
        throw std::runtime_error(
            "Cannot append to file. Either file must be "
            "empty, non-existent, or top-level element must be an array.");
      }
    }
  }

  bool empty() const {
    if (is_array()) {
      return std::get<JsonArray>(value_).empty();
    } else if (is_object()) {
      return std::get<JsonObject>(value_).empty();
    } else {
      return false;
    }
  }

  bool is_array() const { return std::holds_alternative<JsonArray>(value_); }
  bool is_object() const { return std::holds_alternative<JsonObject>(value_); }
  bool is_scalar() const { return !is_array() && !is_object(); }

  template <typename T1, typename T2>
  Json(const std::pair<T1, T2> &p) {
    this->value_.emplace<JsonArray>(p.first, p.second);
  }

  template <typename K, typename V>
  Json(const std::map<K, V> &value) {

    JsonObject obj;
    if constexpr (std::is_arithmetic<K>::value) {
      for (const auto &[key, val] : value) {
        obj[std::to_string(key)] = Json(val);
      }
    } else {
      for (const auto &[key, val] : value) {
        obj[key] = Json(val);
      }
    }

    this->value_ = obj;
  }

  template <typename K, typename V>
  Json(const std::unordered_map<K, V> &value) {

    JsonObject obj;

    if constexpr (std::is_arithmetic<K>::value) {
      for (const auto &[key, val] : value) {
        obj[std::to_string(key)] = Json(val);
      }
    } else {
      for (const auto &[key, val] : value) {
        obj[key] = Json(val);
      }
    }

    this->value_ = obj;
  }

  template <typename T>
  Json(const std::vector<T> &value) {
    this->value_.emplace<JsonArray>(value.begin(), value.end());
  }

  Json &operator[](const std::string &key) {

    if (this->is_object()) {
      if (!std::get<JsonObject>(value_).contains(key)) {
        std::get<JsonObject>(value_)[key] = Json();
      }
    } else if (this->empty()) {
      value_.emplace<JsonObject>();
      std::get<JsonObject>(value_)[key] = Json();
    } else {
      throw std::runtime_error("Cannot use operator[] on non-object JSON.");
    }
    return std::get<JsonObject>(value_)[key];
  }

  const Json &at(const JsonString &key) const {
    return std::get<JsonObject>(value_).at(key);
  }

  const Json &operator[](const JsonString &key) const {
    return std::get<JsonObject>(value_).at(key);
  }

  Json &operator[](const std::size_t &index) {
    return std::get<JsonArray>(value_)[index];
  }

  const Json &operator[](const std::size_t &index) const {
    return std::get<JsonArray>(value_)[index];
  }

  const ValueType &value() const { return value_; }
  ValueType value() { return value_; }

  /**
   * Extracts JSON into a standard C++ type.
   * This version of .get() is used with this syntax:
   * auto value = dict["key"].get<double>();
   * It is also capable of extracting containers, e.g.
   * auto value = dict["key"].get<std::vector<double>>();
   */
  template <typename T>
  T get() const {
    if constexpr (is_instantiation<std::vector, T>()) {
      T vec_result;
      vec_result.reserve(std::get<JsonArray>(value_).size());
      for (const auto &elem : std::get<JsonArray>(value_)) {
        vec_result.emplace_back(variant_unwrapper{elem.value_});
      }
      return vec_result;
    } else if constexpr (is_instantiation<std::map, T>()) {
      T map_result;
      for (const auto &[key, val] : std::get<JsonObject>(value_)) {
        map_result.emplace(key, variant_unwrapper{val.value_});
      }
      return map_result;
    } else {
      return std::get<T>(value_);
    }
  }

  /**
   * Extracts JSON into a standard C++ type.
   * This version of .get() is used with this syntax:
   * double value = dict["key"].get();
   * It is also capable of extracting containers, e.g.
   * std::vector<double> value = dict["key"].get();
   */
  variant_unwrapper<JsonObject, JsonArray, JsonNumber, JsonString, JsonBool,
                    JsonNull>
  get() const {
    return variant_unwrapper{value_};
  }

  /**
   * Extracts an array of scalars, with syntax:
   * auto value = dict["key"].get_array<double>();
   */
  template <typename T>
  std::vector<T> get_array() const {
    std::vector<T> result;
    result.reserve(this->size());

    for (const auto &x : std::get<JsonArray>(value_)) {
      result.emplace_back(x.get<T>());
    }

    return result;
  }

  template <typename V>
  std::map<std::string, V> get_map() const {
    std::map<std::string, V> result;

    for (const auto &[k, v] : std::get<JsonObject>(value_)) {
      result.emplace(k, v.get<V>());
    }

    return result;
  }

  template <typename V>
  std::unordered_map<std::string, V> get_unordered_map() const {
    std::unordered_map<std::string, V> result;

    for (const auto &[k, v] : std::get<JsonObject>(value_)) {
      result.emplace(k, v.get<V>());
    }

    return result;
  }

  json_iterator_wrapper begin();
  json_iterator_wrapper end();

  /**
   * Get size of the top-level Json value_. If array, return length, if
   * object, return number of key-val pairs, if scalar, return 1.
   */
  std::size_t size() const {
    if (empty()) {
      return 0;
    } else if (is_array()) {
      return std::get<JsonArray>(value_).size();
    } else if (is_object()) {
      return std::get<JsonObject>(value_).size();
    } else {
      return 1;
    }
  }

  template <typename... Args>
  void emplace_back(Args &&...args) {
    if (this->is_array()) {
      std::get<JsonArray>(value_).emplace_back(std::forward<Args>(args)...);
    } else if (this->empty()) {
      value_.emplace<JsonArray>();
      std::get<JsonArray>(value_).emplace_back(std::forward<Args>(args)...);
    } else {
      throw std::runtime_error("Cannot emplace_back to a non-array JSON.");
    }
  }

  void push_back(const Json &json) {
    if (this->is_array()) {
      std::get<JsonArray>(value_).push_back(json);
    } else if (this->empty()) {
      value_.emplace<JsonArray>();
      std::get<JsonArray>(value_).push_back(json);
    } else {
      throw std::runtime_error("Cannot push_back to a non-array JSON.");
    }
  }

  Json &back() { return std::get<JsonArray>(value_).back(); }
  Json &front() { return std::get<JsonArray>(value_).front(); }
  const Json &back() const { return std::get<JsonArray>(value_).back(); }
  const Json &front() const { return std::get<JsonArray>(value_).front(); }

public:
  template <typename T>
  Json &operator=(const T &val) {
    if constexpr (is_instantiation<std::vector, T>() ||
                  is_instantiation<std::array, T>()) {

      this->value_.emplace<JsonArray>(val.begin(), val.end());

    } else if constexpr (is_instantiation<std::pair, T>()) {

      this->value_.emplace<JsonArray>(val.first, val.second);

    } else if constexpr (is_instantiation<std::unordered_map, T>() ||
                         is_instantiation<std::map, T>()) {

      this->value_.emplace<JsonObject>(val.begin(), val.end());

    } else if constexpr (std::is_same_v<T, bool>) {

      this->value_.emplace<JsonBool>(val);

    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {

      this->value_.emplace<JsonNull>();

    } else if constexpr (is_instantiation<std::variant, T>()) {

      std::visit([this](auto &&arg) { *this = arg; }, val);

    } else if constexpr (std::is_arithmetic_v<T>) {

      this->value_.emplace<JsonNumber>(static_cast<JsonNumber>(val));

    } else { // string

      this->value_.emplace<JsonString>(val);
    }
    return *this;
  }

private:
  std::stringstream &dump(std::stringstream &ss) const;

  std::stringstream &pretty_dump(std::stringstream &ss,
                                 std::size_t tab_size = 2,
                                 std::size_t current_offset = 0) const;
};

// ========== ITERATORS ============

struct json_iterator_value {

  using JsonRef = std::reference_wrapper<Json>;
  using JsonKeyValRef =
      std::reference_wrapper<std::pair<const std::string, Json>>;

  std::variant<JsonRef, JsonKeyValRef> v_;

  json_iterator_value(Json *json) : v_{*json} {}
  json_iterator_value(std::pair<const std::string, Json> *p) : v_{*p} {}

  std::string key() { return std::get<JsonKeyValRef>(v_).get().first; }
  const std::string &key() const {
    return std::get<JsonKeyValRef>(v_).get().first;
  }

  Json &value() {

    return std::visit(
        [this](auto &&arg) -> Json & {
          using T = std::decay_t<decltype(arg)>;
          if (std::is_same_v<T, JsonRef>) {
            return std::get<JsonRef>(this->v_).get();
          } else {
            return std::get<JsonKeyValRef>(this->v_).get().second;
          }
        },
        v_);
  }

  const Json &value() const {
    return std::visit(
        [this](auto &&arg) -> Json & {
          using T = std::decay_t<decltype(arg)>;
          if (std::is_same_v<T, JsonRef>) {
            return std::get<JsonRef>(this->v_).get();
          } else {
            return std::get<JsonKeyValRef>(this->v_).get().second;
          }
        },
        v_);
  }
};

struct json_iterator_wrapper {

  using JsonIteratorType =
      std::variant<JsonArray::iterator, JsonObject::iterator>;

  JsonIteratorType it;

  bool is_array_iterator() const {
    return std::holds_alternative<JsonArray::iterator>(it);
  }
  bool is_object_iterator() {
    return std::holds_alternative<JsonObject::iterator>(it);
  }

  json_iterator_wrapper &operator++() {
    if (is_array_iterator()) {
      ++std::get<JsonArray::iterator>(it);
    } else if (is_object_iterator()) {
      ++std::get<JsonObject::iterator>(it);
    }
    return *this;
  }

  constexpr bool operator!=(const json_iterator_wrapper &other) const {
    return it != other.it;
  }

  constexpr bool operator==(const json_iterator_wrapper &other) const {
    return it != other.it;
  }

  json_iterator_value operator*() {
    if (is_array_iterator()) {
      return json_iterator_value(&*std::get<JsonArray::iterator>(it));
    } else if (is_object_iterator()) {
      return json_iterator_value(&*std::get<JsonObject::iterator>(it));
    }

    throw std::runtime_error("Cannot dereference iterator.");
  }
};

class JsonParser {
public:
  JsonParser(std::string_view json_str);

  Json parse();

private:
  std::string minified;
  std::size_t index = 0;

  std::stack<std::reference_wrapper<Json>> stack;

public:
  struct Start {
    void handle(JsonParser &context);
  };
  struct ObjKey {
    void handle(JsonParser &context);
  };
  struct ObjValStart {
    void handle(JsonParser &context);
  };
  struct ObjValEnd {
    void handle(JsonParser &context);
  };
  struct ArrValStart {
    void handle(JsonParser &context);
  };
  struct ArrValEnd {
    void handle(JsonParser &context);
  };

  using StateType = std::variant<Start, ObjKey, ObjValStart, ObjValEnd,
                                 ArrValStart, ArrValEnd>;

  StateType state = Start{};

  /**
   * Seeks ahead from start_index to find a matching end quote character (that
   * hasn't been escaped). If the end quote character is found, the string
   * between start_index and the end quote character is returned. If the end
   * quote character is not found, an exception is thrown.
   */
  std::string gather_string(std::size_t start_index);

  std::size_t handle_closing_parenthesis();
  std::size_t handle_scalar();

  template <typename State>
  void handle(char input) {
    std::get<State>(state).handle(*this, input);
  }

  template <typename State>
  void go_to() {
    state.emplace<State>();
  }

  template <typename State>
  bool is_current_state() {
    return std::holds_alternative<State>(state);
  }

  template <typename T, typename... Args>
  void emplace_top(Args &&...args) {
    stack.top().get().value_.emplace<T>(std::forward<Args>(args)...);
  }

  Json &top() { return stack.top().get(); }
  const Json &top() const { return stack.top().get(); }

  std::string get_state_string() {
    return std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if (std::is_same_v<Start, T>) {
            return "Start";
          } else if (std::is_same_v<ObjKey, T>) {
            return "ObjKey";
          } else if (std::is_same_v<ObjValStart, T>) {
            return "ObjValStart";
          } else if (std::is_same_v<ObjValEnd, T>) {
            return "ObjValEnd";
          } else if (std::is_same_v<ArrValStart, T>) {
            return "ArrValStart";
          } else if (std::is_same_v<ArrValEnd, T>) {
            return "ArrValEnd";
          }
        },
        state);
  }
};

// ===============================
// ======= Implementation ========
// ===============================

inline json_iterator_wrapper Json::begin() {
  if (is_array()) {
    return json_iterator_wrapper{std::get<JsonArray>(value_).begin()};
  } else if (is_object()) {
    return json_iterator_wrapper{std::get<JsonObject>(value_).begin()};
  } else {
    throw std::runtime_error("Cannot use begin() on non-iterable JSON.");
  }
}

inline json_iterator_wrapper Json::end() {
  if (is_array()) {
    return json_iterator_wrapper{std::get<JsonArray>(value_).end()};
  } else if (is_object()) {
    return json_iterator_wrapper{std::get<JsonObject>(value_).end()};
  } else {
    throw std::runtime_error("Cannot use begin() on non-iterable JSON.");
  }
}

inline Json parse(std::string_view json_str) {
  JsonParser parser(json_str);
  return parser.parse();
}

inline Json parse_file(const std::string &filename) {
  std::ifstream infile(filename);

  if (!infile.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  std::stringstream buffer;
  buffer << infile.rdbuf();

  infile.close();

  return parse(buffer.str());
}

inline std::string prettify(std::string_view json_str) {
  Json json = parse(json_str);
  return json.pretty_dump();
}

inline void wrap_in_object(const std::string &filename,
                           const std::string &key) {

  if (!std::filesystem::exists(filename)) {
    return;
  }

  std::ifstream f(filename, std::ios_base::binary);
  if (f.peek() == std::ifstream::traits_type::eof()) {
    return; // empty file
  }

  std::stringstream buffer;
  buffer << f.rdbuf();
  f.close();

  std::string out = "{\"" + key + "\":" + buffer.str() + "}";

  std::ofstream of(filename, std::ios_base::binary);
  of << out;
  of.close();
}

inline void wrap_in_array(const std::string &filename) {

  if (!std::filesystem::exists(filename)) {
    return;
  }

  std::ifstream f(filename, std::ios_base::binary);
  if (f.peek() == std::ifstream::traits_type::eof()) {
    return; // empty file
  }

  std::stringstream buffer;
  buffer << f.rdbuf();
  f.close();

  std::string out = "[" + buffer.str() + "]";

  std::ofstream of(filename, std::ios_base::binary);
  of << out;
  of.close();
}

inline std::string minify(std::string_view json_str) {

  std::string minified;

  bool in_quotes = false;
  bool is_escaped = false;

  for (char c : json_str) {
    if (!in_quotes && std::isspace(c)) {
      continue;
    }

    minified += c;

    if (c == '"' && !is_escaped) {
      in_quotes = !in_quotes;
    }

    is_escaped = (c == '\\' && !is_escaped);
  }

  return minified;
}

inline std::string Json::dump() const {
  std::stringstream ss;
  dump(ss);
  return ss.str();
}

inline std::string Json::pretty_dump(std::size_t tab_size) const {
  std::stringstream ss;
  pretty_dump(ss, tab_size);
  return ss.str();
}

inline std::stringstream &Json::dump(std::stringstream &ss) const {
  return std::visit(
      [&ss](auto &&arg) -> std::stringstream & {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, JsonNull>) {
          ss << "null";
        } else if constexpr (std::is_same_v<T, JsonBool>) {
          ss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, JsonNumber>) {
          ss << std::setprecision(std::numeric_limits<JsonNumber>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << '[';
          std::size_t index = 0;
          for (const auto &el : arg) {
            el.dump(ss);
            if (index + 1 < arg.size()) {
              ss << ',';
            }
            ++index;
          }
          ss << ']';
        } else if constexpr (std::is_same_v<T, JsonObject>) {
          ss << '{';
          std::size_t index = 0;
          for (const auto &[key, val] : arg) {
            ss << "\"" << key << "\":" << val.dump();
            if (index + 1 < arg.size()) {
              ss << ',';
            }
            ++index;
          }
          ss << '}';
        } else {
          throw std::runtime_error("Invalid type");
        }
        return ss;
      },
      value_);
}

inline std::stringstream &Json::pretty_dump(std::stringstream &ss,
                                            std::size_t tab_size,
                                            std::size_t current_offset) const {
  return std::visit(
      [&ss, tab_size, current_offset](auto &&arg) -> std::stringstream & {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, JsonNull>) {
          ss << "null";
        } else if constexpr (std::is_same_v<T, JsonBool>) {
          ss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, JsonNumber>) {
          ss << std::setprecision(std::numeric_limits<JsonNumber>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << "[\n";
          std::size_t index = 0;
          for (const auto &el : arg) {
            ss << std::string(current_offset + tab_size, ' ');
            el.pretty_dump(ss, tab_size, current_offset + tab_size);

            if (index + 1 < arg.size()) {
              ss << ',';
            }
            if (index < arg.size()) {
              ss << '\n';
            }
            index++;
          }
          ss << std::string(current_offset, ' ') << ']';
        } else if constexpr (std::is_same_v<T, JsonObject>) {
          ss << "{\n";
          std::size_t index = 0;
          for (const auto &[key, val] : arg) {
            ss << std::string(current_offset + tab_size, ' ') << "\"" << key
               << "\": ";

            val.pretty_dump(ss, tab_size, current_offset + tab_size);
            if (index + 1 < arg.size()) {
              ss << ',';
            }
            if (index < arg.size()) {
              ss << '\n';
            }
            index++;
          }
          ss << std::string(current_offset, ' ') << '}';
        } else {
          throw std::runtime_error("Invalid type");
        }
        return ss;
      },
      value_);
}

// ===== JSON Parsing =====

inline void JsonParser::Start::handle(JsonParser &p) {

  char c = p.minified[p.index];

  if (c == '{') {
    p.index += 1;
    p.state.emplace<JsonParser::ObjKey>();

  } else if (c == '[') {

    p.emplace_top<JsonArray>();
    p.index += 1;
    p.state.emplace<JsonParser::ArrValStart>();

  } else {

    p.index += p.handle_scalar();

    if (p.index != p.minified.size()) {
      throw std::runtime_error("Invalid JSON");
    }
  }
}

inline void JsonParser::ObjKey::handle(JsonParser &p) {

  char c = p.minified[p.index];

  if (c == '}') {

    p.index += p.handle_closing_parenthesis();

  } else {

    std::string key = p.gather_string(p.index);
    p.stack.push(p.stack.top().get()[key]);

    p.index += key.size(); // the actual key size
    p.index += 2;          // start and end quotes

    if (p.minified[p.index] != ':') {
      throw std::runtime_error(
          "Invalid JSON - Key string must be followed by colon");
    }
    p.index += 1; // colon
    p.state.emplace<JsonParser::ObjValStart>();
  }
}

inline void JsonParser::ObjValStart::handle(JsonParser &p) {

  char c = p.minified[p.index];

  if (c == '{') {
    p.index += 1;
    p.state.emplace<JsonParser::ObjKey>();
  } else if (c == '[') {
    p.index += 1;
    p.emplace_top<JsonArray>();
    p.state.emplace<JsonParser::ArrValStart>();
  } else {
    p.index += p.handle_scalar();
    p.stack.pop();
    p.state.emplace<JsonParser::ObjValEnd>();
  }
}

inline void JsonParser::ObjValEnd::handle(JsonParser &p) {

  char c = p.minified[p.index];

  if (c == '}') {
    p.index += p.handle_closing_parenthesis();
  } else if (c == ',') {
    p.index += 1;
    p.state.emplace<JsonParser::ObjKey>();
  } else {
    throw std::runtime_error("Invalid JSON");
  }
}

inline void JsonParser::ArrValStart::handle(JsonParser &p) {

  char c = p.minified[p.index];

  if (c == '[') {
    p.top().emplace_back(JsonArray{});
    p.stack.push(p.top().back());
    p.index += 1;
    p.state.emplace<JsonParser::ArrValStart>();
  } else if (c == '{') {
    p.top().emplace_back(JsonObject{});
    p.stack.push(p.top().back());
    p.index += 1;
    p.state.emplace<JsonParser::ObjKey>();
  } else if (c == ']') {
    p.index += p.handle_closing_parenthesis();
  } else {
    p.top().emplace_back(JsonObject{});
    p.stack.push(p.top().back());
    p.index += p.handle_scalar();
    p.stack.pop();
    p.state.emplace<JsonParser::ArrValEnd>();
  }
}

inline void JsonParser::ArrValEnd::handle(JsonParser &p) {
  char c = p.minified[p.index];

  if (c == ',') {
    p.index += 1;
    p.state.emplace<JsonParser::ArrValStart>();
  } else if (c == ']') {
    p.index += p.handle_closing_parenthesis();
  }
}

inline std::size_t JsonParser::handle_closing_parenthesis() {

  bool is_str_end = (index + 1 == minified.size());

  if (is_str_end) {

    bool is_matching_paren =
        (stack.top().get().is_array() && minified[index] == ']') ||
        (stack.top().get().is_object() && minified[index] == '}');

    if (!is_matching_paren) {
      throw std::runtime_error("Invalid JSON - Incorrect parenthesis match.");
    } else if (stack.size() > 1) {

      throw std::runtime_error("Invalid JSON - Not enough closing  parens");
    } else if (stack.size() < 1) {
      throw std::runtime_error("Invalid JSON - Too many closing parens");
    }

  } else {
    stack.pop();

    if (top().is_array()) {
      go_to<JsonParser::ArrValEnd>();
    } else if (top().is_object()) {
      go_to<JsonParser::ObjValEnd>();
    } else {
      throw std::runtime_error("Invalid JSON");
    }
  }
  return 1;
}

std::string JsonParser::gather_string(std::size_t index) {
  std::size_t end = index + 1;
  while (end < minified.size() && minified[end] != '"') {
    if (minified[end] == '\\') {
      ++end;
    }
    ++end;
  }

  if (end == minified.size()) {
    throw std::runtime_error("Invalid JSON - Unterminated string.");
  }

  return std::string(minified.begin() + index + 1, minified.begin() + end);
}

inline std::size_t JsonParser::handle_scalar() {

  char c = minified[index];

  if (c == '"') {

    // STRING

    std::string s = gather_string(index);
    stack.top().get().value_ = JsonString{s};
    return s.size() + 2;

  } else if (minified.substr(index, 4) == "null") {

    emplace_top<JsonNull>();
    return 4;

  } else if (minified.substr(index, 4) == "true") {

    emplace_top<JsonBool>(true);
    return 4;

  } else if (minified.substr(index, 5) == "false") {

    emplace_top<JsonBool>(false);
    return 5;

  } else {

    std::size_t i = index;
    while (i < minified.size() && minified[i] != ',' && minified[i] != '}' &&
           minified[i] != ']') {
      ++i;
    }
    std::string s = minified.substr(index, i - index);

    if (convertible_to_double(s)) {
      emplace_top<JsonNumber>(std::stold(s));
      return i - index;
    } else {
      throw std::runtime_error("Invalid scalar");
    }
  }
}

inline JsonParser::JsonParser(std::string_view json_str)
    : minified{minify(json_str)} {}

inline Json JsonParser::parse() {

  Json json;

  stack.push(json);

  while (index < minified.size()) {
    std::visit([this](auto &&state) { state.handle(*this); }, state);
  }

  if (stack.size() > 1) {
    throw std::runtime_error(
        "Invalid JSON - Missing closing braces or brackets.");

  } else if (stack.size() < 1) {
    throw std::runtime_error(
        "Invalid JSON - Too many closing braces or brackets.");
  }

  return json;
}

} // namespace utils
