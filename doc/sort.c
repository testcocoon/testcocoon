#define SIZE 10000000
#define NB_TESTS 4
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

void sort (int array[],int size); 

main( ) 
{ 
  int i,t ; 
  long duration[NB_TESTS];
  long duration_val;
  clock_t starttime,endtime ;
  int *array;
  
  for (t=0;t<NB_TESTS;t++)
  {
    starttime=clock();
    array=(int*)malloc(sizeof(int)*SIZE);
    for ( i = 0 ; i < SIZE ; i++ ) 
      array[i]=(i*7)%SIZE;
    sort(array,SIZE);
    free(array);
    endtime=clock();
    duration[t]=1000*((double)(endtime-starttime))/CLOCKS_PER_SEC;
    fprintf(stderr," %dms",duration[t]);
  }
  duration_val=duration[0];
  for (t=1;t<NB_TESTS;t++)
    if (duration_val>duration[t])
      duration_val=duration[t];
  printf("%d",duration_val);
}

static void quicksort ( int array[], int low, int high ) 
{ 
  int pos  ; 
  if ( low < high ) 
  { 
    int item, i, j, t ; 
    item = array[low] ; 
    i = low ; 
    j = high ;
    while ( i < j ) 
    { 
      while ( array[j] > item ) 
        j = j - 1 ;
      while ( array[i] <= item  &&  i < j ) 
        i = i + 1 ;
      if ( i < j ) 
      { 
        t = array[i] ; 
        array[i] = array[j] ; 
        array[j] = t ; 
      } 
    } 
    pos = j ; 
    t = array[low] ; 
    array[low] = array[pos] ; 
    array[pos] = t ; 


    quicksort ( array, pos + 1, high ) ; 
    quicksort ( array, low, pos - 1 ) ; 
  } 
}

void sort (int array[],int size)
{
  quicksort ( array, 0, size-1 ) ;
}

