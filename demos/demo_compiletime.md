# Demo - Compile time

Some compile time utilities that are just useful.

```
if constexpr (utils::is_vector<T>::value) {   // tests if T is a vector
  // ...
}
if constexpr (utils::is_set<T>::value) {   // tests if T is a set
  // ...
}
if constexpr (utils::is_unordered_set<T>::value) {   // tests if T is an unordered_set
  // ...
}
if constexpr (utils::is_map<T>::value) {   // tests if T is a map
  // ...
}
if constexpr (utils::is_unordered_map<T>::value) {   // tests if T is an unordered_map
  // ...
}
```
