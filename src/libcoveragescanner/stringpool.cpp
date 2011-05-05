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

#include "stringpool.h"
#include <string.h>

StringPool::StringPool()
{
}

StringPool::~StringPool()
{
  clear();
}

void StringPool::clear()
{
  for (std::list<char*>::iterator it=list_of_string.begin();it!=list_of_string.end();++it)
    FREE(*it);
  list_of_string.clear();
}

const char *StringPool::add(const char *str)
{
  if (!str)
    return NULL;

  for (std::list<char*>::const_iterator it=list_of_string.begin();it!=list_of_string.end();++it)
  {
    if (strcmp(*it,str)==0)
      return *it;
  }
  char *v=STRDUP(str);
  list_of_string.push_back(v);
  return v;
}
