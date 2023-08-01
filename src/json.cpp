#include <algorithm>
#include <concepts>
#include <fstream>

#include "json.hpp"
#include "string.hpp"

namespace utils {

json_iterator_wrapper Json::begin() {
  if (is_array()) {
    return json_iterator_wrapper{std::get<JsonArray>(value_).begin()};
  } else if (is_object()) {
    return json_iterator_wrapper{std::get<JsonObject>(value_).begin()};
  } else {
    throw std::runtime_error("Cannot use begin() on non-iterable JSON.");
  }
}

json_iterator_wrapper Json::end() {
  if (is_array()) {
    return json_iterator_wrapper{std::get<JsonArray>(value_).end()};
  } else if (is_object()) {
    return json_iterator_wrapper{std::get<JsonObject>(value_).end()};
  } else {
    throw std::runtime_error("Cannot use begin() on non-iterable JSON.");
  }
}

Json parse(std::string_view json_str) {
  JsonParser parser(json_str);
  return parser.parse();
}

Json parse_file(const std::string &filename) {
  std::ifstream infile(filename);

  if (!infile.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  std::stringstream buffer;
  buffer << infile.rdbuf();

  infile.close();

  return parse(buffer.str());
}

std::string prettify(std::string_view json_str) {
  Json json = parse(json_str);
  return json.pretty_dump();
}

void wrap_in_object(const std::string &filename, const std::string &key) {

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

void wrap_in_array(const std::string &filename) {

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

std::string minify(std::string_view json_str) {

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

std::string Json::dump() const {
  std::stringstream ss;
  dump(ss);
  return ss.str();
}

std::string Json::pretty_dump(std::size_t tab_size) const {
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

std::stringstream &Json::pretty_dump(std::stringstream &ss,
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

void JsonParser::Start::handle(JsonParser &p) {

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

void JsonParser::ObjKey::handle(JsonParser &p) {

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

void JsonParser::ObjValStart::handle(JsonParser &p) {

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

void JsonParser::ObjValEnd::handle(JsonParser &p) {

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

void JsonParser::ArrValStart::handle(JsonParser &p) {

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

void JsonParser::ArrValEnd::handle(JsonParser &p) {
  char c = p.minified[p.index];

  if (c == ',') {
    p.index += 1;
    p.state.emplace<JsonParser::ArrValStart>();
  } else if (c == ']') {
    p.index += p.handle_closing_parenthesis();
  }
}

std::size_t JsonParser::handle_closing_parenthesis() {

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

std::size_t JsonParser::handle_scalar() {

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

    if (convertible_to_long_double(s)) {
      emplace_top<JsonNumber>(std::stold(s));
      return i - index;
    } else {
      throw std::runtime_error("Invalid scalar");
    }
  }
}

JsonParser::JsonParser(std::string_view json_str)
    : minified{minify(json_str)} {}

Json JsonParser::parse() {

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
