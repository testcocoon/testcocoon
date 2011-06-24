#!/bin/bash -xv
QTVERSION=4.6.3

PROCESSORS=$(cat /proc/cpuinfo | grep processor | wc -l)
HERE=$(dirname "$0")
pushd $HERE
HERE=$PWD
popd
#DISTRIB_DIR=$HERE/../../build_distrib
DISTRIB_DIR=$(mktemp -d /tmp/testcocoon_build_XXXXX)
SOURCE_DIR=$HERE/../..
BINARYDIR=$SOURCE_DIR/binaries/ 
GEN_DIR=$HERE
#rm -rf $DISTRIB_DIR
DISTRIB_LOG_DIR=$DISTRIB_DIR/log
DISTRIB_RELEASE_DIR=$DISTRIB_DIR/release

function findlib()
{
  LIBTOEXCLUDE=( "/usr/lib/[^/]*$" "/lib/tls/.*$" "/lib/[^/]*$" )
  LIBS=$(ldd "$1" | sed -e 's/.*=> *//g' | sed -e 's/ *(.*) *$//g' | sed -e 's/[ \t]//g' )
  LIBSRES=""
  for LIB in ${LIBS[@]}
  do
    FOUND=0
    for EXCLUDE in ${LIBTOEXCLUDE[@]}
    do
      MATCH=$(expr match "$LIB" "$EXCLUDE")
      if [ "$MATCH" -gt 0 ]
      then
        FOUND=1
      fi
    done
    if [ $FOUND = 0 ]
    then
      LIBSRES="$LIBSRES $LIB"
    fi
  done

  echo $LIBSRES
}
function install_exec()
{
  SOURCE="$1"
  DESTDIR="$2"
  DEST="$DESTDIR/"$(basename "$SOURCE")
  fakeroot install -m 0755 "$SOURCE" "$DEST" || exit
  LIBS=$(findlib "$SOURCE")
  for LIB in ${LIBS[@]}
  do
    DESTLIB="$DESTDIR/"$(basename $LIB)
    if [ ! -e "$DESTLIB" ]
    then
      fakeroot install -m 0755 "$LIB" "$DESTLIB" || exit
    fi
  done
}

export QTDIR=/usr/local/Trolltech/Qt-${QTVERSION}
QTLIB=$QTDIR/lib/
export PATH=$QTDIR/bin:$PATH

function build_testcocoon()
{
  set -xv
  mkdir -p $DISTRIB_LOG_DIR
  cd $DISTRIB_LOG_DIR
  CC=gcc CXX=g++ cmake -C /dev/null -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=log $SOURCE_DIR
  make -j$PROCESSORS coveragescanner || exit -1

  mkdir -p $DISTRIB_RELEASE_DIR
  cd $DISTRIB_RELEASE_DIR
  CC=gcc CXX=g++ cmake -C /dev/null -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=release $SOURCE_DIR
  make -j$PROCESSORS || exit -1
  make pdf || exit -1
}

cd $HERE
build_testcocoon
source $SOURCE_DIR/src/commoncode/version.sh || exit -1
BUILDROOT=/tmp/testcocoon$VERSION


rm -fr ${BUILDROOT}
fakeroot mkdir -p ${BUILDROOT}
fakeroot install -m 0755 -d ${BUILDROOT}/usr || exit
fakeroot install -m 0755 -d ${BUILDROOT}/usr/bin || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt/TestCocoon || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt/TestCocoon/gccwrapper || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt/TestCocoon/${VERSION} || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt/TestCocoon/${VERSION}/iconengines || exit
fakeroot install -m 0755 "$QTDIR/plugins/iconengines/libqsvgicon.so" ${BUILDROOT}/opt/TestCocoon/${VERSION}/iconengines/libqsvgicon.so || exit
fakeroot install -m 0755 -d ${BUILDROOT}/opt/TestCocoon/${VERSION}/imageformats || exit
fakeroot install -m 0755 "$QTDIR/plugins/imageformats/libqsvg.so" ${BUILDROOT}/opt/TestCocoon/${VERSION}/imageformats/libqsvg.so || exit
fakeroot install -m 0755 "$QTDIR/lib/libQtXml.so.4" ${BUILDROOT}/opt/TestCocoon/${VERSION}/libQtXml.so.4 || exit
install_exec $DISTRIB_RELEASE_DIR/src/coveragescanner/coveragescanner ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit
install_exec $DISTRIB_RELEASE_DIR/src/coveragebrowser/coveragebrowser ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/libcoveragebrowser/French.qm ${BUILDROOT}/opt/TestCocoon/${VERSION}/French.qm || exit
install_exec $DISTRIB_RELEASE_DIR/src/cmmerge/cmmerge ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit
install_exec $DISTRIB_RELEASE_DIR/src/cmcsexeimport/cmcsexeimport ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit
install_exec $DISTRIB_RELEASE_DIR/src/cmreport/cmreport ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/doc/testcocoon.pdf ${BUILDROOT}/opt/TestCocoon/${VERSION}/testcocoon.pdf || exit
fakeroot install -m 0644 $SOURCE_DIR/src/LICENSE.TXT ${BUILDROOT}/opt/TestCocoon/${VERSION}/LICENSE.TXT || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/g++.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/g++.cspro || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/ar.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/ar.cspro || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/ld.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/ld.cspro || exit
fakeroot install -m 0755 $SOURCE_DIR/gen/linux/buildqt.sh ${BUILDROOT}/opt/TestCocoon/${VERSION}/buildqt.sh || exit
#fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/remotecontrol/coveragebrowserapi.h ${BUILDROOT}/opt/TestCocoon/${VERSION}/coveragebrowserapi.h || exit
#pushd $SOURCE_DIR/remotecontrol/
#for I in libcoveragebrowserapi.so*
#do
#fakeroot install -m 0644 $SOURCE_DIR/remotecontrol/$I ${BUILDROOT}/opt/TestCocoon/${VERSION}/$I || exit
#done
#popd 
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/gcc.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/gcc.cspro || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/icpc.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/icpc.cspro || exit
fakeroot install -m 0644 $DISTRIB_RELEASE_DIR/src/coveragescanner/icc.cspro ${BUILDROOT}/opt/TestCocoon/${VERSION}/icc.cspro || exit
fakeroot ln -s -f ../../opt/TestCocoon/${VERSION}/cmmerge ${BUILDROOT}/usr/bin/cmmerge || exit
fakeroot ln -s -f ../../opt/TestCocoon/${VERSION}/cmcsexeimport ${BUILDROOT}/usr/bin/cmcsexeimport || exit
fakeroot ln -s -f ../../opt/TestCocoon/${VERSION}/cmreport ${BUILDROOT}/usr/bin/cmreport || exit
fakeroot ln -s -f ${VERSION}/testcocoon.pdf ${BUILDROOT}/opt/TestCocoon/testcocoon.pdf || exit
fakeroot ln -s -f ../../opt/TestCocoon/${VERSION}/coveragebrowser ${BUILDROOT}/usr/bin/coveragebrowser || exit
fakeroot ln -s -f ../../opt/TestCocoon/${VERSION}/coveragescanner ${BUILDROOT}/usr/bin/coveragescanner || exit

