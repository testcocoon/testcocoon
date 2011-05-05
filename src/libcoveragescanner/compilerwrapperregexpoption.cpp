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
#include <stdio.h>
#include "compilerwrapperregexpoption.h"
#include "regexp_custom.h"
#include "debug.h"

CompilerWrapperRegExpOption::CompilerWrapperRegExpOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _value=NULL;
  _regexp=NULL;
}

CompilerWrapperRegExpOption::~CompilerWrapperRegExpOption()
{
  FREE(_value);
  if (_regexp)
    re_free(_regexp);
}

void CompilerWrapperRegExpOption::clear()
{
  FREE(_value);
  if (_regexp)
    re_free(_regexp);
  _value=NULL;
  _regexp=NULL;
}

bool CompilerWrapperRegExpOption::_parse(const char *field,const char *v)
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

void CompilerWrapperRegExpOption::setValue(const char *v) 
{
  FREE(_value);
  _value=STRDUP(v); 
  if (_regexp)
    re_free(_regexp);
  _regexp=NULL;
  int ret;
  if ((ret=re_comp(&_regexp,_value)!=0))
  { // error
    char errorstr[CHAINE_LEN];
    re_error(ret,_regexp,errorstr,CHAINE_LEN);
    FATAL3("Error compiling regular expression '%s':%s\n",v,errorstr);
  }
}

bool CompilerWrapperRegExpOption::match(const char *v)const
{
  if (_regexp)
  {
    if (re_exec (_regexp,v, 0,NULL)!=0)
      return true;
  }
  return false;
}

std::list<std::string> CompilerWrapperRegExpOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;
  ret.push_back(_value);
  return ret;
}
