/**********************************************************************
 * @brief A(n increasing less) lightweight JSON writer (and now parser)
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <any>
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

// Standard integer types
using JsonChar = signed char;
using JsonUChar = unsigned char;
using JsonShort = short int;
using JsonUShort = unsigned short int;
using JsonInt = int;
using JsonUInt = unsigned int;
using JsonLong = long int;
using JsonULong = unsigned long int;
using JsonLongLong = long long int;
using JsonULongLong = unsigned long long int;

// Standard floating point types
using JsonFloat = float;
using JsonDouble = double;
using JsonLongDouble = long double;

using JsonString = std::string;
using JsonArray = std::vector<Json>;
using JsonObject = std::map<JsonString, Json>;

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

struct json_iterator_wrapper;
struct json_iterator_value;

/**
 * A light-weight Json class for writing to Json files.
 * Does not support parsing Json strings. Can be done in the future.
 */
class Json {

private:
  friend JsonParser;

  using ValueType =
      std::variant<JsonObject, JsonArray, JsonNull, JsonBool, JsonChar,
                   JsonUChar, JsonShort, JsonUShort, JsonInt, JsonUInt,
                   JsonLong, JsonULong, JsonLongLong, JsonULongLong, JsonFloat,
                   JsonDouble, JsonLongDouble, JsonString>;

  ValueType value_;

public:
  Json() : value_{JsonObject{}} {}

  constexpr Json(const JsonString &value) : value_{value} {}
  constexpr Json(JsonBool value) : value_{value} {}

  constexpr Json(JsonChar value) : value_{value} {}
  constexpr Json(JsonUChar value) : value_{value} {}
  constexpr Json(JsonShort value) : value_{value} {}
  constexpr Json(JsonUShort value) : value_{value} {}
  constexpr Json(JsonInt value) : value_{value} {}
  constexpr Json(JsonUInt value) : value_{value} {}
  constexpr Json(JsonLong value) : value_{value} {}
  constexpr Json(JsonULong value) : value_{value} {}
  constexpr Json(JsonLongLong value) : value_{value} {}
  constexpr Json(JsonULongLong value) : value_{value} {}

  constexpr Json(JsonFloat value) : value_{value} {}
  constexpr Json(JsonDouble value) : value_{value} {}
  constexpr Json(JsonLongDouble value) : value_{value} {}

  std::string dump() const;
  std::string pretty_dump(int tab_size = 2) const;

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

