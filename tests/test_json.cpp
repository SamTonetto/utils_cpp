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
      "{\"key-bool\":false,\"key-double\":3.8,\"key-int\":1,\"key-null\":null,"
      "\"key-string\":\"helloworld\",\"object-map\":{\"inner-key-1\":1,\"inner-"
      "key-2\":2},\"object-umap\":{\"inner-key-1\":\"inner-val-1\",\"inner-key-"
      "2\":\"inner-val-2\"},\"vector-bool\":[true,false],\"vector-double\":[1,"
      "2.3],\"vector-int\":[1,2,3],\"vector-object-map\":[{\"1\":1,\"2\":2},{"
      "\"3\":3,\"4\":4}],\"vector-string\":[\"hello\",\"world\"]}");

  CHECK(dict.pretty_dump(2) ==
        "{\n  \"key-bool\": false,\n  \"key-double\": 3.8,\n  \"key-int\": "
        "1,\n  \"key-null\": null,\n  \"key-string\": \"helloworld\",\n  "
        "\"object-map\": {\n    \"inner-key-1\": 1,\n    \"inner-key-2\": 2\n  "
        "},\n  \"object-umap\": {\n    \"inner-key-1\": \"inner-val-1\",\n    "
        "\"inner-key-2\": \"inner-val-2\"\n  },\n  \"vector-bool\": [\n    "
        "true,\n    false\n  ],\n  \"vector-double\": [\n    1,\n    2.3\n  "
        "],\n  \"vector-int\": [\n    1,\n    2,\n    3\n  ],\n  "
        "\"vector-object-map\": [\n    {\n      \"1\": 1,\n      \"2\": 2\n    "
        "},\n    {\n      \"3\": 3,\n      \"4\": 4\n    }\n  ],\n  "
        "\"vector-string\": [\n    \"hello\",\n    \"world\"\n  ]\n}");
}
