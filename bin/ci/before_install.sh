#!/bin/bash
# Installs requirements for spatial_index_benchmark
source ./bin/ci/common.sh
gcc --version
clang --version
cmake --version

sudo apt-get install libboost-all-dev

echo "Building test examples"
cd test
mkdir build && cd build && cmake ..
make
cd ../../
