#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "json.hpp"

#include <iostream>
#include <map>
#include <sstream>

TEST_CASE("dump object") {

  utils::Json dict;

  dict["key-null"] = nullptr;
  dict["key-int"] = 1;
  dict["key-double"] = 3.8;
  dict["key-bool"] = false;
  dict["key-string"] = "helloworld";

  dict["vector-int"] = std::vector<int>{1, 2, 3};

  dict["vector-string"] = std::vector<std::string>{"hello", "world"};
  dict["vector-double"] = std::vector<double>{1.0, 2.3};
  dict["vector-bool"] = std::vector<bool>{true, false};

  dict["object-map"] =
      std::map<std::string, int>({{"inner-key-1", 1}, {"inner-key-2", 2}});

  dict["object-umap"] = std::unordered_map<std::string, std::string>(
      {{"inner-key-1", "inner-val-1"}, {"inner-key-2", "inner-val-2"}});

  dict["vector-object-map"] = std::vector<std::unordered_map<int, int>>(
      {{{1, 1}, {2, 2}}, {{3, 3}, {4, 4}}});

  CHECK(
      dict.dump() ==
      "{\"key-bool\":false,\"key-double\":3.7999999999999998,\"key-int\":1,"
      "\"key-null\":null,"
      "\"key-string\":\"helloworld\",\"object-map\":{\"inner-key-1\":1,\"inner-"
      "key-2\":2},\"object-umap\":{\"inner-key-1\":\"inner-val-1\",\"inner-key-"
      "2\":\"inner-val-2\"},\"vector-bool\":[true,false],\"vector-double\":[1,"
      "2.2999999999999998],\"vector-int\":[1,2,3],\"vector-object-map\":[{"
      "\"1\":1,\"2\":2},{"
      "\"3\":3,\"4\":4}],\"vector-string\":[\"hello\",\"world\"]}");

  CHECK(dict.pretty_dump(2) ==
        "{\n  \"key-bool\": false,\n  \"key-double\": 3.7999999999999998,\n  "
        "\"key-int\": "
        "1,\n  \"key-null\": null,\n  \"key-string\": \"helloworld\",\n  "
        "\"object-map\": {\n    \"inner-key-1\": 1,\n    \"inner-key-2\": 2\n  "
        "},\n  \"object-umap\": {\n    \"inner-key-1\": \"inner-val-1\",\n    "
        "\"inner-key-2\": \"inner-val-2\"\n  },\n  \"vector-bool\": [\n    "
        "true,\n    false\n  ],\n  \"vector-double\": [\n    1,\n    "
        "2.2999999999999998\n  "
        "],\n  \"vector-int\": [\n    1,\n    2,\n    3\n  ],\n  "
        "\"vector-object-map\": [\n    {\n      \"1\": 1,\n      \"2\": 2\n    "
        "},\n    {\n      \"3\": 3,\n      \"4\": 4\n    }\n  ],\n  "
        "\"vector-string\": [\n    \"hello\",\n    \"world\"\n  ]\n}");
}

TEST_CASE("create nested keys easily") {

  utils::Json dict;

  dict["key1"]["key2"]["key3"] = 1;

  CHECK(dict.dump() == "{\"key1\":{\"key2\":{\"key3\":1}}}");
}

TEST_CASE("Testing minify") {
  CHECK(utils::minify(
            "{\n    \"name\": \"John\",\n    \"age\": 30,\n    \"city\": "
            "\"Canberra\"}") ==
        "{\"name\":\"John\",\"age\":30,\"city\":\"Canberra\"}");
  CHECK(utils::minify(
            "{ \"name\": \"John\", \"age\": 30, \"city\": \"Canberra\" }") ==
        "{\"name\":\"John\",\"age\":30,\"city\":\"Canberra\"}");
  CHECK(utils::minify("{\"name\": \"John Smith\", \"age\": 30, \"city\": \"New "
                      "York\", \"escaped\": \"This\\\"is\\\"escaped\"}") ==
        "{\"name\":\"John Smith\",\"age\":30,\"city\":\"New "
        "York\",\"escaped\":\"This\\\"is\\\"escaped\"}");
  CHECK(utils::minify("{\"name\":\"John\",\"age\":30,\"city\":\"Canberra\"}") ==
        "{\"name\":\"John\",\"age\":30,\"city\":\"Canberra\"}");
}

TEST_CASE("test parse") {

  std::string s1 = "{\"key\": \"abc\"}";

  auto json = utils::parse(s1);

  CHECK(std::get<std::string>(json["key"].value()) == "abc");
}

TEST_CASE("parse empty") {
  std::string s2 = "{}";
  auto json = utils::parse(s2);
  CHECK(json.dump() == "{}");
}

TEST_CASE("parse number") {

  std::string s3 = "42";
  auto json = utils::parse(s3);
  CHECK(std::get<double>(json.value()) == 42);
}

TEST_CASE("parse array") {

  std::string s4 = "[true, false, 42, [1,2,3]]";
  auto json = utils::parse(s4);
  CHECK(std::get<bool>(json[0].value()) == true);
  CHECK(std::get<bool>(json[1].value()) == false);
  CHECK(std::get<double>(json[2].value()) == 42);
  CHECK(std::get<double>(json[3][0].value()) == 1);
  CHECK(std::get<double>(json[3][1].value()) == 2);
  CHECK(std::get<double>(json[3][2].value()) == 3);
}

TEST_CASE("test parse again ") {

  std::string json_string =
      "{\n  \"key1\": [1,2],\n  \"key2\": [{\"key3\":  \"val\"}]\n}";

  auto json = utils::parse(json_string);

  CHECK(std::get<double>(json["key1"][0].value()) == 1);
  CHECK(json["key1"][0].get<double>() == 1);

  CHECK(std::get<double>(json["key1"][1].value()) == 2);
  CHECK(json["key1"][1].get<double>() == 2);

  CHECK(std::get<std::string>(json["key2"][0]["key3"].value()) == "val");
  CHECK(json["key2"][0]["key3"].get<std::string>() == "val");
}
