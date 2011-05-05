#!/bin/bash
ranlib "$1"
if [ -e "$1.csmes" ]
then
  touch "$1.csmes"
fi
