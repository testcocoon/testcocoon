#!/bin/bash -xv
HERE=$PWD
cd ../..
export GCC_VERSION=


mkdir -p build/xcode
pushd build/xcode 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -G "Xcode" ../.. 
popd

mkdir -p build/debug
pushd build/debug 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=debug ../.. 
make coveragescanner
popd

mkdir -p build/release
pushd build/release 
export COVERAGESCANNER_PATH="${PWD}/src/coveragescanner" 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=release ../.. 
make coveragescanner
popd

mkdir -p build/log
pushd build/log 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=log ../.. 
make coveragescanner
popd

mkdir -p build/coverage
pushd build/coverage 
cmake -DCMAKE_TOOLCHAIN_FILE=$HERE/toolchain.cmake -DCMAKE_BUILD_TYPE=coverage -G "Unix Makefiles" ../..  || exit -1
popd


pushd build/debug 
make
popd

pushd build/release 
make
popd

pushd build/log 
make
popd

pushd build/coverage 
make
popd

