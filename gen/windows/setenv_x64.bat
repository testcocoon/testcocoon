call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
set QTVERSION=4.7.1
set BUILD_ARCH=x64
set ARCH_SIZE=64
set QT=qt-everywhere-opensource-src-%QTVERSION%
set QTDIR=\qt\%BUILD_ARCH%\%QT%
set PATH=%PATH%;%QTDIR%\bin
