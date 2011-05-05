#!/bin/bash

COMMAND=$@
EXECUTABLE=$1
while [ $# -gt 0 ]; do
    case $1 in
        -o)
            OUTPUT=$2
            shift
            ;;
        *)
            ;;
    esac
    shift
done
$COMMAND
RET=$?
if [ ! -z "$OUTPUT" ]
then
export LC_ALL=en_EN
gprof $EXECUTABLE gmon.out > $OUTPUT.gmon
fi
exit $RET
