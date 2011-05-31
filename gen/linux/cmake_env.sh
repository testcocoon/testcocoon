#!/bin/bash 
#QTDIR=/usr/local/Trolltech/Qt-4.6.3
#PATH=$QTDIR/bin:$PATH
DIR=$(dirname $0)
HERE=$(realpath "$DIR")
PROCESSORS=$(cat /proc/cpuinfo | grep processor | wc -l)
TESTCOCOON_TOP_SOURCE=../..
TESTCOCOON_TOP_OUTPUT=$HERE/../../../build/

mkdir -p $TESTCOCOON_TOP_OUTPUT/debug
pushd $TESTCOCOON_TOP_OUTPUT/debug || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=debug $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/release
pushd $TESTCOCOON_TOP_OUTPUT/release  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=release $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/log
pushd $TESTCOCOON_TOP_OUTPUT/log  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=log $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/profiler
pushd $TESTCOCOON_TOP_OUTPUT/profiler  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=profiler $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/duma
pushd $TESTCOCOON_TOP_OUTPUT/duma  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=duma $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/efence
pushd $TESTCOCOON_TOP_OUTPUT/efence  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=efence $TESTCOCOON_TOP_SOURCE 
make -j$PROCESSORS coveragescanner
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/coverage
pushd $TESTCOCOON_TOP_OUTPUT/coverage  || exit -1
export COVERAGESCANNER_PATH=$(realpath "$TESTCOCOON_TOP_OUTPUT/release/testcocoon/src/coveragescanner")
cmake -DCMAKE_BUILD_TYPE=coverage -DCMAKE_TOOLCHAIN_FILE=$HERE/toolchain.cmake -G "Unix Makefiles" $TESTCOCOON_TOP_SOURCE  || exit
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/coverage_debug
pushd $TESTCOCOON_TOP_OUTPUT/coverage_debug  || exit -1
export COVERAGESCANNER_PATH=$(realpath "$TESTCOCOON_TOP_OUTPUT/debug/testcocoon/src/coveragescanner")
cmake -DCMAKE_BUILD_TYPE=coverage -DCMAKE_TOOLCHAIN_FILE=$HERE/toolchain.cmake -G "Unix Makefiles" $TESTCOCOON_TOP_SOURCE  || exit
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/coverage_log
pushd $TESTCOCOON_TOP_OUTPUT/coverage_log  || exit -1
export COVERAGESCANNER_PATH=$(realpath "$TESTCOCOON_TOP_OUTPUT/log/testcocoon/src/coveragescanner")
cmake -DCMAKE_BUILD_TYPE=coverage -DCMAKE_TOOLCHAIN_FILE=$HERE/toolchain.cmake -G "Unix Makefiles" $TESTCOCOON_TOP_SOURCE  || exit
popd

mkdir -p $TESTCOCOON_TOP_OUTPUT/genimgsrc
pushd $TESTCOCOON_TOP_OUTPUT/genimgsrc  || exit -1
cmake -C /dev/null -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_BUILD_TYPE=release -DGENSRCIMG=1 $TESTCOCOON_TOP_SOURCE 
popd

