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
#include "compilerwrapperlistoption.h"
#include "compilerwrapperenumoption.h"
#include "debug.h"

CompilerWrapperListOption::CompilerWrapperListOption(CompilerWrapperOptionPool&pool,const char *field_name) : CompilerWrapperOption(pool,field_name)
{
  _values=NULL;
}

CompilerWrapperListOption::~CompilerWrapperListOption()
{
  clear();
}

void CompilerWrapperListOption::clear()
{
  if (_values)
    delete_table(_values);
  _values=NULL;
}

bool CompilerWrapperListOption::_parse(const char *field,const char *v)
{
  if (strcmp(field,name())==0)
  {
    clear();
    build_table(_values,v);
    FREE(_description);
    _description=STRDUP(v);
    return true;
  }
  return false;
}

void CompilerWrapperListOption::delete_table(char **table)
{
  FUNCTION_TRACE;
  char **item=table;
  while (*item!=NULL)
  {
    FREE(*item);
    *item=NULL;
    item++;
  }
  FREE(table);
}

bool CompilerWrapperListOption::build_table (char **&table,const char *val)
{
  FUNCTION_TRACE;
  if (val[0]=='\0')
  {
    table=(char**)MALLOC(sizeof(char*));
    (table)[0]=NULL;
    return true;
  }

  /* Count the number of items */
  int count=1;
  const char *pos=val;
  while (*pos==';')
    pos++;
  while ( (pos=strchr(pos,';')) !=NULL )
  {
    count++;
    while (*pos==';')
      pos++;
  }
  
  table=(char**)MALLOC(sizeof(char*)*(count+1));

  /* Fill the datas */
  table[count]=NULL;
  const char *pos1=val;
  const char *pos2;
  int i=0;
  while (*pos1==';')
    pos1++;
  while ( (pos2=strchr(pos1,';')) !=NULL )
  {
    table[i]=(char*)MALLOC(pos2-pos1+1);
    strncpy(table[i],pos1,pos2-pos1);
    table[i][pos2-pos1]='\0';
    while (*pos2==';')
      pos2++;
    pos1=pos2;
    i++;
  }
  table[i]=STRDUP(pos1);
  return true;
}

const char * CompilerWrapperListOption::matchBooleanOption(const char *arg0,const char *arg1,const CompilerWrapperEnumOption &behaviour) const
{
  return matchBooleanOption_table(arg0,arg1,_values,behaviour);
}

const char * CompilerWrapperListOption::matchBooleanOption_item(const char *arg0,const char *arg1,const char *option,const CompilerWrapperEnumOption &behaviour)
{
  FUNCTION_TRACE;
  int lg=strlen(option);
  if (behaviour.value()==CompilerWrapperEnumOption::OPT_YES && strncmp(arg0,option,lg)==0)
    return & (arg0[lg]);
  else if (behaviour.value()==CompilerWrapperEnumOption::OPT_NO && strcmp(arg0,option)==0)
    return arg1;
  else if (behaviour.value()==CompilerWrapperEnumOption::OPT_ALLOW && strncmp(arg0,option,lg)==0)
  {
    if (arg0[lg]=='\0')
      return arg1;
    else
      return & (arg0[lg]);
  }

  return NULL;
}

const char * CompilerWrapperListOption::matchBooleanOption_table(const char *arg0,const char *arg1,char ** const option_table,const CompilerWrapperEnumOption & behaviour)
{
  FUNCTION_TRACE;
  const char * option= findOptionTable (option_table,arg0,behaviour.value()!=CompilerWrapperEnumOption::OPT_NO)  ;
  if (option)
    return matchBooleanOption_item(arg0,arg1,option,behaviour);
  else
    return NULL;
}

const char *  CompilerWrapperListOption::findOptionTable (const char *param,bool append)  const
{
  FUNCTION_TRACE;
  return findOptionTable(_values,param,append);
}

const char *  CompilerWrapperListOption::findOptionTable (char **const table,const char *param,bool append)  
{
  FUNCTION_TRACE;
  char *opt=NULL;
  for (int j=0;(opt=table[j]);j++)
  {
    if (append)
    {
      if (strncmp(param,opt,strlen(opt))==0)
        return opt;
    }
    else
    {
      if (strcmp(param,opt)==0)
        return opt;
    }
  }
  return NULL;
}


std::list<std::string> CompilerWrapperListOption::profileValues() const
{
  std::list<std::string> ret;
  if (!defined())
    return ret;

  for (int i=0;_values[i];i++)
    ret.push_back(_values[i]);
  return ret;
}
