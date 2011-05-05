#!/bin/bash

sectionsize()
{
  SIZE_HEX=$(readelf -S a.out | egrep -e $1 | sed -e 's/^.*'$1' *[^ ]* *[^ ]* *[^ ]* *\([^ ]*\) .*$/\1/g' )
  calc "0x$SIZE_HEX"
}

data_size()
{
  BSS=$(sectionsize "\.bss")
  DATA=$(sectionsize "\.data")
  expr $BSS + $DATA
}

code_size()
{
  sectionsize "\.text"
}



codesize_one()
{
  echo $1 >> codesize.tex  
    rm a.out
  gcc $2 sort.c -o a.out
  echo ' & ~data ' >> codesize.tex
  echo ' & ' >> codesize.tex
  NORMAL_DATA=$(data_size)
  echo $NORMAL_DATA >> codesize.tex  

  echo ' & ' >> codesize.tex
    rm a.out
  ../coveragescanner/csgcc $2 sort.c -o a.out
  MES_DATA=$(data_size)
  echo $MES_DATA >> codesize.tex  
  echo ' & ' >> codesize.tex
  calc "int ($MES_DATA - $NORMAL_DATA)" >> codesize.tex
  echo ' \\ ' >> codesize.tex

  echo ' & ~code ' >> codesize.tex
  echo ' & ' >> codesize.tex
  gcc $2 sort.c -o a.out
  NORMAL_CODE=$(code_size)
  echo $NORMAL_CODE >> codesize.tex  

  echo ' & ' >> codesize.tex
    rm a.out
  ../coveragescanner/csgcc $2 sort.c -o a.out
  MES_CODE=$(code_size)
  echo $MES_CODE >> codesize.tex  
  echo ' & ' >> codesize.tex
  calc "int ($MES_CODE - $NORMAL_CODE)" >> codesize.tex
  echo ' \\ ' >> codesize.tex
  echo ' \hline ' >> codesize.tex
}

echo '\begin{tabularx}{\linewidth}{|cl|X|X|X|}' > codesize.tex
echo ' \hline ' >> codesize.tex
echo '\multicolumn{2}{|c|}{\textbf{Compiler} } & ' >> codesize.tex
echo '\textbf{Without Instrumentation}  \hvanewline (bytes) & ' >> codesize.tex
echo '\textbf{With Instrumentation} \hvanewline (bytes) & ' >> codesize.tex
echo '\textbf{Difference} \hvanewline (bytes)  \\ ' >> codesize.tex
echo ' \hline ' >> codesize.tex
echo ' \hline ' >> codesize.tex
codesize_one "GCC without optimization" ""
codesize_one "GCC with optimization -Os" "-Os"
codesize_one "GCC with optimization -O1" "-O1"
codesize_one "GCC with optimization -O2" "-O2"
codesize_one "GCC with optimization -O3" "-O3"
echo '\end{tabularx}' >> codesize.tex
exit 0
