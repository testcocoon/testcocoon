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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compilerwrapperintoption.h"
#include "debug.h"

CompilerWrapperIntOption::CompilerWrapperIntOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _value=0;
  _defined=false;
}

CompilerWrapperIntOption::~CompilerWrapperIntOption()
{
}

void CompilerWrapperIntOption::clear()
{
  _value=0;
  _defined=false;
}

bool CompilerWrapperIntOption::_parse(const char *field,const char *value)
{
  if (strcmp(field,name())==0)
  {
    _value=atoi(value);
    _description=STRDUP(value);
    _defined=true;
    return true;
  }
  return false;
}

std::list<std::string> CompilerWrapperIntOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;
  char tmp[50];
  snprintf(tmp,sizeof(tmp),"%i",value());
  std::string s=std::string(tmp);
  ret.push_back(s);
  return ret;
}
