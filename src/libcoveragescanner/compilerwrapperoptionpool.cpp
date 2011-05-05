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

#include "compilerwrapperoptionpool.h"
#include "expression.h"

CompilerWrapperOptionPool::CompilerWrapperOptionPool()
{
   architecture_arg_append_p = new        CompilerWrapperEnumOption  ( *this,"ARCHITECTURE_APPEND");
   table_architecture_arg_p  = new        CompilerWrapperListOption  ( *this, "ARCHITECTURE");
}

CompilerWrapperOptionPool::~CompilerWrapperOptionPool()
{
  options.clear();
  delete architecture_arg_append_p;
  delete table_architecture_arg_p;
}

void CompilerWrapperOptionPool::add(CompilerWrapperOption *v)
{
  options.push_back(v);
}

void CompilerWrapperOptionPool::remove(CompilerWrapperOption *v)
{
  options.remove(v);
}

bool CompilerWrapperOptionPool::parse(const char *field,const char *value,bool override)
{
  bool res=false;
  for ( std::list<CompilerWrapperOption*>::iterator it=options.begin();it!=options.end();++it)
  {
    if ( (*it)->parse(field,value,override) )
      res=true;
  }
  return res;
}

#if LOG
void CompilerWrapperOptionPool::PDEBUG() const
{
  for ( std::list<CompilerWrapperOption*>::const_iterator it=options.begin();it!=options.end();++it)
  {
    DEBUG3("Options: %s=%s\n",(*it)->name(),(*it)->description());
  }
}
#endif

void CompilerWrapperOptionPool::clear() 
{
  for ( std::list<CompilerWrapperOption*>::iterator it=options.begin();it!=options.end();++it)
    (*it)->clear();
}

bool CompilerWrapperOptionPool::parseprofile(const std::string & profile_name,const char *architecture_option,const Argv &param_args)
{
  FUNCTION_TRACE;
  bool success=true;
  char *current_architecture=NULL;
  FILE *f;
  int line=0;
  char tmp[CHAINE_LEN];
  DEBUG2("Profile path: %s\n",profile_name.c_str());
  f=fopen(profile_name.c_str(),"r");
  if (f==NULL)
    return false;
  while (fgets(tmp,CHAINE_LEN,f)!=NULL)
  {
    line++;
    int lg=strlen(tmp);
    if (lg<1)
    {
      ERROR4("Malformated profile line(%s:%i):%s\n",profile_name.c_str(),line,tmp);
      success=false;
      break;
    }
    if (tmp[lg-1]!='\n')
    {
      ERROR4("Malformated profile line(%s:%i):%s\n",profile_name.c_str(),line,tmp);
      success=false;
      break;
    }
    tmp[lg-1]='\0';
    if (tmp[0]=='#')
      continue ; // comment
    bool empty=true;
    for (int i=0;empty && tmp[i];i++)
    {
      if (tmp[i]!=' ' && tmp[i]!='\n' && tmp[i]!='\r' && tmp[i]!='\t')
        empty=false;
    }
    if (empty)
      continue; // empty lines

    char *value=strchr(tmp,'=');
    if (value==NULL)
    {
      ERROR4("Malformated profile line(%s:%i):%s\n",profile_name.c_str(),line,tmp);
      success=false;
      break;
    }
    *value='\0';
    value++;

    char *architecture=strchr(tmp,'[');
    if (architecture && (architecture<value) )
    {
      char *end_architecture=strchr(tmp,']');
      if (end_architecture<value)
      {
        *architecture='\0';
        architecture++;
        *end_architecture='\0';
      }
      else
      {
        ERROR4("Malformated profile line(%s:%i):%s\n",profile_name.c_str(),line,tmp);
        success=false;
        break;
      }
    }
    char *item=tmp;

    /* Check if the architecture match */
    bool override=false;
    if (architecture)
    {
      if (!current_architecture)
      {
        if (architecture_option)
          current_architecture=STRDUP(architecture_option);
        else
        {
          for (int iparam=1;!current_architecture && (iparam<param_args.argc());iparam++)
          {
            const char * architecture_arg_str=table_architecture_arg_p->matchBooleanOption(param_args[iparam],param_args[iparam+1],*architecture_arg_append_p);
            if (architecture_arg_str)
              current_architecture=STRDUP(architecture_arg_str);
          }
        }
#if LOG
        if (current_architecture)
        {
          DEBUG2("current_architecture=%s\n",current_architecture);
        }
#endif
      }
      if (!current_architecture)
        continue; // Skip argument: this parameter is not for this architecture
      if (strcmp(architecture,current_architecture)!=0)
        continue; // Skip argument: this parameter is not for this architecture
      override=true;
    }

    if (parse(item,value,override))
      ;
    else
    {
      ERROR5("Malformated profile line(%s:%i):%s=%s\n",profile_name.c_str(),line,item,value);
      success=false;
      break;
    }
  };
  FREE(current_architecture);
  current_architecture=NULL;
  fclose(f);

#if LOG
  PDEBUG();
#endif
  return success;
}

