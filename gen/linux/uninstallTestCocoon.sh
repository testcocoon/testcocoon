#!/bin/bash
# Uninstallation script of TestCocoon.
# This script must be executed as root.
REQUESTED_USER=$1
DESTDIR=$2


WHOAMI=$(whoami)
COV_VERSION=_log

if [ "$WHOAMI" != "$REQUESTED_USER" ]
then
  echo "This script must be executed as $REQUESTED_USER"
  exit -1;
fi  
_PATH="$PATH":"$DESTDIR/usr/bin":"/scratchbox/compilers/bin/"
while [ ! -z "$_PATH" ]
do
  INDEX=$(expr index "$_PATH" ":")
  if [ $INDEX -gt 0 ]
  then
    LENGTH=$(expr $INDEX - 1)
    ONEPATH=${_PATH:0:$LENGTH}
    _PATH=${_PATH:$INDEX}
  else
    ONEPATH="$_PATH"
    _PATH=""
  fi

    for CSGNUGCC in $ONEPATH/{cs*ar*,cs*gcc*,cs*g++*,cs*ld*}
    do
      if [ -x "$CSGNUGCC" ]
      then
        COVERAGESCANNER="$DESTDIR/opt/TestCocoon/$COV_VERSION/coveragescanner"
        if (cmp -s  "$COVERAGESCANNER"  "$CSGNUGCC" )
        then
          rm "$CSGNUGCC"  2> /dev/null > /dev/null
        fi
      fi
    done

done

rm "/scratchbox/compilers/bin/coveragescanner"                  2> /dev/null > /dev/null
rm "/scratchbox/compilers/bin/*.cspro"                          2> /dev/null > /dev/null
rm  "$DESTDIR/usr/bin/coveragescanner"                          2> /dev/null > /dev/null
rm  "$DESTDIR/usr/bin/coveragebrowser"                          2> /dev/null > /dev/null
rm -r  "$DESTDIR/opt/TestCocoon/$COV_VERSION"                2> /dev/null > /dev/null
rm  "$DESTDIR/opt/TestCocoon/cmregister"                     2> /dev/null > /dev/null
rm  "$DESTDIR/opt/TestCocoon/testcocoon.pdf"              2> /dev/null > /dev/null
rmdir --ignore-fail-on-non-empty  "$DESTDIR/opt/TestCocoon"  2> /dev/null > /dev/null
rmdir --ignore-fail-on-non-empty  "$DESTDIR/opt"                2> /dev/null > /dev/null
rmdir --ignore-fail-on-non-empty  "$DESTDIR"                    2> /dev/null > /dev/null

echo "TestCocoon v$COV_VERSION is removed."
