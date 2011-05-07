#!/bin/bash +xv
QT=4.6.3
HERE=$(dirname "$0")
pushd $HERE
HERE=$PWD
popd
WORKINGDIR=$HERE
BINARYDIR=$WORKINGDIR/../../../binaries 
SNAPSHOTDIR=$WORKINGDIR/TestCocoon
DISTRIB_DIR=$WORKINGDIR/../../build_distrib/
DISTRIB_RELEASE_DIR=$DISTRIB_DIR/release
DISTRIB_LOG_DIR=$DISTRIB_DIR/log
COVERAGEBROWSER=$WORKINGDIR/../../coveragebrowser/coveragebrowser.app
QTDIR=/usr/local/Trolltech/Qt-$QT
QTLIB=$QTDIR/lib
QTLIBRELOC=$QTDIR/lib
QTPLUGINS=$QTDIR/plugins
QMAKE=$QTDIR/bin/qmake
export PATH=$QTDIR/bin:$PATH

#QTLIB=/Library/Frameworks
#QTLIBRELOC=
#QTPLUGINS=/Developer/Applications/Qt/plugins
#QMAKE=/usr/bin/qmake
#rm -rf $DISTRIB_DIR

cd $WORKINGDIR/../macos


function build_testcocoon()
{
  set -xv
  mkdir -p $DISTRIB_LOG_DIR
  cd $DISTRIB_LOG_DIR || exit -1
  cmake -C /dev/null -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=log ../.. || exit -1
  #make clean || exit -1
  make -j2 coveragescanner || exit -1 || exit -1

  mkdir -p $DISTRIB_RELEASE_DIR
  cd $DISTRIB_RELEASE_DIR
  cmake -C /dev/null -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=release ../.. || exit -1
  #make clean || exit -1
  make -j2 || exit -1
  make pdf || exit -1
}

function build_package()
{
  set -xv
  source $WORKINGDIR/../../src/commoncode/version.sh || exit -1
  FREEZE=/usr/local/bin/freeze
  cd $WORKINGDIR || exit -1
  rm -rf $WORKINGDIR/../../binaries/TestCocoonSetup_$VERSION.pkg  || exit -1
  cp TestCocoon.packproj  TestCocoon_tmp.packproj || exit -1
  sed -i -e 's/VERSION/'$VERSION'/g' TestCocoon_tmp.packproj || exit -1
  sed -i -e 's#MACOSPATH#'$WORKINGDIR'#g' TestCocoon_tmp.packproj || exit -1
  $FREEZE -v TestCocoon_tmp.packproj  || exit -1
  mv $WORKINGDIR/../../binaries/TestCocoon.pkg $WORKINGDIR/../../binaries/TestCocoonSetup_$VERSION.pkg  || exit -1
}

function patch_qt_executable()
{
  set -xv
  EXEC="$1"
  QTLIBPATH="$2"
  install_name_tool -change ${QTLIBRELOC}QtXml.framework/Versions/4/QtXml "$QTLIBPATH"/QtXml "$EXEC" || exit -1
  install_name_tool -change ${QTLIBRELOC}QtNetwork.framework/Versions/4/QtNetwork "$QTLIBPATH"/QtNetwork "$EXEC" || exit -1
  install_name_tool -change ${QTLIBRELOC}QtGui.framework/Versions/4/QtGui "$QTLIBPATH"/QtGui "$EXEC" || exit -1
  install_name_tool -change ${QTLIBRELOC}QtSvg.framework/Versions/4/QtSvg "$QTLIBPATH"/QtSvg "$EXEC" || exit -1
  install_name_tool -change ${QTLIBRELOC}QtCore.framework/Versions/4/QtCore  "$QTLIBPATH"/QtCore "$EXEC" || exit -1

  otool -L "$EXEC"
}

