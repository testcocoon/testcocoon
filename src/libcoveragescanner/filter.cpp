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

#include "filter.h"
#include "wildcard.h"
#include <string.h>
#include <stdlib.h>
#include "regexp_custom.h"
#include "fileinfo.h"
#if defined(OS_WIN32)
#include <windows.h>
#include <process.h>
#endif
#include <stdio.h>

Filter::~Filter()
{
  FUNCTION_TRACE;
  clear();
}

void Filter::clear()
{
  FUNCTION_TRACE;
    for (std::list<filter_t>::iterator it=filter.begin();it!=filter.end();++it)
    {
      FREE(it->expression);
      FREE(it->expression_abs);
      if (it->regexpr!=NULL)
      {
        re_free(it->regexpr);
      }
    }
    filter.clear();
}

std::string Filter::setInternal(filter_t &filter,bool ignore_flag,type_t type,const char *expression) 
{
  FUNCTION_TRACE;
  filter.ignore=ignore_flag;
  filter.type=type;
  filter.expression_abs=NULL;
  filter.regexpr=NULL ;
  switch (type)
  {
    case FUNCTION_WILDCARD:
      filter.expression=STRDUP(expression);
      break;
    case FUNCTION_REGULAR_EXPRESSION:
    case FILE_REGULAR_EXPRESSION:
    case FILE_REGULAR_EXPRESSION_ABS:
      filter.expression=STRDUP(expression);
      filter.expression_abs=STRDUP(expression);
      {
        int ret;
        if ((ret=re_comp(&filter.regexpr,filter.expression)!=0))
        { // error
          char errorstr[CHAINE_LEN];
          re_error(ret,filter.regexpr,errorstr,CHAINE_LEN);
          return std::string(errorstr);
        }
      }
      break;
    case FILE_WILDCARD:
    case FILE_WILDCARD_ABS:
    case PATH:
    case FILENAME:
      {
        filter.expression=(char*)MALLOC(strlen(expression)+2);
        cannonicalFileName(expression,filter.expression);
        if (filter.expression[0]=='\0')
          strcpy(filter.expression,".");
        char tmp[MAX_PATH];
        if (realPath(filter.expression,tmp))
        {
#if defined(OS_WIN32)
          char *it=tmp;
          while (*it)
          {
            *it=tolower(*it);
            it++;
          }
#endif
          filter.expression_abs=STRDUP(tmp);
        }
        else 
          filter.expression_abs=NULL;
      }
      break;
  }
  return std::string();
}

std::string Filter::appendDefault(bool ignore_flag,type_t type,const char *expression)
{
  FUNCTION_TRACE;
  filter_t f;
  std::string err = setInternal(f,ignore_flag,type,expression);
  filter.push_back(f);
  return err;
}

std::string Filter::append(bool ignore_flag,type_t type,const char *expression)
{
  FUNCTION_TRACE;
  filter_t f;
  std::string err = setInternal(f,ignore_flag,type,expression);
  filter.push_front(f);
  return err;
}


