# Demo - Compile time

Some compile time utilities that are just useful.


Test if T is an instantiation of a templated object,

```
std::vector<int> x;

utils::is_instantiation<std::vector, decltype(x)>(); // true
utils::is_isntantiation<std::map, decltype(x)>(); //false
```

Test if an object is an STL container,

```
utils::is_stl_container<std::vector<int>>(); // true
utils::is_stl_container<int>(); // false
utils::is_stl_container<customclass<int>>(); // false
```
