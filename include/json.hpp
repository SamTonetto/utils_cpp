/**********************************************************************
 * @brief A lightweight JSON writer
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <any>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>

#include "compiletime.hpp"

namespace utils {

class Json;
using JsonNull = std::nullptr_t;
using JsonBool = bool;
using JsonInt = int;
using JsonDouble = double;
using JsonString = std::string;
using JsonArray = std::vector<Json>;
using JsonObject = std::map<JsonString, Json>;

/**
 * A light-weight Json class for writing to Json files.
 * Does not support parsing Json strings. Can be done in the future.
 */
class Json {

public:
  Json() : value{JsonObject{}} {}
  Json(JsonInt value) : value{value} {}
  Json(JsonDouble value) : value{value} {}
  Json(JsonBool value) : value{value} {}
  Json(JsonString value) : value{value} {}

  std::string dump() const;
  std::string pretty_dump(int tab_size = 2) const;

  template <typename K, typename V> Json(const std::map<K, V> &value) {

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

    this->value = obj;
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

    this->value = obj;
  }

  template <typename T> Json(const std::vector<T> &value) {

    JsonArray arr;
    for (const auto &el : value) {
      arr.push_back(Json(el));
    }
    this->value = arr;
  }

  Json &operator[](const JsonString &key) {
    return std::get<JsonObject>(value)[key];
  }

public:
  template <typename T> Json &operator=(const T &value) {
    if constexpr (is_vector<T>::value) {
      this->value = ValueType(JsonArray{});
      for (const auto &el : value) {
        std::get<JsonArray>(this->value).push_back(Json(el));
      }
    } else if constexpr (is_map<T>::value) {
      this->value = ValueType(JsonObject{});
      for (const auto &[key, val] : value) {
        std::get<JsonObject>(this->value)[key] = Json(val);
      }
    } else if constexpr (is_unordered_map<T>::value) {
      this->value = ValueType(JsonObject{});
      for (const auto &[key, val] : value) {
        std::get<JsonObject>(this->value)[key] = Json(val);
      }
    } else {
      this->value = ValueType(value);
    }
    return *this;
  }

private:
  using ValueType = std::variant<JsonNull, JsonBool, JsonInt, JsonDouble,
                                 JsonString, JsonArray, JsonObject>;

  ValueType value;

private:
  std::stringstream &dump(std::stringstream &ss) const;

  std::stringstream &pretty_dump(std::stringstream &ss, int tab_size = 2,
                                 int current_offset = 0) const;
};

} // namespace utils
