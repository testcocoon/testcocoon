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

#include "filename.h"
#include "coveragescannerpdef.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "regexp_custom.h"

FileName::FileName()
{
  FUNCTION_TRACE;
}

FileName::~FileName()
{
  FUNCTION_TRACE;
  clear();
}

void FileName::clear()
{
  FUNCTION_TRACE;
  for (std::vector<char*>::iterator itFile=files_tab.begin();itFile!=files_tab.end();++itFile)
    FREE(*itFile);
  files_tab.clear();
}

bool  FileName::find(const char *n) const
{
  FUNCTION_TRACE;
  for (std::vector<char*>::const_iterator itFile=files_tab.begin();itFile!=files_tab.end();++itFile)
  {
    if (strcmp(n,*itFile)==0)
      return true;
  }
  return false;
}

const char * FileName::append(const char *n)
{
  FUNCTION_TRACE;
  for (std::vector<char*>::iterator itFile=files_tab.begin();itFile!=files_tab.end();++itFile)
  {
    const char *f=*itFile;
    if (strcmp(n,f)==0)
      return f; /* already exists */
  }

  /* append */
  int nb_files_tab=files_tab.size()+1;
  if ( (files_tab.size()&0xFF)== 0)
     files_tab.reserve(256+nb_files_tab);
  files_tab.resize(nb_files_tab);
  files_tab[nb_files_tab-1]=STRDUP(n);
  return files_tab[nb_files_tab-1];
}

#if LOG
void FileName::PDEBUG() const
{
  FUNCTION_TRACE;
  for (std::vector<char*>::const_iterator itFile=files_tab.begin();itFile!=files_tab.end();++itFile)
  {
    DEBUG2("FileName:%s\n",*itFile);
  }
}
#endif

unsigned long FileName::signature(long i) const
{
  FUNCTION_TRACE;
  const char *tmp = at(i);
  if (tmp==NULL)
    FATAL1("Invalid file index\n");

  unsigned long s=calc_signature(tmp);
  return s;
}

void FileName::merge(const FileName &v)
{
  FUNCTION_TRACE;
  int count=v.count();
  for (int i=0;i<count;i++)
    append(v.at(i));
}

unsigned long FileName::calc_signature(const char *file) const
{
  FUNCTION_TRACE;
  unsigned long s=0;
  FILE *f;
  if ((f=fopen(file,"r")))
  {
    while (!feof(f))
    {
      char c = fgetc(f);
      s =  ( (s << 4) | (s >> 28) ) ^ c;
    }
    s=(s&0xFFFFFF);
    fclose(f);
    return s;
  }
  else
    return 0;
}
