#!/bin/bash
if [ "$1" ]
then
  INSTALLDIR="$1"
else
  INSTALLDIR=$(dirname "$0")
fi
INSTALLDIR=/Applications/TestCocoon/

cd "$INSTALLDIR"
if [ ! -e "$INSTALLDIR/coveragescanner" ]
then
  echo "$INSTALLDIR does not containt CoverageScanner executables"
  exit 1
fi

cd "$INSTALLDIR"
ln -s -f coveragescanner csgcc
ln -s -f coveragescanner csg++
ln -s -f coveragescanner csar
ln -s -f coveragescanner csicc
ln -s -f coveragescanner csicpc
rm -f -r wrapper
mkdir -p wrapper

for G in /usr/bin/gcc*
do
  if [ -e "$G" ]
  then
    NAME=$(basename "$G")
    if [ "$NAME" != "gcc" ]
    then
      ln -s -f gcc.cspro "$NAME".cspro 
      ln -s -f coveragescanner cs"$NAME"
    fi
    ln -s -f ../coveragescanner wrapper/"$NAME"
  fi
done

for G in /usr/bin/g++*
do
  if [ -e "$G" ]
  then
    NAME=$(basename "$G")
    if [ "$NAME" != "g++" ]
    then
      ln -s -f g++.cspro "$NAME".cspro 
      ln -s -f coveragescanner cs"$NAME"
    fi
    ln -s -f ../coveragescanner wrapper/"$NAME"
  fi
done
#ln -s -f coveragescanner cslibtool
#ln -s -f ../coveragescanner wrapper/libtool
ln -f -s coveragebrowser.app/Contents/MacOS/cmreport cmreport
ln -f -s coveragebrowser.app/Contents/MacOS/cmmerge cmmerge
ln -f -s coveragebrowser.app/Contents/MacOS/cmcsexeimport cmcsexeimport
exit 0