function create_snapshot()
{
  set -xv
  cd $WORKINGDIR || exit -1
  rmdir $SNAPSHOTDIR
  mkdir -p $SNAPSHOTDIR
  cd $SNAPSHOTDIR || exit -1

  cp -r $DISTRIB_RELEASE_DIR/src/coveragebrowser/coveragebrowser.app $SNAPSHOTDIR/ || exit -1
  mkdir -p $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  cp -f $QTLIB/QtXml.framework/Versions/4/QtXml $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  install_name_tool -id @executable_path/../Frameworks/QtXml $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtXml || exit -1

  cp $QTLIB/QtGui.framework/Versions/4/QtGui $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  install_name_tool -id @executable_path/../Frameworks/QtGui $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtGui || exit -1

  cp $QTLIB/QtSvg.framework/Versions/4/QtSvg $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  install_name_tool -id @executable_path/../Frameworks/QtSvg $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtSvg || exit -1

  cp $QTLIB/QtNetwork.framework/Versions/4/QtNetwork $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  install_name_tool -id @executable_path/../Frameworks/QtNetwork $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtNetwork || exit -1

  cp $QTLIB/QtCore.framework/Versions/4/QtCore $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/ || exit -1
  install_name_tool -id @executable_path/../Frameworks/QtCore $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtCore || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtNetwork @executable_path/../Frameworks || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtGui @executable_path/../Frameworks || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtXml @executable_path/../Frameworks || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtSvg @executable_path/../Frameworks || exit 0
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/Frameworks/QtCore @executable_path/../Frameworks || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/coveragebrowser @executable_path/../Frameworks || exit -1

  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/coveragescanner $SNAPSHOTDIR/ || exit -1
  cp $DISTRIB_LOG_DIR/src/coveragescanner/coveragescanner $SNAPSHOTDIR/coveragescannerdbg || exit -1


  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/ar.cspro $SNAPSHOTDIR/ || exit -1
  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/gcc.cspro $SNAPSHOTDIR/ || exit -1
  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/g++.cspro $SNAPSHOTDIR/ || exit -1
  #cp $DISTRIB_RELEASE_DIR/src/coveragescanner/libtool.cspro $SNAPSHOTDIR/ || exit -1
  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/icc.cspro $SNAPSHOTDIR/ || exit -1
  cp $DISTRIB_RELEASE_DIR/src/coveragescanner/icpc.cspro $SNAPSHOTDIR/ || exit -1

  cp $DISTRIB_RELEASE_DIR/src/cmmerge/cmmerge $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/ || exit -1
  cp $DISTRIB_RELEASE_DIR/src/cmcsexeimport/cmcsexeimport $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/ || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/cmcsexeimport @executable_path/../Frameworks|| exit -1

  cp $DISTRIB_RELEASE_DIR/src/cmreport/cmreport $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/ || exit -1
  patch_qt_executable $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/cmreport @executable_path/../Frameworks || exit -1
  cp $DISTRIB_RELEASE_DIR/doc/testcocoon.pdf $SNAPSHOTDIR/ || exit -1
  #cp $DISTRIB_RELEASE_DIR/macos/coveragescanner_install.sh $SNAPSHOTDIR/ || exit -1
  cp $WORKINGDIR/xcode.sh $SNAPSHOTDIR/ || exit -1
  $QTDIR//bin/macdeployqt $SNAPSHOTDIR/coveragebrowser.app

#mkdir $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/iconengines/ || exit -1
#cp $QTPLUGINS/iconengines/libqsvgicon.dylib $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/iconengines/ || exit -1
#mkdir $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/imageformats/ || exit -1
#cp $QTPLUGINS/imageformats/libqsvg.dylib $SNAPSHOTDIR/coveragebrowser.app/Contents/MacOS/imageformats/ || exit -1
}

function create_tbz2()
{
    [ -e $BINARYDIR ]  || mkdir -p $BINARYDIR
    cd $BINARYDIR || exit -1
    source $WORKINGDIR/../../src/commoncode/version.sh || exit -1
    TBZ=TestCocoonSetupMacOs_$VERSION.pkg.tbz2 
    rm -f "$TBZ"
    tar cvfj "$TBZ" TestCocoonSetup_$VERSION.pkg  || exit -1
}

build_testcocoon
create_snapshot
build_package
create_tbz2
