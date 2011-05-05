#!/bin/bash
GENERATION_CONFIG="CONFIG-=debug CONFIG+=release CONFIG-=log"
#GENERATION_CONFIG="CONFIG+=debug CONFIG-=release CONFIG+=log"
rm -rf /tmp/TestCocoon
( (
pushd ../libcsmes/
qmake $GENERATION_CONFIG
make clean
make 
popd

pushd ../coveragescanner/
qmake $GENERATION_CONFIG
make clean
make 
popd

pushd ../libcsmes/
qmake  CONFIG-=debug CONFIG+=release
make clean
make CC=$PWD/../coveragescanner/csgcc CXX=$PWD/../coveragescanner/csg++ LINK=$PWD/../coveragescanner/csg++ AR="$PWD/../coveragescanner/csar cqs"
popd

pushd ../coveragebrowser/
qmake  CONFIG-=debug CONFIG+=release
make clean
make CC=$PWD/../coveragescanner/csgcc CXX=$PWD/../coveragescanner/csg++ LINK=$PWD/../coveragescanner/csg++
popd
)  2>&1 ) | tee /tmp/coveragebrowser.log
