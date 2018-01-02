#!/bin/bash

./test/build/test

# Builds and runs spatial_index_benchmark
source ./bin/ci/common.sh
cd benchmark
mkdir -p _build
cd _build
echo "Calling cmake -DBOOST_PREFIX=${BOOST_PREFIX}"
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBOOST_ROOT=${BOOST_PREFIX} \
    -DBGI_ENABLE_CT=ON \
    ..
#cmake --build .
make -j ${NUMTHREADS}

# Benchmarks run takes long time, skip
#ctest -C Release -V --output-on-failure .  
