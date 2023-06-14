# Demo - hash

`hash.hpp` has a bunch of basic hash functions for standard STL containers:

```
pair_hash<T1, T2>   // order matters
vector_hash<T>      // order matters

set_hash<T>              // permutation invariant
unordered_set_hash<T>    // permutation invariant

map_hash<Key, T>             // permutation invariant
unordered_map_hash<Key, T>   // permutation invariant
```

This allows you to use them as keys in unordered_sets or unordered_maps, for example:

```
std::unordered_set<std::set<int>, set_hash<int>> my_set_of_sets;
std::unordered_map<std::vector<int>, int> my_vector_to_int_map;
std::unordered_map<std::pair<int, int>, int> my_pair_to_int_map;
```

In particular, the mappings from pair to int is quite useful whenever you need to map from coordinates to an index of some sort.
