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
#include "compilerwrapperlanguageoption.h"
#include "debug.h"

CompilerWrapperLanguageOption::CompilerWrapperLanguageOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _value=Expression::CODESTYLE_UNKNOWN;
}

CompilerWrapperLanguageOption::~CompilerWrapperLanguageOption()
{
}

void CompilerWrapperLanguageOption::clear()
{
  _value=Expression::CODESTYLE_UNKNOWN;
}


bool CompilerWrapperLanguageOption::_parse(const char *field,const char *value)
{
  if (strcmp(field,name())==0)
  {
    if (strcasecmp(value,"C#")==0)
      setValue(Expression::CODESTYLE_CS);
    else if (strcasecmp(value,"C++")==0)
      setValue(Expression::CODESTYLE_CPP);
    else
      return false;
    FREE(_description);
    _description=(char*)MALLOC(strlen(value)+CHAINE_LEN);
    sprintf(_description,"%s (%i)",value,_value);
    return true;
  }
  return false;
}

std::list<std::string> CompilerWrapperLanguageOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;
  std::string s;
  switch (value())
  {
    case Expression::CODESTYLE_CPP:
      s="C++";
      break;
    case Expression::CODESTYLE_CS:
      s="C#";
      break;
  }

  ret.push_back(s);
  return ret;
}
