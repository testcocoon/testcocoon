/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.TXT included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#include "coveragescannerpdef.h"
#include "parsemisc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long inIntervalRange(long l, long c,long start_line,long start_column, long end_line,long end_column)
{
  FUNCTION_TRACE;
  if ((l>=start_line) && (l<=end_line))
  {
    if ((l==start_line) && (c>=start_column))
    {
      if (l==end_line)
      {
        if (c<end_column)
          return true;
      }
      else
        return true;
    }
    else if ((l!=start_line) &&  (l==end_line) && (c<end_column) )
      return true;
    else if ( (l>start_line) && (l<end_line) )
      return true;
  }
  return false;
}

/*! \brief generate an index of lines
 *
 * \note the index must be freed 
 * by the caller
 */
void generate_line_index(const char *s,long s_lg,long **line_index,long *nb_line_index)
{
  FUNCTION_TRACE;
  *nb_line_index=0;
  *line_index=NULL;

  bool new_line=true;
  for (int i=0;i<s_lg;i++)
  {
    if (s[i]=='\0')
      continue;
    if (new_line)
    {
      if ( ((*nb_line_index)&0xFF) == 0 )
        *line_index=(long *)REALLOC(*line_index,sizeof(long)*( (*nb_line_index) + 257 ));
      (*line_index)[*nb_line_index]=i;
      (*nb_line_index)++;
      new_line=false;
    }
    if (s[i]=='\n')
      new_line=true;
  }
}

/*! \brief extract string from the source
 *
 * \note the string is allocated, the deallocation must be performed
 * by the caller
 */
void extract_string(char *s,long s_lg,const long *line_index, long nb_line_index,long start_line,long start_column, long end_line, long end_column,char **str_p)
{
  FUNCTION_TRACE;
    long size=0;
    long pos=-1;
    long i,nb;

    long start_index=0;
    long end_index=s_lg;
    int l=1;
    int c=1;
    if (start_line-1<nb_line_index)
    {
      l=start_line;
      start_index=line_index[start_line-1];
    }
    if (end_line<nb_line_index)
      end_index=line_index[end_line];

    for (i=start_index;i<end_index;i++)
    {
        if (s[i]=='\0')
            continue;
        if (inIntervalRange(l,c,start_line,start_column,end_line,end_column))
        {
          if (pos<0) pos=i;
          size++;
        }
        if (s[i]=='\n')
        {
            l++;
            c=1;
        }
        else
          c++;
    }
#ifndef NO_DEBUG
    long _size=0;
    long _pos=-1;
    long _i;
    int _l=1;
    int _c=1;

    for (_i=0;_i<s_lg;_i++)
    {
        if (s[_i]=='\0')
            continue;
        if (inIntervalRange(_l,_c,start_line,start_column,end_line,end_column))
        {
          if (_pos<0) _pos=_i;
          _size++;
        }
        if (s[_i]=='\n')
        {
            _l++;
            _c=1;
        }
        else
          _c++;
    }

    if(_size!=size)
      FATAL1("Incoherent size");
    if(_pos!=pos)
      FATAL1("Incoherent position");
#endif

    if (pos==-1) 
    {
      if (*str_p!=NULL) FREE (*str_p);
      *str_p=NULL;
      return;
    }
    *str_p = (char*)REALLOC((void*)*str_p,size+1);
    for (i=pos,nb=0;nb<size;i++)
    {
        if (s[i]!='\0')
            (*str_p)[nb++]=s[i];
    }
    (*str_p)[size]='\0';

}

