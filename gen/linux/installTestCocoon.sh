#!/bin/bash
# Installation script of TestCocoon.
# This script must be executed as root.
REQUESTED_USER=$1
DESTDIR=$2

WHOAMI=$(whoami)
COV_VERSION=2.2.1_log

function createwrapper()
{
TOOLNAME=$1
_PATH="$2"
_PROFILEDIR="$3"
_COVERAGESCANNER="$4"
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

    for GNUGCC in $ONEPATH/{*-$TOOLNAME,$TOOLNAME,$TOOLNAME-*,*-$TOOLNAME-*}
    do
      if [ -x "$GNUGCC" ]
      then 
        BASENAMEGNUGCC=$(basename $GNUGCC)
        if [ ${BASENAMEGNUGCC:0:2} != "cs" ]
        then
          if [ "$DESTDIR" == "/" ]
          then
            DESTCS="$ONEPATH"/cs"$BASENAMEGNUGCC"
          else
            DESTCS="$DESTDIR/usr/bin/"/cs"$(basename $GNUGCC)"
          fi
          PROFILE="$_PROFILEDIR"/"$BASENAMEGNUGCC.cspro"
          ln -s -f "$_COVERAGESCANNER" "$DESTCS"
          if [ ! -e "$PROFILE" ]
          then
            sed -e 's/COMPILER=.*$/COMPILER='$BASENAMEGNUGCC'/g' "$DESTDIR/opt/TestCocoon/$COV_VERSION"/$TOOLNAME.cspro > $PROFILE
            chmod 0644 $PROFILE
          fi
        fi
      fi
    done

done
}

if [ "$WHOAMI" != "$REQUESTED_USER" ]
then
  echo "This script must be executed as $REQUESTED_USER"
  exit -1;
fi  
mkdir "$DESTDIR" 2> /dev/null > /dev/null
tar xf TestCocoonInstall.tar -C "$DESTDIR"
COVERAGESCANNER="$DESTDIR/opt/TestCocoon/$COV_VERSION/coveragescanner"

#General installation
createwrapper ar  "$PATH" "$DESTDIR/opt/TestCocoon/$COV_VERSION" "$COVERAGESCANNER"
createwrapper ld  "$PATH" "$DESTDIR/opt/TestCocoon/$COV_VERSION" "$COVERAGESCANNER"
createwrapper gcc "$PATH" "$DESTDIR/opt/TestCocoon/$COV_VERSION" "$COVERAGESCANNER"
createwrapper g++ "$PATH" "$DESTDIR/opt/TestCocoon/$COV_VERSION" "$COVERAGESCANNER"

#Scratchbox
if [ "root" = "$REQUESTED_USER" ]
then
  SCRATCHBOX="/scratchbox/compilers/bin" 
  if [ -d "$SCRATCHBOX" ]
  then
    cp $COVERAGESCANNER "$SCRATCHBOX"/coveragescanner
    createwrapper ar  "$SCRATCHBOX" "$SCRATCHBOX" "coveragescanner"
    createwrapper ld  "$SCRATCHBOX" "$SCRATCHBOX" "coveragescanner"
    createwrapper gcc "$SCRATCHBOX" "$SCRATCHBOX" "coveragescanner"
    createwrapper g++ "$SCRATCHBOX" "$SCRATCHBOX" "coveragescanner"
    echo "TestCocoon is installed on Scratchbox"
  fi
fi

echo "TestCocoon v$COV_VERSION is installed."
