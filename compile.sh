#!/bin/bash

# Compile ankerl
git clone https://github.com/martinus/unordered_dense.git
cd unordered_dense
rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=./unordered_dense_install ..
cmake --build . --target install
cd ../..

# Compile STEP code
mkdir build
cd build
cmake .. && make

