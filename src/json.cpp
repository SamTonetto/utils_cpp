#include <fstream>

#include "json.hpp"
#include "string.hpp"

namespace utils {

Json parse(std::string_view json_str) {
  JsonParser parser(json_str);
  return parser.parse();
}

Json parse_file(const std::string &filename) {
  std::ifstream infile(filename);

  if (!infile.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  std::string json_str((std::istreambuf_iterator<char>(infile)),
                       std::istreambuf_iterator<char>());

  infile.close();

  return parse(json_str);
}

std::string prettify(std::string_view json_str) {
  Json json = parse(json_str);
  return json.pretty_dump();
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
          ss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, JsonChar> ||
                             std::is_same_v<T, JsonUChar> ||
                             std::is_same_v<T, JsonShort> ||
                             std::is_same_v<T, JsonUShort> ||
                             std::is_same_v<T, JsonInt> ||
                             std::is_same_v<T, JsonUInt> ||
                             std::is_same_v<T, JsonLong> ||
                             std::is_same_v<T, JsonULong> ||
                             std::is_same_v<T, JsonLongLong> ||
                             std::is_same_v<T, JsonULongLong>) {
          ss << arg;
        } else if constexpr (std::is_same_v<T, JsonFloat>) {
          ss << std::setprecision(std::numeric_limits<JsonFloat>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonDouble>) {
          ss << std::setprecision(std::numeric_limits<JsonDouble>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonLongDouble>) {
          ss << std::setprecision(
                    std::numeric_limits<JsonLongDouble>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << '[';
          int index = 0;
          for (const auto &el : arg) {
            el.dump(ss);
            if (index < static_cast<int>(arg.size()) - 1) {
              ss << ',';
            }
            index++;
          }
          ss << ']';
        } else if constexpr (std::is_same_v<T, JsonObject>) {
          ss << '{';
          int index = 0;
          for (const auto &[key, val] : arg) {
            ss << "\"" << key << "\":" << val.dump();
            if (index < static_cast<int>(arg.size()) - 1) {
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
      value_);
}

std::stringstream &Json::pretty_dump(std::stringstream &ss, int tab_size,
                                     int current_offset) const {
  return std::visit(
      [&ss, tab_size, current_offset](auto &&arg) -> std::stringstream & {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, JsonNull>) {
          ss << "null";
        } else if constexpr (std::is_same_v<T, JsonBool>) {
          ss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, JsonChar> ||
                             std::is_same_v<T, JsonUChar> ||
                             std::is_same_v<T, JsonShort> ||
                             std::is_same_v<T, JsonUShort> ||
                             std::is_same_v<T, JsonInt> ||
                             std::is_same_v<T, JsonUInt> ||
                             std::is_same_v<T, JsonLong> ||
                             std::is_same_v<T, JsonULong> ||
                             std::is_same_v<T, JsonLongLong> ||
                             std::is_same_v<T, JsonULongLong>) {
          ss << arg;
        } else if constexpr (std::is_same_v<T, JsonFloat>) {
          ss << std::setprecision(std::numeric_limits<JsonFloat>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonDouble>) {
          ss << std::setprecision(std::numeric_limits<JsonDouble>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonLongDouble>) {
          ss << std::setprecision(
                    std::numeric_limits<JsonLongDouble>::max_digits10)
             << arg;
        } else if constexpr (std::is_same_v<T, JsonString>) {
          ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, JsonArray>) {
          ss << "[\n";
          int index = 0;
          for (const auto &el : arg) {
            ss << std::string(current_offset + tab_size, ' ');
            el.pretty_dump(ss, tab_size, current_offset + tab_size);

            if (index < static_cast<int>(arg.size()) - 1) {
              ss << ',';
            }
            if (index < static_cast<int>(arg.size())) {
              ss << '\n';
            }
            index++;
          }
          ss << std::string(current_offset, ' ') << ']';
        } else if constexpr (std::is_same_v<T, JsonObject>) {
          ss << "{\n";
          int index = 0;
          for (const auto &[key, val] : arg) {
            ss << std::string(current_offset + tab_size, ' ') << "\"" << key
               << "\": ";

            val.pretty_dump(ss, tab_size, current_offset + tab_size);
            if (index < static_cast<int>(arg.size()) - 1) {
              ss << ',';
            }
            if (index < static_cast<int>(arg.size())) {
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

void JsonParser::Start::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == '{') {
    jp.handle_left_brace_as_obj_value();
  } else if (c == '[') {
    jp.handle_left_bracket_as_obj_value();
  } else {
    jp.handle_scalar_isolated();

    jp.transition_to<JsonParser::End>();
  }
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

void JsonParser::ObjKeyStart::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == '"') {

    std::string key = jp.gather_string(jp.index);
    jp.json_stack.push(jp.json_stack.top().get()[key]);

    jp.transition_to<JsonParser::ObjKeyEnd>();

    jp.index += key.size() + 2;

  } else if (c == '}') {
    jp.handle_right_brace();
  } else {
    throw std::runtime_error("Invalid JSON - Expected '\"' or '}'.");
  }
}

void JsonParser::ObjKeyEnd::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == ':') {
    jp.handle_colon();
  } else {
    throw std::runtime_error("Invalid JSON - Expected ':' after key.");
  }
}

void JsonParser::ObjValStart::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == '{') {
    jp.handle_left_brace_as_obj_value();
  } else if (c == '[') {
    jp.handle_left_bracket_as_obj_value();
  } else {
    jp.handle_scalar_as_obj_value();
    jp.transition_to<JsonParser::ObjValEnd>();
  }
}

void JsonParser::ObjValEnd::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == ',') {
    jp.transition_to<JsonParser::ObjKeyStart>();
    ++jp.index;
  } else if (c == '}') {
    jp.handle_right_brace();
  } else {
    throw std::runtime_error("Invalid JSON - Expected ',' or '}'.");
  }
}

void JsonParser::ArrValStart::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == '{') {
    jp.handle_left_brace_in_array();
  } else if (c == '[') {
    jp.handle_left_bracket_in_array();
  } else {
    jp.handle_scalar_in_array();
    jp.transition_to<JsonParser::ArrValEnd>();
  }
}

void JsonParser::ArrValEnd::handle(JsonParser &jp) {

  char c = jp.minified[jp.index];

  if (c == ',') {
    jp.transition_to<JsonParser::ArrValStart>();
    ++jp.index;
  } else if (c == ']') {
    jp.handle_right_bracket();
  } else {
    throw std::runtime_error("Invalid JSON - Expected ',' or ']'.");
  }
}

void JsonParser::End::handle(JsonParser &jp) {
  throw std::runtime_error("Invalid JSON - Unexpected character.");
}

void JsonParser::handle_right_brace() {

  if (!std::holds_alternative<JsonObject>(json_stack.top().get().value_)) {
    throw std::runtime_error("Invalid JSON - Unexpected closing brace.");
  }

  if (json_stack.size() == 1 && index == minified.size() - 1) {
    ++index;
    transition_to<JsonParser::End>();
  } else {

    json_stack.pop();

    if (std::holds_alternative<JsonObject>(json_stack.top().get().value_)) {
      transition_to<JsonParser::ObjValEnd>();
      ++index;
    } else if (std::holds_alternative<JsonArray>(
                   json_stack.top().get().value_)) {
      transition_to<JsonParser::ArrValEnd>();
      ++index;
    }
  }
}

void JsonParser::handle_right_bracket() {

  if (!std::holds_alternative<JsonArray>(json_stack.top().get().value_)) {
    throw std::runtime_error("Invalid JSON - Unexpected closing bracket.");
  }

  if (json_stack.size() == 1 && index == minified.size() - 1) {
    ++index;
    transition_to<JsonParser::End>();
  } else {

    json_stack.pop();

    if (std::holds_alternative<JsonObject>(json_stack.top().get().value_)) {
      transition_to<JsonParser::ObjValEnd>();
      ++index;
    } else if (std::holds_alternative<JsonArray>(
                   json_stack.top().get().value_)) {
      transition_to<JsonParser::ArrValEnd>();
      ++index;
    }
  }
}

void JsonParser::handle_colon() {

  transition_to<JsonParser::ObjValStart>();
  ++index;
}

void JsonParser::handle_left_brace_in_array() {

  auto &arr = std::get<JsonArray>(json_stack.top().get().value_);
  arr.emplace_back(JsonObject{});
  json_stack.push(arr.back());

  transition_to<JsonParser::ObjKeyStart>();
  ++index;
}

void JsonParser::handle_left_brace_as_obj_value() {

  json_stack.top().get().value_ = JsonObject();

  transition_to<JsonParser::ObjKeyStart>();
  ++index;
}

void JsonParser::handle_left_bracket_in_array() {

  auto &arr = std::get<JsonArray>(json_stack.top().get().value_);
  arr.emplace_back(JsonArray{});
  json_stack.push(arr.back());

  transition_to<JsonParser::ArrValStart>();
  ++index;
}

void JsonParser::handle_left_bracket_as_obj_value() {

  json_stack.top().get().value_ = JsonArray();
  transition_to<JsonParser::ArrValStart>();
  ++index;
}

void JsonParser::handle_scalar_isolated() {

  char c = minified[index];

  if (c == '"') { // string
    std::string s = gather_string(index);
    json_stack.top().get().value_ = JsonString{s};
    index += s.size() + 2;
  }
  // check null
  else if (minified.substr(index, 4) == "null") {
    json_stack.top().get().value_ = JsonNull{};
    index += 4;
  } else if (minified.substr(index, 4) == "true") {
    json_stack.top().get().value_ = JsonBool{true};
    index += 4;
  } else if (minified.substr(index, 5) == "false") {
    json_stack.top().get().value_ = JsonBool{false};
    index += 5;
  } else {

    // seek forward until we find a comma, a closing brace, or a closing
    // bracket
    std::string s;
    while (index < minified.size() && minified[index] != ',' &&
           minified[index] != '}' && minified[index] != ']') {
      s += minified[index];
      ++index;
    }

    if (convertible_to_double(s)) {
      json_stack.top().get().value_ = JsonDouble{std::stod(s)};
    } else {
      throw std::runtime_error("Invalid scalar");
    }
  }

  if (index != minified.size()) {
    throw std::runtime_error("Invalid JSON");
  }
}

void JsonParser::handle_scalar_in_array() {

  char c = minified[index];

  if (c == '"') { // string
    std::string s = gather_string(index);

    std::get<JsonArray>(json_stack.top().get().value_)
        .emplace_back(JsonString{s});

    index += s.size() + 2;
  }
  // check null
  else if (minified.substr(index, 4) == "null") {

    std::get<JsonArray>(json_stack.top().get().value_).emplace_back(JsonNull{});

    index += 4;
  } else if (minified.substr(index, 4) == "true") {

    std::get<JsonArray>(json_stack.top().get().value_)
        .emplace_back(JsonBool{true});

    index += 4;
  } else if (minified.substr(index, 5) == "false") {

    std::get<JsonArray>(json_stack.top().get().value_)
        .emplace_back(JsonBool{false});

    index += 5;
  } else {

    // seek forward until we find a comma, a closing brace, or a closing
    // bracket
    std::string s;
    while (index < minified.size() && minified[index] != ',' &&
           minified[index] != '}' && minified[index] != ']') {
      s += minified[index];
      ++index;
    }

    if (convertible_to_double(s)) {

      std::get<JsonArray>(json_stack.top().get().value_)
          .emplace_back(JsonDouble{std::stod(s)});

    } else {
      throw std::runtime_error("Invalid scalar");
    }
  }
}

void JsonParser::handle_scalar_as_obj_value() {

  char c = minified[index];

  if (c == '"') { // string
    std::string s = gather_string(index);

    json_stack.top().get().value_.emplace<JsonString>(s);

    index += s.size() + 2;

  }
  // check null
  else if (minified.substr(index, 4) == "null") {
    json_stack.top().get().value_ = Json::ValueType(JsonNull{});
    index += 4;
  } else if (minified.substr(index, 4) == "true") {
    json_stack.top().get().value_ = Json::ValueType(JsonBool{true});
    index += 4;
  } else if (minified.substr(index, 5) == "false") {
    json_stack.top().get().value_ = Json::ValueType(JsonBool{false});
    index += 5;
  } else {

    // seek forward until we find a comma, a closing brace, or a closing
    // bracket
    std::string s;
    while (index < minified.size() && minified[index] != ',' &&
           minified[index] != '}' && minified[index] != ']') {
      s += minified[index];
      ++index;
    }

    if (convertible_to_double(s)) {
      json_stack.top().get().value_ = Json::ValueType(JsonDouble{std::stod(s)});
    } else {
      throw std::runtime_error("Invalid scalar");
    }
  }

  json_stack.pop();
}

JsonParser::JsonParser(std::string_view json_str)
    : minified{minify(json_str)} {}

Json JsonParser::parse() {

  Json json;

  json_stack.push(json);

  while (index < minified.size()) {
    std::visit([this](auto &&state) { state.handle(*this); }, state);
  }

  if (json_stack.size() > 1) {
    throw std::runtime_error(
        "Invalid JSON - Missing closing braces or brackets.");

  } else if (json_stack.size() < 1) {
    throw std::runtime_error(
        "Invalid JSON - Too many closing braces or brackets.");
  }

  return json;
}

} // namespace utils
