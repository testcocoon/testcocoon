#!/bin/bash
# Installation script of TestCocoon.

WHOAMI=$(whoami)
COV_VERSION=2.2.1_log

echo
echo "TestCocoon v$COV_VERSION"
echo "1) Installation on the local machine (need to be root)"
echo "2) Deinstallation from the local machine (need to be root)"
echo "3) Installation on the home directory"
echo "4) Deinstallation from the home directory"
echo "q) quit"

while (true)
do
  read -s -n 1 KEY

  case "$KEY" in
  "1") 
  if [ "$WHOAMI" != "root" ]
  then
    echo "Enter root password"
    su -c "./installTestCocoon.sh root /"
  else
    ./installTestCocoon.sh root /
  fi
  exit
  ;;

  "2") 
  if [ "$WHOAMI" != "root" ]
  then
    echo "Enter root password"
    su -c "./uninstallTestCocoon.sh root /"
  else
    ./uninstallTestCocoon.sh root /
  fi
  exit
  ;;

  "3") 
  ./installTestCocoon.sh $(whoami) "$HOME/TestCocoon"
  echo "please add $HOME/TestCocoon/usr/bin/ to the PATH variable"
  echo "and $HOME/TestCocoon/opt/TestCocoon/$COV_VERSION/ to the LD_LIBRARY_PATH variable."
  echo "Example:"
  echo "  export PATH=$HOME/TestCocoon/usr/bin/:"'$PATH'
  echo "  export LD_LIBRARY_PATH=$HOME/TestCocoon/opt/TestCocoon/$COV_VERSION/:"'$LD_LIBRARY_PATH'
  exit
  ;;

  "4") 
  ./uninstallTestCocoon.sh $(whoami) "$HOME/TestCocoon"
  exit
  ;;

  "q") 
  exit
  ;;

  esac
done
