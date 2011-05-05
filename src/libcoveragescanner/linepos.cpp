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

#include "linepos.h"
#include "coveragescannerpdef.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "regexp_custom.h"

LinePos::LinePos()
{
  FUNCTION_TRACE;
}

LinePos::~LinePos()
{
  FUNCTION_TRACE;
  clear();
}

void LinePos::clear()
{
  FUNCTION_TRACE;
  filenames.clear();
  lineinfos.clear();
}

const char*  LinePos::append(const char *name,long line_orig,long abs_line) 
{
  FUNCTION_TRACE;
  if (name)
  {
    const char * found ;
    long line_pre=0;
    for (std::vector<lineinfo_t>::reverse_iterator l=lineinfos.rbegin();l!=lineinfos.rend();l++)
    {
      if ( strcmp(name,l->file) == 0)
      {
        long absolute_line_end=l->absolute_line_end;
        if (absolute_line_end==-1L)
          absolute_line_end=abs_line;
        long offset=abs(absolute_line_end - l->absolute_line_start);
        line_pre=l->line_pre + offset-1;
        break;
      }
    }

    found = append_internal(name,line_orig,abs_line,line_pre); 
    DEBUG5("LinePos::append:%s org=%i abs=%i pre=%i\n",name,line_orig,abs_line,line_pre);
    return found;
  }
  else
    return NULL;
}

/*! \brief find the relative line in the file
 *         from the absolute line in the preprocessed file 
 */
const char*  LinePos::find_internal(const char *name,long abs_line,long &line_pre,long &line_orig)
{
  FUNCTION_TRACE;
  const char *f=NULL;
  find(abs_line,line_orig,f,line_pre);
  if (f==NULL)
    return NULL;
  if (strcmp(f,name)==0)
    return f;
  else
    return NULL;
}

/*! \brief find the relative line in the file
 *         from the absolute line in the preprocessed file 
 */
void  LinePos::find(long abs_line,long &line_orig,const char *&f,long &line_pre) const
{
  FUNCTION_TRACE;
  f=NULL;
  line_pre=-1;
  line_orig=-1;
  long base_absolute_line=-1;
  find_prot(abs_line,base_absolute_line,f,line_pre,line_orig);
}

void  LinePos::find_prot(long abs_line,long &base_abs_line,const char *&f,long &line_pre,long &line_orig) const
{
  FUNCTION_TRACE;
  if (abs_line==-1)
    return ;
  for (std::vector<lineinfo_t>::const_reverse_iterator l=lineinfos.rbegin();l!=lineinfos.rend();l++)
  {
    if ( (abs_line>=l->absolute_line_start && abs_line<l->absolute_line_end)
        || (abs_line>=l->absolute_line_start && l->absolute_line_end==-1) )
    {
      long offset=abs(abs_line-l->absolute_line_start);
      long pos_rel=l->line_pre + offset;
      line_pre = pos_rel;
      long pos_orig=l->line_orig + offset;
      line_orig = pos_orig;
      f=l->file;
      base_abs_line=l->absolute_line_start;
      return ;
    }
  }
}

#if LOG
void LinePos::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG1("LinePos correlations:\n");
  for (std::vector<lineinfo_t>::const_iterator l=lineinfos.begin();l!=lineinfos.end();++l)
  {
    DEBUG6("LinePos: abs=%i->%i file=%s org=%i pre=%i\n",l->absolute_line_start,l->absolute_line_end,l->file,l->line_orig,l->line_pre);
  }
}
#endif

const char*  LinePos::append_internal(const char *name,long line_orig,long abs_line,long line_pre) 
{
  FUNCTION_TRACE;
  std::vector<lineinfo_t>::reverse_iterator l=lineinfos.rbegin();
  if (l!=lineinfos.rend())
  {
    if (l->absolute_line_end==-1)
      l->absolute_line_end=abs_line;
  }

  int nb_lineinfos=lineinfos.size()+1;
  if ( (lineinfos.size()&0xFF)== 0)
     lineinfos.reserve(256+nb_lineinfos);
  lineinfos.resize(nb_lineinfos);
  int idx=nb_lineinfos-1;
  lineinfos[idx].file=filenames.append(name);
  lineinfos[idx].absolute_line_start=abs_line;
  lineinfos[idx].line_pre=line_pre;
  lineinfos[idx].line_orig=line_orig;
  lineinfos[idx].absolute_line_end=-1;

  return lineinfos[idx].file;
}

/*! \brief find the location in the original file and the
 *         measurement file (preprocessed)
 *
 *         This is used to create the line translation list
 */
void LinePos::at(long id,long &line_orig,long &line_abs, long &line_pre,const char * &filename) const
{
  FUNCTION_TRACE;
  if (id<count())
  {
    line_abs=lineinfos[id].absolute_line_start;
    line_pre=lineinfos[id].line_pre;
    line_orig=lineinfos[id].line_orig;
    filename=lineinfos[id].file;
  }
  else
  {
    line_abs=-1;
    line_pre=-1;
    line_orig=-1;
    filename=NULL;
  }
}

