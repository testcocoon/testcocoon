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
#include "compilerwrapperenumoption.h"
#include "debug.h"

CompilerWrapperEnumOption::CompilerWrapperEnumOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _value=OPT_UNDEFINED;
}

CompilerWrapperEnumOption::~CompilerWrapperEnumOption()
{
}

void CompilerWrapperEnumOption::clear()
{
  _value=OPT_UNDEFINED;
}

bool CompilerWrapperEnumOption::_parse(const char *field,const char *value)
{
  if (strcmp(field,name())==0)
  {
    if (strcasecmp(value,"YES")==0)
      setValue(OPT_YES);
    else if (strcasecmp(value,"NO")==0)
      setValue(OPT_NO);
    else if (strcasecmp(value,"ALLOW")==0)
      setValue(OPT_ALLOW);
    else if (strcasecmp(value,"POSIX")==0)
      setValue(OPT_POSIX);
    else if (strcasecmp(value,"C++")==0)
      setValue(OPT_CPP);
    else if (strcasecmp(value,"MSCE")==0)
      setValue(OPT_MSCE);
    else if (strcasecmp(value,"NONE")==0)
      setValue(OPT_NONE);
    else if (strcasecmp(value,"GNU")==0)
      setValue(OPT_GNU);
    else if (strcasecmp(value,"MS")==0)
      setValue(OPT_MS);
    else
      return false;
    FREE(_description);
    _description=(char*)MALLOC(strlen(value)+CHAINE_LEN);
    sprintf(_description,"%s (%i)",value,_value);
    return true;
  }
  return false;
}

std::list<std::string> CompilerWrapperEnumOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;
  std::string s;
  switch (value())
  {
    case OPT_YES:
      s="YES";
      break;
    case OPT_NO:
      s="NO";
      break;
    case OPT_ALLOW:
      s="ALLOW";
      break;
    case OPT_POSIX:
      s="POSIX";
      break;
    case OPT_CPP:
      s="C++";
      break;
    case OPT_MSCE:
      s="MSCE";
      break;
    case OPT_NONE:
      s="NONE";
      break;
    case OPT_GNU:
      s="GNU";
      break;
    case OPT_MS:
      s="MS";
      break;
  }

  ret.push_back(s);
  return ret;
}
