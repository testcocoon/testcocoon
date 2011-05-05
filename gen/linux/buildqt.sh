#!/bin/bash
QTVERSION=4.6.3
CPUNR=1
if [ -e /proc/cpuinfo ]
then
  CPUS=$(grep -e '^processor.*:.*[0-9]$' /proc/cpuinfo | wc -l)
  if [ "$CPUS" -gt "$CPUNR" ]
  then
    CPUNR=$CPUS
  fi
fi

CURRENTDIR=$PWD
CMD="$1"
if [ "$CMD" != "install" -a ! -e "$CURRENTDIR/coveragebrowser" ]
then
  echo "The current directory is not the installation directory of TestCocoon" 
  exit
fi

( (
TMPDIR=/tmp/QtBuild
mkdir -p $TMPDIR 2> /dev/null > /dev/null
BUILDDIR=$(mktemp -p "$TMPDIR" -d)
QT=qt-everywhere-opensource-src-$QTVERSION
if [ -e $QT.tar.gz ]
then
  QTLOCATION="$CURRENTDIR"/$QT.tar.gz
fi 
if [ -z "$QTLOCATION" ]
then
  QTLOCATION=$(locate "$QT.tar.gz" | head -n 1)
  if [ ! -e "$QTLOCATION" ]
  then
    QTLOCATION=""
  fi
fi
QTPATH=$BUILDDIR/$QT
export PATH=/usr/local/bin:$PATH
cd $BUILDDIR || exit
if [ -z "$QTLOCATION" ]
then
  echo "Downloading $QT ..."
  wget "ftp://ftp.qtsoftware.com/qt/source/$QT.tar.gz" -O "$QT.tar.gz" || wget "ftp://ftp.trolltech.com/qt/source/$QT.tar.gz" -O "$QT.tar.gz" || exit
else
  echo "Copying $QT ..."
  cp -v "$QTLOCATION" "$BUILDDIR" || exit
fi
echo "Unpacking $QTPATH ..."
tar xfz $QT.tar.gz || exit
echo "Configuring $QTPATH ..."
cd $QT || exit
printf 'o\nyes\n' | ./configure -no-qt3support -script -svg -nomake examples -nomake demos -nomake tutorials -no-opengl -no-dbus -no-phonon -no-webkit -fast -no-cups -no-libjpeg -no-libmng -qt-libpng -xrender -xrandr -no-xinerama || exit
echo "Building $QTPATH ..."
make -j${CPUNR} || exit
if [ "$CMD" = "install" ]
then
  echo "Installing $QTPATH ..."
  make install || exit
else
  echo "Copying libraries into TestCocoon Distribution"
  cp -v -f "$BUILDDIR/$QT"/lib/libQtSvg.so.4 "$CURRENTDIR/"
  cp -v -f "$BUILDDIR/$QT"/lib/libQtCore.so.4 "$CURRENTDIR/"
  cp -v -f "$BUILDDIR/$QT"/lib/libQtGui.so.4 "$CURRENTDIR/"
  cp -v -f "$BUILDDIR/$QT"/lib/libQtNetwork.so.4 "$CURRENTDIR/"
fi
echo "Removing $BUILDDIR ..."
rm -rf $BUILDDIR
) 2>&1 ) | tee buildqt.log
