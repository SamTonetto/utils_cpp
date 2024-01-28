#pragma once

#include <numeric>
#include <unordered_map>
#include <vector>

namespace utils {
/// DisjointSet is a standard data-structure for performing union/find
/// operations.
///
/// Union-find operations are useful for identifying disjoint components of
/// various relational entities like graphs.
class DisjointSet {

  /// Number nodes in the data structure.
  int _n;

  /// Number of connected components.
  int _num_components;

  /// An array indicating which 'parent' node the node at given index is
  /// related to.
  std::vector<int> _parent;

  /// The size of the component that the node at given index is a part of.
  std::vector<int> _size;

public:
  /// Initiates an n-node data structure.
  DisjointSet(int n);

  /// Return the "ultimate" parent at the end of the chain of parent nodes
  /// ending at node with index v.
  ///
  /// Performs path compression along the way as an optimization.
  int find(int v);

  /// Merges two groups if they have the same "ultimate" parent.
  void unify(int a, int b);

  /// Returns a vector of vectors where each subvector represents a single
  /// connected component, with elements being the indices of nodes in
  /// that component.
  std::vector<std::vector<int>> get_connected_components();
};

// IMPLEMENTATION

inline DisjointSet::DisjointSet(int n)
    : _n{n}, _num_components{n}, _size{std::vector<int>(n, 1)} {
  _parent.resize(n);
  std::iota(_parent.begin(), _parent.end(), 0);
}

inline int DisjointSet::find(int v) {
  int root = v;
  while (root != _parent[root])
    root = _parent[root];

  while (v != root) // path compression
  {
    int next = _parent[v];
    _parent[v] = root;
    v = next;
  }
  return root;
}

inline void DisjointSet::unify(int a, int b) {
  a = find(a);
  b = find(b);

  if (a == b)
    return;

  if (_size[a] < _size[b]) {
    _size[b] += _size[a];
    _parent[a] = b;
  } else {
    _size[a] += _size[b];
    _parent[b] = a;
  }
  _num_components--;
}

inline std::vector<std::vector<int>> DisjointSet::get_connected_components() {
  std::vector<std::vector<int>> connected_components(_num_components);

  std::unordered_map<int, int> group_indices;
  int group_idx = 0;
  for (int i = 0; i < _n; ++i) {
    if (group_indices.count(find(i)) == 0) {
      group_indices[find(i)] = group_idx;
      group_idx++;
    }
    connected_components[group_indices[find(i)]].push_back(i);
  }
  return connected_components;
}

} // namespace utils
