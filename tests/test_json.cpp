#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "json.hpp"

#include <iostream>
#include <map>
#include <sstream>

TEST_CASE("key with space") {
  utils::Json dict;

  dict["key with space"] = 1;

  CHECK(dict.dump() == "{\"key with space\":1}");
}

TEST_CASE("non-templated get()") {

  utils::Json dict;

  std::vector<double> vec_in{1, 2, 3};
  std::map<std::string, double> obj_in{{"a", 1}, {"b", 2}};

  dict["obj"] = obj_in;
  dict["vec"] = vec_in;

  std::vector<double> vec_out = dict["vec"].get();
  std::map<std::string, double> obj_out = dict["obj"].get();

  CHECK(vec_in == vec_out);
  CHECK(obj_in == obj_out);
}

TEST_CASE("templated get()") {

  utils::Json dict;

  std::vector<double> vec_in{1, 2, 3};
  std::map<std::string, double> obj_in{{"a", 1}, {"b", 2}};

  dict["obj"] = obj_in;
  dict["vec"] = vec_in;

  auto vec_out = dict["vec"].get<std::vector<double>>();
  auto obj_out = dict["obj"].get<std::map<std::string, double>>();

  CHECK(vec_in == vec_out);
  CHECK(obj_in == obj_out);
}

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
      "{\"key-bool\":false,\"key-"
      "double\":3.7999999999999998,\"key-int\":1,"
      "\"key-null\":null,"
      "\"key-string\":\"helloworld\",\"object-map\":{\"inner-key-1\":1,\"inner-"
      "key-2\":2},\"object-umap\":{\"inner-key-1\":\"inner-val-1\",\"inner-key-"
      "2\":\"inner-val-2\"},\"vector-bool\":[true,false],\"vector-"
      "double\":[1,"
      "2.2999999999999998],\"vector-int\":[1,2,3],\"vector-object-map\":[{"
      "\"1\":1,\"2\":2},{"
      "\"3\":3,\"4\":4}],\"vector-string\":[\"hello\",\"world\"]}");

  CHECK(dict.pretty_dump(2) ==
        "{\n  \"key-bool\": false,\n  \"key-double\": "
        "3.7999999999999998,\n  "
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

TEST_CASE("assign variant") {

  std::variant<int, double> v = 1;

  utils::Json j;

  j["variant"] = v;

  CHECK(j.dump() == "{\"variant\":1}");
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

  CHECK_FALSE(json.is_scalar());
  CHECK(json.is_object());
  CHECK_FALSE(json.is_array());
}

TEST_CASE("parse empty") {
  std::string s2 = "{}";
  auto json = utils::parse(s2);
  CHECK(json.dump() == "{}");

  CHECK_FALSE(json.is_scalar());
  CHECK(json.is_object());
  CHECK_FALSE(json.is_array());
}

TEST_CASE("parse number") {

  std::string s3 = "42";
  auto json = utils::parse(s3);
  CHECK(std::get<double>(json.value()) == 42);

  CHECK(json.is_scalar());
  CHECK_FALSE(json.is_object());
  CHECK_FALSE(json.is_array());
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

  CHECK(json.is_array());
  CHECK_FALSE(json.is_object());
  CHECK_FALSE(json.is_scalar());
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

  CHECK_FALSE(json.is_scalar());
  CHECK(json.is_object());
  CHECK_FALSE(json.is_array());
}

TEST_CASE("test parse file") {

  std::ofstream f("temporary_test_file.json");
  f << "{\n  \"key1\": [1,2],\n  \"key2\": [{\"key3\":  \"val\"}]\n}";
  f.close();

  auto json = utils::parse_file("temporary_test_file.json");

  CHECK(std::get<double>(json["key1"][0].value()) == 1);
  CHECK(json["key1"][0].get<double>() == 1);

  CHECK(std::get<double>(json["key1"][1].value()) == 2);
  CHECK(json["key1"][1].get<double>() == 2);

  CHECK(std::get<std::string>(json["key2"][0]["key3"].value()) == "val");
  CHECK(json["key2"][0]["key3"].get<std::string>() == "val");

  CHECK_FALSE(json.is_scalar());
  CHECK(json.is_object());
  CHECK_FALSE(json.is_array());

  std::remove("temporary_test_file.json");
}

TEST_CASE("Test write_to_file method") {
  utils::Json json;
  json["key"] = "value"; // Assuming the Json class works this way

  std::string filename = "test_file.json";
  json.write_to_file(filename);

  std::ifstream file(filename);
  CHECK(file.is_open());

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  CHECK(content == "{\"key\":\"value\"}");

  std::remove(filename.c_str()); // Clean up
}

TEST_CASE("Test load_from_file method") {
  std::string filename = "test_load_file.json";
  std::ofstream outfile(filename);
  outfile << "{\"key\":\"value\"}";
  outfile.close();

  utils::Json json;
  json.load_from_file(filename);

  CHECK(json["key"].get<std::string>() == "value");

  std::remove(filename.c_str()); // Clean up
}

TEST_CASE("Test append_to_file method for an empty file") {
  std::string filename = "test_append_file_empty.json";
  std::ofstream outfile(filename);
  outfile.close(); // Create an empty file

  utils::Json json;
  json["key"] = "value1";
  json.append_to_file(filename);

  utils::Json jsonFromFile;
  jsonFromFile.load_from_file(filename);

  CHECK(jsonFromFile[0]["key"].get<std::string>() == "value1");

  std::remove(filename.c_str()); // Clean up
}

TEST_CASE("Test create and then append to file") {
  std::string filename = "test_create_append_file.json";

  utils::Json json;
  json["key"] = "value1";

  json.append_to_file(filename);

  utils::Json jsonFromFile;

  jsonFromFile.load_from_file(filename);

  CHECK(jsonFromFile[0]["key"].get<std::string>() == "value1");

  std::remove(filename.c_str()); // Clean up
}

TEST_CASE("test throw exception if no ending bracket") {

  std::string json_str1 = "[{\"k1\": 1, \"k2\": 2}]";
  std::string json_str2 = "[{\"k1\": 1, \"k2\": 2}";

  CHECK_NOTHROW(utils::parse(json_str1));
  CHECK_THROWS(utils::parse(json_str2));
}

TEST_CASE("Test append_to_file method for a file with top-level array") {
  std::string filename = "test_append_file_array.json";
  std::ofstream outfile(filename);
  outfile << "[{\"key\":\"value1\"}]";
  outfile.close();

  utils::Json json;
  json["key2"] = "value2";

  json.append_to_file(filename);

  utils::Json jsonFromFile;
  jsonFromFile.load_from_file(filename);

  CHECK(jsonFromFile[0]["key"].get<std::string>() == "value1");
  CHECK(jsonFromFile[1]["key2"].get<std::string>() == "value2");

  std::remove(filename.c_str()); // Clean up
}

TEST_CASE("get array") {

  std::string json_string = "{\"key\": [0,1,2,3,4,0.5]}";
  utils::Json json = utils::parse(json_string);

  std::vector<double> v = json["key"].get_array<double>();

  CHECK(v == std::vector<double>({0, 1, 2, 3, 4, 0.5}));
}

TEST_CASE("get map and umap") {
  std::string json_string = "{\"key1\":1, \"key2\": 2, \"key3\": 3}";
  utils::Json json = utils::parse(json_string);

  SUBCASE("map") {
    std::map<std::string, double> result = json.get_map<double>();

    CHECK(result ==
          std::map<std::string, double>{{"key1", 1}, {"key2", 2}, {"key3", 3}});
  }

  SUBCASE("unordered_map") {
    std::unordered_map<std::string, double> result =
        json.get_unordered_map<double>();

    CHECK(result == std::unordered_map<std::string, double>{
                        {"key1", 1}, {"key2", 2}, {"key3", 3}});
  }
}

TEST_CASE("array and obj iteration") {

  std::string json_string = "[{\"key1\": [0,1,2,3,4,0.5], \"key2\": [-1,-2]}, "
                            "{\"key3\": [1,1,2,3,4,0.5]}]";

  utils::Json json = utils::parse(json_string);

  std::vector<std::map<std::string, std::vector<double>>>
      encoded_data_structure;

  // Note: Could also just use for(auto array_entry : json)
  for (utils::json_iterator_value array_entry : json) {

    encoded_data_structure.push_back({});

    for (utils::json_iterator_value map_entry : array_entry.value()) {

      std::string key = map_entry.key(); // keys are always strings for JSON.
      auto val = map_entry.value().get_array<double>();

      encoded_data_structure.back()[key] = val;
    }
  }

  std::vector<std::map<std::string, std::vector<double>>> correct(2);
  correct[0] = {{"key1", {0, 1, 2, 3, 4, 0.5}}, {"key2", {-1, -2}}};
  correct[1] = {{"key3", {1, 1, 2, 3, 4, 0.5}}};

  CHECK(encoded_data_structure == correct);
}
