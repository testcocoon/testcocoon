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
#include "compilerwrapperoption.h"
#include "debug.h"
#include "compilerwrapperoptionpool.h"

CompilerWrapperOption::CompilerWrapperOption(CompilerWrapperOptionPool &pool,const char *field_name) : _pool(pool)
{
  _description=NULL;
  _field_name=STRDUP(field_name);
  _pool.add(this);
}

CompilerWrapperOption::~CompilerWrapperOption()
{
  _pool.remove(this);
  FREE(_field_name);
}


bool CompilerWrapperOption::parse(const char *field,const char *value, bool override)
{
  if (defined() && !override)
  {
    return (strcmp(field,name())==0);
  }
  else
    return _parse(field,value);
}
