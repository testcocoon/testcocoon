call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
set QTVERSION=4.6.3
set BUILD_ARCH=x86
set ARCH_SIZE=32
set QT=qt-everywhere-opensource-src-%QTVERSION%
set QTDIR=\qt\%BUILD_ARCH%\%QT%
set PATH=%PATH%;%QTDIR%\bin
