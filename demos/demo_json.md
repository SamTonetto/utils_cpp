# Demo - Json output

The json writer is quite flexible.

Simply define a Json object, e.g.

```
#include "json.hpp"
utils::Json json;
```

By default, it is a dictionary-like object, and you can set keys and values.

```
json["key1"] = 2; // int
json["key2"] = 2.3; // double
json["key2"] = "hello world"; // string
json["key4"] = true; // bool
json["key5"] = nullptr; // nullptr
```

The RHS can also be any nested combination of data composed of `vector`, `unordered_map`, and `map` containers, where the basic types are from the aforementioned types: `int, double, string, bool, nullptr`. The only other rule is if a `map` or `unordered_map` is used, the key must either be a string or an arithmetic type (e.g. int/double etc.). If the key is an arithmetic type, it is cast to a string.


For example, you can do,

```
json["key6"] = std::vector<int>{1,2,3};
json["key7"] = std::unordered_map<std::string, int> {{"a",1},{"b",2}};

json["key8"] = std::vector<std::unordered_map<int, std::map<std::string, double>>>
{ 
  { 
    {1, {{"a",1.5}, {"b", 2.5}}},
    {3, {{"c", 3.5}, {"d", 4.5}}},
  }
  {
    {10, {{"aa",1.5}, {"bb", 2.5}}},
    {30, {{"cc", 3.5}, {"dd", 4.5}}}
  },
  {
    {100, {{"aaa",1.5}, {"bbb", 2.5}}},
    {300, {{"ccc", 3.5}, {"ddd", 4.5}}}
  }
};
```

You can also nest json dictionaries, i.e.

```
Json json2;
json["key9"] = json;
```

Then to dump to string, you just do
```
std::string json_string = json2.dump();
```

Or, a formatted string:
```
int tab_size = 2;
std::string formatted_json_string = json2.pretty_dump(tab_size);
```





