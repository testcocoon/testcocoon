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

#include "libcsmespdef.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#if defined(OS_LINUX) || defined(OS_MACX)
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#elif defined(OS_WIN32)
#include "ms_dirent.h"
#include <windows.h>
#endif
enum argv_state_t
{
  NORMAL,
  ESCAPE_CHAR,
  IN_STRING,
  SEPARATOR,
  IN_STRING_QUOTE
};

static void insert_argv(int &arg_lg,char *arg,int &argc,char **argv[])
{
  FUNCTION_TRACE;
  if (arg[0]!='\0')
  {
    argc++;
    (*argv)=(char **)REALLOC(*argv,sizeof(char*)*(argc+1));
    (*argv)[argc]=NULL;
    (*argv)[argc-1]=STRDUP(arg);
    arg_lg=0;
    arg[0]='\0';
  }
}

void System::command2argv(const char *command,char **argv_p[])
{
  FUNCTION_TRACE;
  int argc=0;
  char **argv=(char **)MALLOC(sizeof(char*)*(argc+1));
  argv_state_t state=NORMAL;
  argv_state_t laststate=NORMAL;
  char *arg=(char*)MALLOC((strlen(command)+1)*sizeof(char));
  arg[0]='\0';
  int arg_lg=0;

  for (const char *c=command;*c!='\0';c++)
  {
    switch(*c)
    {
      case '\\':
        switch (state)
        {
          case SEPARATOR:
            insert_argv(arg_lg,arg,argc,&argv);
            state=ESCAPE_CHAR;
            laststate=NORMAL;
            break;
          case IN_STRING:
          case NORMAL:
            laststate=state;
            state=ESCAPE_CHAR;
            break;
          case ESCAPE_CHAR:
            state=laststate;
            arg[arg_lg++]=*c;
            break;
          case IN_STRING_QUOTE:
            break;
        }
        break;
      case '"':
        switch (state)
        {
          case SEPARATOR:
            insert_argv(arg_lg,arg,argc,&argv);
            state=IN_STRING;
            break;
          case NORMAL:
            laststate=state;
            state=IN_STRING;
            break;
          case ESCAPE_CHAR:
            state=laststate;
            arg[arg_lg++]=*c;
            break;
          case IN_STRING:
            state=NORMAL;
            break;
          case IN_STRING_QUOTE:
            arg[arg_lg++]=*c;
            break;
        }
        break;
      case '\'':
        switch (state)
        {
          case ESCAPE_CHAR:
            arg[arg_lg++]=*c;
            state=laststate;
            break;
          case SEPARATOR:
            insert_argv(arg_lg,arg,argc,&argv);
            state=IN_STRING_QUOTE;
            break;
          case NORMAL:
            laststate=state;
            state=IN_STRING_QUOTE;
            break;
          case IN_STRING:
            arg[arg_lg++]=*c;
            break;
          case IN_STRING_QUOTE:
            state=laststate;
            break;
        }
        break;
      case ' ':
        switch (state)
        {
          case SEPARATOR:
            break;
          case IN_STRING_QUOTE:
          case IN_STRING:
          case ESCAPE_CHAR:
            arg[arg_lg++]=*c;
            break;
          case NORMAL:
            state=SEPARATOR;
            break;
        }
        break;
      default:
        switch (state)
        {
          case SEPARATOR:
            insert_argv(arg_lg,arg,argc,&argv);
            arg[arg_lg++]=*c;
            state=NORMAL;
            break;
          case IN_STRING_QUOTE:
          case IN_STRING:
          case ESCAPE_CHAR:
          case NORMAL:
            arg[arg_lg++]=*c;
            break;
        }
        break;
    }
    arg[arg_lg]='\0';
  }
  insert_argv(arg_lg,arg,argc,&argv);

  *argv_p=argv;
}