      Json json{JsonArray{}};
      std::get<JsonArray>(json.value_).push_back(*this);
      json.write_to_file(filename);

    } else {

      Json json = parse_file(filename);

      if (json.empty()) {
        json.value_.emplace<JsonArray>();
        std::get<JsonArray>(json.value_).push_back(*this);
      } else if (json.is_array()) {
        std::get<JsonArray>(json.value_).push_back(*this);
      } else {
        throw std::runtime_error(
            "Cannot append to file. Either file must be "
            "empty, or top-level element must be an array.");
      }

      json.write_to_file(filename);
    }
  }

  constexpr bool empty() const {
    if (is_array()) {
      return std::get<JsonArray>(value_).empty();
    } else if (is_object()) {
      return std::get<JsonObject>(value_).empty();
    } else {
      return false;
    }
  }

  constexpr bool is_array() const {
    return std::holds_alternative<JsonArray>(value_);
  }
  constexpr bool is_object() const {
    return std::holds_alternative<JsonObject>(value_);
  }
  constexpr bool is_scalar() const { return !is_array() && !is_object(); }

  template <typename T1, typename T2>
  Json(const std::pair<T1, T2> &p) {
    JsonArray arr;
    arr.push_back(Json(p.first));
    arr.push_back(Json(p.second));
    this->value_ = arr;
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

    JsonArray arr;
    for (const auto &el : value) {
      arr.push_back(Json(el));
    }
    this->value_ = arr;
  }

  Json &operator[](const JsonString &key) {

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

  Json &operator[](const JsonULongLong &index) {
    return std::get<JsonArray>(value_)[index];
  }

  const Json &operator[](const JsonULongLong &index) const {
    return std::get<JsonArray>(value_)[index];
  }

  const ValueType &value() const { return value_; }
  ValueType value() { return value_; }

  /**
   * Number aware visitor is a visitor that just returns the type, unless it is
   * a number, in which case it obeys the following rules:
   * 1. Allow conversion from integral to floating-point types, but not the
   *    other way.
   * 2. Prevent downconversion but allow upconversion within integral types and
   *    within floating point types separately.
   */
  template <typename RequestedType>
  struct number_aware_visitor {

    template <typename StoredType>
    RequestedType operator()(const StoredType &u) const {
      if constexpr (std::is_arithmetic_v<RequestedType> &&
                    std::is_arithmetic_v<StoredType>) {
        if (std::is_integral_v<RequestedType> &&
            std::is_floating_point_v<StoredType>) {
          throw std::runtime_error(
              "Invalid conversion, precision would be lost due to "
              "integral/floating-point cross-conversion");
        } else if constexpr (sizeof(RequestedType) < sizeof(StoredType)) {
          throw std::runtime_error("Invalid conversion, precision would be "
                                   "lost due to downconversion");
        } else {
          return static_cast<RequestedType>(u);
        }
      } else if constexpr (std::is_same_v<RequestedType, StoredType>) {
        return static_cast<StoredType>(u);
      } else {
        throw std::runtime_error("Unable to extract type");
      }
    }
  };

  /**
   * Number-aware getting.
   */
  template <typename T>
  T get() const {
    return std::visit(number_aware_visitor<T>{}, value_);
  }

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
  constexpr std::size_t size() const {
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
  constexpr void emplace_back(Args &&...args) {
    if (this->is_array()) {
      std::get<JsonArray>(value_).emplace_back(std::forward<Args>(args)...);
    } else if (this->empty()) {
      value_.emplace<JsonArray>();
      std::get<JsonArray>(value_).emplace_back(std::forward<Args>(args)...);
    } else {
      throw std::runtime_error("Cannot emplace_back to a non-array JSON.");
    }
  }

  constexpr void push_back(const Json &json) {
    if (this->is_array()) {
      std::get<JsonArray>(value_).push_back(json);
    } else if (this->empty()) {
      value_.emplace<JsonArray>();
      std::get<JsonArray>(value_).push_back(json);
    } else {
      throw std::runtime_error("Cannot push_back to a non-array JSON.");
    }
  }

public:
  template <typename T>
  Json &operator=(const T &val) {
    if constexpr (is_same_container_v<std::vector, T>) {
      this->value_ = ValueType(JsonArray{});
      for (const auto &el : val) {
        std::get<JsonArray>(this->value_).push_back(Json(el));
      }
    } else if constexpr (is_same_container_v<std::pair, T>) {
      this->value_ = ValueType(JsonArray{});
      std::get<JsonArray>(this->value_).push_back(Json(val.first));
      std::get<JsonArray>(this->value_).push_back(Json(val.second));
    } else if constexpr (is_same_container_v<std::map, T>) {
      this->value_ = ValueType(JsonObject{});
      for (const auto &[key, v] : val) {
        std::get<JsonObject>(this->value_)[key] = Json(v);
      }
    } else if constexpr (is_same_container_v<std::unordered_map, T>) {
      this->value_ = ValueType(JsonObject{});
      for (const auto &[key, v] : val) {
        std::get<JsonObject>(this->value_)[key] = Json(v);
      }
    } else {
      this->value_ = ValueType(val);
    }
    return *this;
  }

private:
  std::stringstream &dump(std::stringstream &ss) const;

  std::stringstream &pretty_dump(std::stringstream &ss, int tab_size = 2,
                                 int current_offset = 0) const;
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

  constexpr bool is_array_iterator() const {
    return std::holds_alternative<JsonArray::iterator>(it);
  }
  constexpr bool is_object_iterator() {
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

  std::stack<std::reference_wrapper<Json>> json_stack;

public:
  // State Machine
  struct Start {
    void handle(JsonParser &context);
  };
  struct ObjKeyStart {
    void handle(JsonParser &context);
  };
  struct ObjKeyEnd {
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
  struct End {
    void handle(JsonParser &context);
  };

  using StateType = std::variant<Start, ObjKeyStart, ObjKeyEnd, ObjValStart,
                                 ObjValEnd, ArrValStart, ArrValEnd, End>;

  StateType state = Start{};

  /**
   * Handles number|bool|null|string for the current value, if object and
   * array have already been ruled out.
   */
  void handle_scalar_as_obj_value();
  void handle_scalar_in_array();

  /**
   * Seeks ahead from start_index to find a matching end quote character (that
   * hasn't been escaped). If the end quote character is found, the string
   * between start_index and the end quote character is returned. If the end
   * quote character is not found, an exception is thrown.
   */
  std::string gather_string(std::size_t start_index);

  // Common handlers
  // As part of handling, they also manage index updating.
  void handle_colon();
  void handle_left_brace_in_array();
  void handle_left_brace_as_obj_value();
  void handle_left_bracket_in_array();
  void handle_left_bracket_as_obj_value();
  void handle_right_brace();
  void handle_right_bracket();
  void handle_scalar_isolated(); // handle a string only consisting of a scalar

  template <typename State>
  void handle(char input) {
    std::get<State>(state).handle(*this, input);
  }

  template <typename State>
  void transition_to() {
    state = State{};
  }

  template <typename State>
  bool is_current_state() {
    return std::holds_alternative<State>(state);
  }
};

} // namespace utils
