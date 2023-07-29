#!/bin/bash

# Based on:
# https://stackoverflow.com/questions/72331290/gcov-produces-different-results-on-clang-and-gcc

# To run script for gcc toolchain:
#     TC=clang build-run-cov.sh
# To run script for LLVM toolchain:
#     TC=gcc build-run-cov.sh

# https://vaneyckt.io/posts/safer_bash_scripts_with_set_euxo_pipefail/
set -Eeuxo pipefail

if [ -z "${TC}" ]; then
    echo "Error: TC (toolchain) variable is not set. Please specify 'gcc' or 'clang'."
    echo "Example: TC=gcc ./build-run-cov.sh"
    exit 1
elif [ "${TC}" = "gcc" ]; then
    export CC="gcc-13"
    export CXX="g++-13"
    COV="gcov-13"

    if [ -d "gcc-build" ]; then
	rm -r "gcc-build"
    fi

    cmake --preset gcc-build
    cmake --build --preset gcc-build
    cd gcc-build/
    ctest

    # Create coverage report - lcov must be installed.
    lcov --directory . --capture --output-file gcc-coverage.info --gcov-tool $COV

    # Exclude system files from report.
    lcov --remove gcc-coverage.info '/usr/* /opt/*' --output-file gcc-coverage.info

    cd ..


elif [ "${TC}" = "clang" ]; then
    export CC="clang"
    export CXX="clang++"
    COV="llvm-cov"

    if [ -d "clang-build" ]; then
	rm -r "clang-build"
    fi
    
    cmake --preset clang-build
    cmake --build --preset clang-build
    cd clang-build/
    ctest

    # Create coverage report - lcov must be installed.
    lcov --directory . --capture --output-file clang-coverage.info --gcov-tool $COV

    # Exclude system files from report.
    lcov --remove clang-coverage.info '/usr/*' --output-file clang-coverage.info

    cd ..

else
    echo "Error: Invalid toolchain specified in TC variable. Options are 'gcc' or 'clang'."
    exit 1
fi


