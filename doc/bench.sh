#!/bin/bash
NB_LOOP=1
FILE=$1
REF="none"

bench_one()
{
  echo $1 >> $FILE  
    rm a.out
  gcc $2 sort.c -o a.out
  echo ' & ' >> $FILE

  echo -n "Native:$1"
  NORMAL=$(./a.out)
  echo
  if [ "$REF" == "none" ]
  then
    REF=$NORMAL
  fi

  calc "( int (( 1000 * $NORMAL ) / $REF) ) / 10.0" >> $FILE

  echo ' & ' >> $FILE
    rm a.out
  ../coveragescanner/csgcc $2 sort.c -o a.out
  echo -n "Mes:$1"
  MES=$(./a.out)
  echo
  calc "( int (( 1000 * $MES ) / $REF) ) / 10.0" >> $FILE

  echo ' & ' >> $FILE
  calc "( int (( 1000 * ( $MES - $NORMAL ) ) / $NORMAL) ) / 10.0" >> $FILE
  echo ' \\ ' >> $FILE
  echo ' \hline ' >> $FILE
}

echo '\begin{tabularx}{\linewidth}{|l|X|X|X|}' > $FILE
echo ' \hline ' >> $FILE
echo '\textbf{Compiler} &  ' >> $FILE
echo '\textbf{Normal Execution} \hvanewline (time) &  ' >> $FILE
echo '\textbf{Execution with Instrumentation} \hvanewline (time)  & ' >> $FILE
echo '\textbf{Difference} \hvanewline (\%) \\ ' >> $FILE
echo ' \hline ' >> $FILE
echo ' \hline ' >> $FILE
bench_one "GCC without optimization" ""
bench_one "GCC with optimization -Os" "-Os"
bench_one "GCC with optimization -O1" "-O1"
bench_one "GCC with optimization -O2" "-O2"
bench_one "GCC with optimization -O3" "-O3"
echo '\end{tabularx}' >> $FILE
exit 0