int System::exec(const char *command,const char *stdout_filename,const char *input_txt)
{
  FUNCTION_TRACE;
  System sys;

  DEBUG2("begin system call, cmd='%s'\n",command);
  char buffer[4096];
  FILE* output_stream=stdout;

  if (!sys.call(command,(stdout_filename!=NULL) || (input_txt!=NULL)))
  {
    return -1;
  }
  if (input_txt)
    sys.writeStdin(input_txt,strlen(input_txt));
  if (stdout_filename)
  {
    output_stream=fopen(stdout_filename,"wb");
    if (output_stream==NULL)
      FATAL2("Could not open file '%s'\n",stdout_filename);

    for (;;)
    {
      int nb_read=sys.readStdout(buffer,sizeof(buffer));
      if (nb_read!=0)
      {
        int nb_written=0;
        do
        {
          int nb_wr=fwrite(buffer,1,nb_read,output_stream);
          nb_written+=nb_wr;
        } while (nb_written!=nb_read);
      }
      else
        break;
    }
    fclose(output_stream);
  }
  
  int ret = sys.exitValue();
  DEBUG2("end system call, ret=%i\n",ret);
  return ret;
}



std::string System::quoteArgument(const std::string &str)
{
  char *s=quote_argument(str.c_str());
  std::string r=s;
  FREE(s);
  return r;
}

char * System::quote_argument(const char*str)
{
  FUNCTION_TRACE;
  char *out;

  if (str==NULL)
    return NULL;

  if (needQuotes(str))
  {
    out=(char*)MALLOC(strlen(str)*2+3);
    char *c_out=out;
    *c_out='"'; c_out++;
    for (const char *c_str=str;*c_str;c_str++)
    {
      switch(*c_str)
      {
        case '\\':
        case '"':
          *c_out='\\';
          c_out++; 
          break;
      }
      *c_out=*c_str;
      c_out++;
    }
    *c_out='"'; c_out++;
    *c_out='\0';
  }
  else
    out=STRDUP(str);
  return out;
}


bool System::needQuotes(const char *str)
{
  FUNCTION_TRACE;
  const char *c;
  for (c=str;*c!='\0';c++)
  {
#if defined(OS_WIN32)
    if (*c==' ')
      return true;
#else
    if (isblank(*c))
      return true;
#endif
    if (*c=='"')
      return true;
  }
  return false;
}

void System::stripQuotes(std::string &str)
{
  char *s=STRDUP(str.c_str());
  strip_quotes(s);
  str=std::string(s);
  FREE(s);
}

void System::strip_quotes(char *
#if defined(OS_WIN32)
    arg
#endif
    )
{
  FUNCTION_TRACE;
#if defined(OS_WIN32)
  int id=0;
  bool escape=false;
  while (arg[id]!='\0')
  {
    if (arg[id]=='"')
    { /* stripping quotes */
      char *tmp=&arg[id];
      while (*tmp) 
      {
        *tmp=*(tmp+1);
        tmp++;
      }
    }
    else
      id++;
    if (arg[id]=='\\')
    {
      if (escape)
        escape=false;
      else
        escape=true;
    }
    else
      escape=false;
  }
#endif
}

bool System::rename_file ( const char * oldname , const char * newname )
{
  if (rename(oldname,newname)==0)
  {
    DEBUG3("Rename File %s->%s\n",oldname,newname);
    return true;
  }
  else
  {
    DEBUG4("Rename File %s->%s failed:%s\n",oldname,newname,strerror(errno));
    return false;
  }
}

bool System::delete_file(const char * name)
{
  FUNCTION_TRACE;
  if (unlink(name)==0)
  {
    DEBUG2("Delete %s\n",name);
    return true;
  }
  else
  {
    DEBUG3("File %s can not be removed:%s\n",name,strerror(errno));
    return false;
  }
}

std::string System::suppressExecSuffix(const std::string &f)
{
    int pos=f.length()-4;
    if (pos>0 && strcasecmp(&(f.c_str())[pos],".exe")==0)
    {
      return f.substr(0,pos);
    }
    return f;
}
