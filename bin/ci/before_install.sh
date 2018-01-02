#!/bin/bash
# Installs requirements for spatial_index_benchmark
source ./bin/ci/common.sh
gcc --version
clang --version
cmake --version
echo "$(tmstamp) *** before_install::svn co boost starting $(date) ***"
# Boost 1.55+ or trunk is required
mkdir -p ${BOOST_PREFIX}
echo "Running svn co ${BOOST_SVN} ${BOOST_HEADERS}"
svn checkout ${BOOST_SVN} ${BOOST_HEADERS} > /dev/null
echo "$(tmstamp) *** before_install::svn co boost finished  $(date) ***"

echo "Building test examples"
cd test
mkdir build && cd build && cmake ..
make
cd ../../
