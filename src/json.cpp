#include "json.hpp"
namespace utils {

std::string Json::dump() const {
  std::stringstream ss;
  dump(ss);
  return ss.str();
}

std::string Json::pretty_dump(int tab_size) const {
  std::stringstream ss;
  pretty_dump(ss, tab_size);
  return ss.str();
}

std::stringstream &Json::dump(std::stringstream &ss) const {
  return std::visit(
      [&ss](auto &&arg) -> std::stringstream & {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, JsonNull>) {
          ss << "null";
        } else if constexpr (std::is_same_v<T, JsonBool>) {
          ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, JsonInt>) {
          ss << arg;
        } else if constexpr (std::is_same_v<T, JsonDouble>) {
          ss << std::setprecision(16) << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << '[';
          int index = 0;
          for (const auto &el : arg) {
            el.dump(ss);
            if (index < arg.size() - 1) {
              ss << ',';
            }
            index++;
          }
          ss << ']';
        } else if constexpr (std::is_same_v<T, JsonObject>) {
          ss << '{';
          int index = 0;
          for (const auto &[key, value] : arg) {
            ss << "\"" << key << "\":" << value.dump();
            if (index < arg.size() - 1) {
              ss << ',';
            }
            index++;
          }
          ss << '}';
        } else {
          throw std::runtime_error("Invalid type");
        }
        return ss;
      },
      value);
}

std::stringstream &Json::pretty_dump(std::stringstream &ss, int tab_size,
                                     int current_offset) const {
  return std::visit(
      [&ss, tab_size, current_offset](auto &&arg) -> std::stringstream & {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, JsonNull>) {
          ss << "null";
        } else if constexpr (std::is_same_v<T, JsonBool>) {

          ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, JsonInt>) {
          ss << arg;
        } else if constexpr (std::is_same_v<T, JsonDouble>) {
          ss << std::setprecision(16) << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << "[\n";
          int index = 0;
          for (const auto &el : arg) {
            ss << std::string(current_offset + tab_size, ' ');
            el.pretty_dump(ss, tab_size, current_offset + tab_size);

            if (index < arg.size() - 1) {
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
          int index = 0;
          for (const auto &[key, value] : arg) {
            ss << std::string(current_offset + tab_size, ' ') << "\"" << key
               << "\": ";

            value.pretty_dump(ss, tab_size, current_offset + tab_size);
            if (index < arg.size() - 1) {
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
      value);
}
} // namespace utils