fakeroot ln -s -f  coveragescanner ${BUILDROOT}/usr/bin/csgcc || exit
fakeroot ln -s -f  coveragescanner ${BUILDROOT}/usr/bin/csicc || exit
fakeroot ln -s -f  coveragescanner ${BUILDROOT}/usr/bin/csicpc || exit
fakeroot ln -s -f  coveragescanner ${BUILDROOT}/usr/bin/csg++ || exit
fakeroot ln -s -f  coveragescanner ${BUILDROOT}/usr/bin/csar || exit

fakeroot ln -s -f  ../${VERSION}/coveragescanner ${BUILDROOT}/opt/TestCocoon/gccwrapper/ar || exit
fakeroot ln -s -f  ../${VERSION}/coveragescanner ${BUILDROOT}/opt/TestCocoon/gccwrapper/gcc || exit
fakeroot ln -s -f  ../${VERSION}/coveragescanner ${BUILDROOT}/opt/TestCocoon/gccwrapper/g++ || exit

mv $DISTRIB_LOG_DIR/src/coveragescanner/coveragescanner $DISTRIB_LOG_DIR/src/coveragescanner/coveragescannerdbg || exit
install_exec $DISTRIB_LOG_DIR/src/coveragescanner/coveragescannerdbg ${BUILDROOT}/opt/TestCocoon/${VERSION}/ || exit

# Create of the tar file
fakeroot rm -r /tmp/TestCocoonInstall_$VERSION
fakeroot mkdir /tmp/TestCocoonInstall_$VERSION || exit
pushd ${BUILDROOT}
fakeroot tar cvf /tmp/TestCocoonInstall_$VERSION/TestCocoonInstall.tar . || exit
popd
fakeroot sed -e  's/COV_VERSION=.*/COV_VERSION='$VERSION'/g' $GEN_DIR/installTestCocoon.sh > /tmp/TestCocoonInstall_$VERSION/installTestCocoon.sh || exit
fakeroot sed -e  's/COV_VERSION=.*/COV_VERSION='$VERSION'/g' $GEN_DIR/uninstallTestCocoon.sh > /tmp/TestCocoonInstall_$VERSION/uninstallTestCocoon.sh  || exit
fakeroot sed -e  's/COV_VERSION=.*/COV_VERSION='$VERSION'/g' $GEN_DIR/setupMenu.sh > /tmp/TestCocoonInstall_$VERSION/setupMenu.sh  || exit
chmod +x /tmp/TestCocoonInstall_$VERSION/*.sh
pushd /tmp
fakeroot tar cvfj TestCocoonInstall_$VERSION.tar.bz2 TestCocoonInstall_$VERSION || exit
fakeroot makeself --bzip2  TestCocoonInstall_$VERSION  TestCocoonSetup_$VERSION.run  "TestCocoon $VERSION" ./setupMenu.sh || exit
popd
#cp /tmp/TestCocoonInstall_$VERSION.tar.bz2 ../binaries/
mkdir -p $BINARYDIR 
cp /tmp/TestCocoonSetup_$VERSION.run $BINARYDIR/ || exit -1
echo "Finished!"
exit 0
