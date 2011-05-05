#!/bin/bash
FILE=$1
echo -n "Number of conflicts:"
egrep -e "\[[^'].*\]" $FILE | wc -l
echo "List of token which are concerned with conflicts:"
echo -n '    '
declare -a TOKEN_LIST
TOKEN_LIST=$(egrep -e "\[[^'].*\]" $FILE | sed -e 's/\[[^'"'"'].*$//g' | sort -u)
echo $TOKEN_LIST
TOKEN_LIST=$TOKEN_LIST' %'

TOKEN=$(echo $TOKEN_LIST | sed -e 's/ .*//')
TOKEN_LIST=$(echo $TOKEN_LIST | sed -e 's/[^ ]* //')
while [ "$TOKEN" != "%" ]
do
echo "List of rules for:$TOKEN"
TOKEN_EXPR=$TOKEN
TOKEN_EXPR=$(echo $TOKEN_EXPR | sed -e 's/\[/\\[/g')
TOKEN_EXPR=$(echo $TOKEN_EXPR | sed -e 's/\*/\\*/g')
TOKEN_EXPR=$(echo $TOKEN_EXPR | sed -e 's/(/\\(/g')
TOKEN_EXPR=$(echo $TOKEN_EXPR | sed -e 's/)/\\)/g')
echo "  Reduce"
egrep -e '^  *'"$TOKEN_EXPR"' *\[' $FILE | sort -u | egrep -e "(règle|rule)"
echo "  Shift"
egrep -e '^  *'"$TOKEN_EXPR"' *\[' $FILE | sort -u | egrep -e "(state|état)"

TOKEN=$(echo $TOKEN_LIST | sed -e 's/ .*//')
TOKEN_LIST=$(echo $TOKEN_LIST | sed -e 's/[^ ]* //')
done

