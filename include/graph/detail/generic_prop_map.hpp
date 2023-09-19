/**********************************************************************
 * @brief A string-to-property map metamap.
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <nlohmann/json.hpp>

namespace utils {

namespace gl {

template <typename PropType>
struct GenericPropMap {
  std::map<std::string, PropType> data;

  PropType &operator[](const std::string &key) { return data[key]; }

  const PropType &operator[](const std::string &key) const {
    return data.at(key);
  }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }

  std::size_t size() const { return data.size(); }

  bool contains(const std::string &key) const {
    return data.find(key) != data.end();
  }

  nlohmann::json to_json() const {
    nlohmann::json j;
    for (const auto &[k, v] : data) {
      std::visit([&j, &k](const auto &e) { j[k] = e; }, v.p);
    }
    return j;
  }
};

} // namespace gl
} // namespace utils
