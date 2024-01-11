![Tests](https://github.com/samtonetto/utils_cpp/actions/workflows/tests.yml/badge.svg)

[![codecov](https://codecov.io/gh/SamTonetto/utils_cpp/branch/main/graph/badge.svg?token=ZYM5TU5WJB)](https://codecov.io/gh/SamTonetto/utils_cpp)

# utils_cpp

A header-only library of utilities I commonly use in personal projects. I don't recommend other people use this until I add better documentation and more rigorous tests. Requires CMake version of at least 3.10, and a C++20 compiler. As of writing this, it does not compile in gcc due to a compiler bug, so use clang until that's resolved. I have not tested MSVC.

## Features

### General utilities

- `hash.hpp`: Hash functions for common STL containers.
- `random.hpp`: Portable random number generation. If you use STL random functions, even if you have the same random engine and seed, different compilers can still give you different outputs due to differences in how they convert the raw output of the generator to e.g. a number in a certain range. This should give the same output no matter the compiler.
- `print.hpp`: operator<< overloads and pretty printing for common STL containers. Note that since C++23, std::print appears to be able to do the same thing, so this may be deprecated.
- `json.hpp`: Json reading/writing utilities
- `logger.hpp`: A compile-time logger where custom log groups can be specified. Works only for log comments in header files and the main executable cpp file, not in any files that must be linked (since that occurs after compilation)
- `timing.hpp`: Contains `time`, a wrapper function that can be called on any function to produce a timed version of that function, which returns a pair (function result std::optional, duration). The function result is an optional in case it returns void.
- `compiletime.hpp`: Various compile-time programming utilities, e.g.
  - `utils::is_instantiation<A, B>()` which checks if B is an instantiation of the template class A,
  - `utils::is_stl_container<A>()`, which checks if A is an STL container
  - `utils::overload`, which allows you to write concise visitors for `std::variant`
  - `utils::tuple_contains_type<A>::value`, which lets you check if an `std::tuple` contains `A`.

- Some enum utilities (heavily inspired by magic enum)

  - `enum/enum_print`: Automatic compile-time printing of enum fields.

  - `enum/enum_map`: Compile-time flat-map from enum value to value of another constexpr type.

- `parallel/thread_pool.hpp` and `parallel/threadsafe_queue.hpp`: Based very heavily on the implementation in the book `C++ Concurrency in Action`. Not well tested.


### Features of other languages

- `python.hpp`: Right now just supports "enumerate", e.g. `for(auto [i, x] : utils::enumerate(v)) { ... }`.
- `string.hpp`: Contains pythonic string functions, such as `split`, `startswith` and `join`. Should merge this into `python.hpp` at some point.
- `numpy.hpp`: Right now just supports numpy's `arange` function.
- `R.hpp`: Supports R's `seq`, `rep`, and `fapply` functions.





### Data structures

- `matrix.hpp`: A 2D matrix that is internally represented as a 1d array for maximum efficiency.
- `bigint.hpp`: Arbitrary-precision integer arithmetic
- `bitvector.hpp`: A bit-wise representation for binary vectors.
- `interval_tree.hpp`: Efficiently find all intervals that overlap with a given point (can be extended to find all overlaps with a given interval, todo if necessary)
- `segment_tree.hpp`: For an array of elements, this data structure can efficiently perform range queries (e.g. mean, median etc.) for elements in a given range [min, max). 
- `avl_tree.hpp`: A self-balancing binary search tree.
- `disjointset.hpp`: An efficient data structure for finding and counting isolated graph components.

### Graph-related

The graph utilities are mostly Boost::Graph wrappers that try to make it (slightly) more convenient. These exist in the nested `gl` (graph library) namespace, so you would access them as `utils::gl::`.

- `graph/graph.hpp`: Basic graph definitions, aliases, printing
- `graph/library.hpp`: A library of different graphs
- `graph/graphviz.hpp`: Outputting graphs to dot files
- `graph/pathfinding.hpp`: BFS, Dijkstra and A-star wrappers
- `graph/bitadjmat.hpp`: A custom and (in my opinion) very efficient bitwise adjacency matrix representation. Best suited for dense matrices, but still quite fast for graphs on the order of 100-1000s of vertices.
- `graph/vecbooladjmat.hpp`: An alternative to `bitadjmat.hpp` based on `std::vector<bool>` instead of bit-wise operations. Generally not as good as `BitAdjmat`, since it can't take advantage of bit-packing operations like `std::popcount` or `std::countr_zero`, so just use that instead.
- `graph/algorithms.hpp`: Right now just graph coloring and floyd warshall.
- `graph/line_graph.hpp`: Creates the [line graph](https://en.wikipedia.org/wiki/Line_graph) of an input graph.
- `graph/conversions.hpp`: Right now only has graph -> adjacency matrix conversions.
- `graph/transforms.hpp`: Various graph mutations. Right now randomly removing vertices or edges while keeping the graph connected. Also vertex relabelling, vertex shuffling, and contiguizing vertex labels.
- `graph/properties.hpp`: An attempt to create dynamically-typed properties that can be associated with a graph on the fly, similar to NetworkX in python. Unfortunately this turns out to be very hard to do in C++, and even though it "works" using type erasure, it has serious limitations which make it quite unpleasant to use.



## How to include in a project

### Alternative 1: Clone it then copy the include folder into the project.

This is the most basic way. It is just a header-only library, so it will work as long as the include path is right.

To clone:
```
git clone git@github.com:SamTonetto/utils_cpp.git
```

### Alternative 2: Add as a submodule

This adds it to the project and also tracks its version
```
git submodule add git@github.com:SamTonetto/utils_cpp.git
```

To update all submodules of a project, run the following command in the project root:
```
git submodule update --remote --merge
```

### Usage with CMake

If your project uses CMake, simply add this to your project root CMakeLists.txt:
```
add_subdirectory(<path to folder containing utils_cpp>)
target_include_directories(<your executable> PRIVATE <path to your include folder> <path to folder containing utils_cpp>)
target_link_libraries(<your executable> utils_cpp)
```

Then in your project you should just be able to write
```
#include "utils_cpp/include/..."
```


### Building manually

To build the library, go inside the `utils_cpp` folder and run
```
mkdir build
cd build
cmake ..
make
```
This will produce a dynamic library: `libutils_cpp.dll` for Windows or `libutils_cpp.dylib` for Linux/OSX. It will also build the documentation in `build/html/index.html`. In order to only build the docs, run
```
mkdir build
cd build
cmake ..
make utils_docs
```


