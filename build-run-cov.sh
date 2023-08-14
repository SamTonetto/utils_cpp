#!/bin/bash

# Based on:
# https://stackoverflow.com/questions/72331290/gcov-produces-different-results-on-clang-and-gcc

# To run script for gcc toolchain:
#     TC=clang build-run-cov.sh
# To run script for LLVM toolchain:
#     TC=gcc build-run-cov.sh

# Note: clangd, which is the language server used for editor LSPs, by default only searches subdirectories called build/. Therefore, instead of calling the two builds "gcc-build" and "build", we will just pick one to be called "build/", and that is the one that clangd will get compile_commands.json from. It probably doesn't matter, but we will pick 'clang' to be the default build folder, since it belongs to the same toolchain as clangd, so higher chance they play nice together. So we will have "build" and "gcc-build".


# https://vaneyckt.io/posts/safer_bash_scripts_with_set_euxo_pipefail/
set -Eeuxo pipefail

# Because for some reason lcov --remove doesn't work well with relative paths
BASE_DIR=$(pwd)

if [ -z "${TC}" ]; then
    echo "Error: TC (toolchain) variable is not set. Please specify 'gcc' or 'clang'."
    echo "Example: TC=gcc ./build-run-cov.sh"
    exit 1

elif [ "${TC}" = "clang" ]; then

    # Darwin = Mac, for running locally.
    if [[ "$(uname -s)" == "Darwin" ]]; then
	export CC="clang"
	export CXX="clang++"
	LLVM_PROFDATA_CMD="llvm-profdata"
	LLVM_COV_CMD="llvm-cov"
    else
	export CC="clang-16"
	export CXX="clang++-16"
	LLVM_PROFDATA_CMD="llvm-profdata-16"
	LLVM_COV_CMD="llvm-cov-16"
    fi

    if [ -d "build" ]; then
	rm -r "build"
    fi
    
    cmake --preset clang-build
    cmake --build --preset clang-build -j6
    cd build/tests/
    
    # reference: https://stackoverflow.com/questions/50613601/getting-llvm-cov-to-talk-to-codecov-io

    LLVM_PROFILE_FILE="coverage/%p.profraw" ctest --rerun-failed --output-on-failure




    # Merge coverage files
    $LLVM_PROFDATA_CMD merge -sparse coverage/*.profraw -o coverage/coverage.profdata

    # Generate coverage report
    $LLVM_COV_CMD export -format="lcov" test_* -instr-profile="coverage/coverage.profdata" > ../clang-coverage.info --ignore-filename-regex="external/*"

    cd ../../


elif [ "${TC}" = "gcc" ]; then
    export CC="gcc-13"
    export CXX="g++-13"
    COV="gcov-13"

    if [ -d "gcc-build" ]; then
	rm -r "gcc-build"
    fi

    cmake --preset gcc-build
    cmake --build --preset gcc-build -j6
    cd gcc-build/
    ctest --rerun-failed --output-on-failure

    # Create coverage report - lcov must be installed.
    lcov --directory . --capture --output-file gcc-coverage.info --gcov-tool $COV

    # Exclude system files.
    lcov --remove gcc-coverage.info '/usr/*' '/opt/*' --output-file gcc-coverage.info

    # Exclude external/ folder
    lcov --remove gcc-coverage.info "$BASE_DIR/external/*" --output-file gcc-coverage.info

    cd ..

else
    echo "Error: Invalid toolchain specified in TC variable. Options are 'gcc' or 'clang'."
    exit 1
fi


