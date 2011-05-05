#!/bin/bash
BUILDDIR=$(mktemp -d /tmp/QtBuildXXXXXX)
QTVERSION=4.6.3

QT=qt-everywhere-opensource-src-$QTVERSION
export PATH=/usr/local/bin:$PATH
mkdir -p $BUILDDIR 
cd $BUILDDIR || exit
echo "Unpacking $QT ..."
ssh sfri@firewall.vpn "cat /home/NET/TROLLTECH/$QT.tar.gz" | tar -x -f - -z -v || exit
echo "Configuring $QT ..."
cd $QT || exit
#-sdk /Developer/SDKs/MacOSX10.4u.sdk 
printf 'o\nyes\n'  | ./configure  -no-exceptions -arch x86_64 -platform macx-g++ -nomake examples -nomake demos -nomake tutorials -no-opengl -no-openssl -no-dbus -no-phonon -no-webkit -fast -no-cups -no-libjpeg -no-libmng -svg -qt-libpng -no-xinerama || exit
echo "Building $QT ..."
 GCC_VERSION=-4.0
 CC=$CC$GCC_VERSION
 LINK=$LINK$GCC_VERSION
 CXX=$CXX$GCC_VERSION
make -j2 || exit
echo "Installing $QT ..."
sudo rm -rf /usr/local/Trolltech/Qt-$QTVERSION
sudo make install || exit
echo "Archiving $QT"
cd /
#tar cvfj /usr/local/Trolltech/Qt-$QTVERSION.tbz2 /usr/local/Trolltech/Qt-$QTVERSION
echo "Removing $QT ..."
rm -rf $QT
echo "Installing $QT ... The  End"
