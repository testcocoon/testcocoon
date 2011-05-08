#!/bin/bash -xv
cd $(dirname "$0")
HERE=$PWD

export GCC_VERSION=
export TESTCOCOON_SOURCE_DIR=../..
export TESTCOCOON_BUILD_DIR=$HERE/../../../build


mkdir -p $TESTCOCOON_BUILD_DIR/xcode
pushd $TESTCOCOON_BUILD_DIR/xcode 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -G "Xcode" $TESTCOCOON_SOURCE_DIR 
popd

mkdir -p $TESTCOCOON_BUILD_DIR/debug
pushd $TESTCOCOON_BUILD_DIR/debug 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=debug $TESTCOCOON_SOURCE_DIR 
make coveragescanner
popd

mkdir -p $TESTCOCOON_BUILD_DIR/release
pushd $TESTCOCOON_BUILD_DIR/release 
export COVERAGESCANNER_PATH="${PWD}/testcocoon/src/coveragescanner" 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=release $TESTCOCOON_SOURCE_DIR 
make coveragescanner
popd

mkdir -p $TESTCOCOON_BUILD_DIR/log
pushd $TESTCOCOON_BUILD_DIR/log 
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc$GCC_VERSION) -DCMAKE_CXX_COMPILER=$(which g++$GCC_VERSION) -DCMAKE_BUILD_TYPE=log $TESTCOCOON_SOURCE_DIR 
make coveragescanner
popd

mkdir -p $TESTCOCOON_BUILD_DIR/coverage
pushd $TESTCOCOON_BUILD_DIR/coverage 
cmake -DCMAKE_TOOLCHAIN_FILE=$HERE/toolchain.cmake -DCMAKE_BUILD_TYPE=coverage -G "Unix Makefiles" ../..  || exit -1
popd


pushd $TESTCOCOON_BUILD_DIR/debug 
make
popd

pushd $TESTCOCOON_BUILD_DIR/release 
make
popd

pushd $TESTCOCOON_BUILD_DIR/log 
make
popd

pushd $TESTCOCOON_BUILD_DIR/coverage 
make
popd

