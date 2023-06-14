![Tests](https://github.com/samtonetto/utils_cpp/actions/workflows/tests.yml/badge.svg)

# utils_cpp

Custom convenience utilities with minimal reliance on external libraries.

Currently includes

- `json.hpp`: Json writing utilities
- `random.hpp`: Portable random distributions
- `print.hpp`: Convenience functions to print c++ data structures.
- `hash.hpp`: Hash functions for common STL containers.
- `compiletime.hpp`: Useful compile-time functions and utilities.

Run
```
git clone git@github.com:SamTonetto/utils_cpp.git
```

### Adding as submodule to existing repo

To add this as a submodule to another project, run:
```
git submodule add git@github.com:SamTonetto/utils_cpp.git
```

To update all submodules of a project, run the following command in the project root:
```
git submodule update --remote --merge
```


### Building with CMake

To build the library, you will require CMake version at least 3.10, and a C++20 compiler. Then inside the `utils_cpp` project root, run
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

### Including in a larger project

After building with CMake, you can include this in a larger project by placing the `utils_cpp` project folder inside a folder such as `lib/` in your project, then adding to CMake:

```
add_subdirectory(lib/utils_cpp)

target_include_directories(<your executable> PRIVATE <path to lib/ folder>)
target_link_libraries(<your executable> utils_cpp)
```

Then in your larger program, you can use headers like:
```
#include <utils_cpp/include/json.hpp>
#include <utils_cpp/include/print.hpp>
#include <utils_cpp/include/random.hpp>
#include <utils_cpp/include/hash.hpp>
#include <utils_cpp/include/compiletime.hpp>
```
