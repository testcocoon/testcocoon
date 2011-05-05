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
#include "compilerwrappertextoption.h"
#include "debug.h"

CompilerWrapperTextOption::CompilerWrapperTextOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _value=NULL;
}

CompilerWrapperTextOption::~CompilerWrapperTextOption()
{
  FREE(_value);
}

bool CompilerWrapperTextOption::_parse(const char *field,const char *v)
{
  if (strcmp(field,name())==0)
  {
    setValue(v);
    FREE(_description);
    _description=STRDUP(v);
    return true;
  }
  return false;
}

void CompilerWrapperTextOption::clear()
{
  FREE(_value);
  _value=NULL;
}

std::list<std::string> CompilerWrapperTextOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;
  ret.push_back(value());
  return ret;
}
