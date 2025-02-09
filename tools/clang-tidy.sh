#!/usr/bin/env bash
# Uses run-clang-tidy.py, so clang-tidy must be in your $PATH for this script to run
cd ..
rm -rf build/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
# Settings for tests to run in .clang-tidy
`locate run-clang-tidy.py` -fix