bool Filter::isInclude(const char *file_in,std::string &expression_match) const 
{
  FUNCTION_TRACE;
  expression_match.clear();
  int file_lg=strlen(file_in);
  char *path=(char*)MALLOC(file_lg+1); path[0]='\0';
  char *name=(char*)MALLOC(file_lg+1); name[0]='\0';
  char *file=(char*)MALLOC(file_lg+1); file[0]='\0';
  char fileabs[MAX_PATH];
  char pathabs[MAX_PATH];
  cannonicalFileName(file_in,file);
  extractPath(file,path,name);
  realPath(file,fileabs);
  realPath(path,pathabs);

  for (std::list<filter_t>::const_iterator it=filter.begin();it!=filter.end();++it)
  {
    const filter_t f=*it;
    switch (f.type)
    {
      case FILENAME:
        if (f.expression_abs && strcmp(f.expression_abs,fileabs)==0)
          expression_match=std::string(f.expression_abs);
        if (strcmp(f.expression,file)==0)
          expression_match=std::string(f.expression);
        break;
      case PATH:
        if (strcmp(f.expression,path)==0)
          expression_match=std::string(f.expression);
        if (f.expression_abs && strcmp(f.expression_abs,pathabs)==0)
          expression_match=std::string(f.expression_abs);
        break;
      case FILE_WILDCARD_ABS:
        if (wildmatch(f.expression_abs,fileabs))
          expression_match=std::string(f.expression_abs);
        break;
      case FILE_WILDCARD:
        if (wildmatch(f.expression,file))
          expression_match=std::string(f.expression);
        break;
      case FILE_REGULAR_EXPRESSION_ABS: 
        if (re_exec (f.regexpr,fileabs, 0,NULL)!=0)
          expression_match=std::string(f.expression);
        break;
      case FILE_REGULAR_EXPRESSION: 
        if (re_exec (f.regexpr,file, 0,NULL)!=0)
          expression_match=std::string(f.expression);
        break;
      case FUNCTION_WILDCARD:
        if (wildmatch(f.expression,file_in))
          expression_match=std::string(f.expression);
        break;
      case FUNCTION_REGULAR_EXPRESSION:
        if (re_exec (f.regexpr,file_in, 0,NULL)!=0)
          expression_match=std::string(f.expression);
        break;
    }
    if (!expression_match.empty())
    {
#if LOG
      char tmp[CHAINE_LEN];
      if (f.ignore)
       strcpy(tmp,"[EXCLUDE]");
      else
       strcpy(tmp,"[INCLUDE]");
      switch(f.type)
      {
        case PATH                        : strcat(tmp,"[PATH]"); break;
        case FILENAME                    : strcat(tmp,"[FILENAME]"); break;
        case FILE_REGULAR_EXPRESSION     : strcat(tmp,"[REGULAR_EXPRESSION]"); break;
        case FILE_REGULAR_EXPRESSION_ABS : strcat(tmp,"[REGULAR_EXPRESSION_ABS]"); break;
        case FILE_WILDCARD               : strcat(tmp,"[WILDCARD]"); break;
        case FILE_WILDCARD_ABS           : strcat(tmp,"[WILDCARD_ABS]"); break;
        case FUNCTION_REGULAR_EXPRESSION : strcat(tmp,"[REGULAR_EXPRESSION]"); break;
        case FUNCTION_WILDCARD           : strcat(tmp,"[WILDCARD]"); break;
        default: break;
      }
      if (f.expression_abs)
      {
        DEBUG5("Filter:%s[%s->%s] : %s\n",tmp,f.expression,f.expression_abs,file);
      }
      else
      {
        DEBUG4("Filter:%s[%s] : %s\n",tmp,f.expression,file);
      }
#endif
      FREE (file);
      FREE (path);
      FREE (name);
      return !f.ignore;
    }
  }
  FREE (file);
  FREE (path);
  FREE (name);
  expression_match="<default_behaviour>";
  return default_behaviour; // per default exclude
}


#if LOG
void Filter::PDEBUG()  const
{
  FUNCTION_TRACE;
  for (std::list<filter_t>::const_iterator f=filter.begin();f!=filter.end();++f)
  {
    char tmp[CHAINE_LEN];
    if (f->ignore)
      strcpy(tmp,"[EXCLUDE]");
    else
      strcpy(tmp,"[INCLUDE]");
    switch(f->type)
    {
      case PATH                        : strcat(tmp,"[PATH]"); break;
      case FILENAME                    : strcat(tmp,"[FILENAME]"); break;
      case FILE_REGULAR_EXPRESSION     : strcat(tmp,"[FILE_REGULAR_EXPRESSION]"); break;
      case FILE_REGULAR_EXPRESSION_ABS : strcat(tmp,"[FILE_REGULAR_EXPRESSION_ABS]"); break;
      case FILE_WILDCARD               : strcat(tmp,"[FILE_WILDCARD]"); break;
      case FILE_WILDCARD_ABS           : strcat(tmp,"[FILE_WILDCARD_ABS]"); break;
      case FUNCTION_REGULAR_EXPRESSION : strcat(tmp,"[FUNCTION_REGULAR_EXPRESSION]"); break;
      case FUNCTION_WILDCARD           : strcat(tmp,"[FUNCTION_WILDCARD]"); break;
    }
    if (f->expression_abs)
    {
      DEBUG4("Filter:%s[%s->%s]\n",tmp,f->expression,f->expression_abs);
    }
    else
    {
      DEBUG3("Filter:%s[%s]\n",tmp,f->expression);
    }
  }
}
#endif




