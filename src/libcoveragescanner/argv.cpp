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

#include "argv.h"
#include "debug.h"
#include <string.h>

Argv::Argv() 
{
  _argc=0;
  _argv=NULL;
}

Argv::Argv(const Argv &v)
{
  _argc=0;
  _argv=NULL;
  if (v.argc())
    duplicate(_argc,_argv,v.argc(),v.argv());
}

Argv::Argv(int c,const char * const * v) 
{
  _argc=0;
  _argv=NULL;
  if (v)
    duplicate(_argc,_argv,c,v);
}

Argv::~Argv() 
{
  Argv::free(_argv);
}


void Argv::append(const char *val)
{
  append(_argc,_argv,val);
}

void Argv::suppress_args(int min_index,int max_index)
{
  suppress_args(_argc,_argv,min_index,max_index);
}

void Argv::append(int argc_to_append,const char *const* argv_to_append)
{
  append(_argc,_argv,argc_to_append,argv_to_append);
}

Argv& Argv::operator=(const Argv&v)
{
  duplicate(_argc,_argv,v.argc(),v.argv());
  return *this;
}

void Argv::append(int &argc,char **&argv,const char *val)
{
  argv=(char**)REALLOC(argv,sizeof(*argv)*(argc+2));
  argv[argc]=STRDUP(val);
  argc++;
  argv[argc]=NULL;
}

void Argv::append(int &argc,char **&argv,int argc_to_append,const char *const* argv_to_append)
{
  argv=(char**)REALLOC(argv,sizeof(*argv)*(argc+argc_to_append+1));
  int i;
  for (i=0;argv_to_append[i];i++)
    argv[argc+i]=STRDUP(argv_to_append[i]);
  argv[argc+i]=NULL;
  argc+=argc_to_append;
}

void Argv::suppress_args(int &argc,char **&argv,int min_index,int max_index)
{
  if (min_index>max_index)
    return;
  if (min_index>=argc)
    return;
  if (max_index>=argc)
    max_index=argc-1;
  if (min_index>max_index)
    min_index=max_index;

  int sz=max_index-min_index+1;
  int i;
  for (i=min_index;i<argc-sz;i++)
    argv[i]=argv[i+sz];
  argv[i]=NULL;
  argc-=sz;
}

void Argv::free(char **&argv)
{
  if (argv)
  {
    for (int i=0;argv[i];i++)
    {
      FREE(argv[i]);
      argv[i]=NULL;
    }

    FREE(argv);
    argv=NULL;
  }
}

void Argv::duplicate(int &argc_dest,char **&argv_dest,int argc_src,const char * const * argv_src)
{
  Argv::free(argv_dest);
  argc_dest=argc_src;
  argv_dest=(char**)MALLOC(sizeof(char*)*(argc_src+1));
  int i;
  for (i=0;i<argc_src;i++)
    argv_dest[i] = STRDUP(argv_src[i]);
  argv_dest[i] = NULL;
}

void Argv::clear()
{
  Argv::free(_argv);
  _argc=0;
}


bool Argv::set(int index,const char*value)
{
  if (index>=argc())
    return false;
  FREE(_argv[index]);
  _argv[index]=STRDUP(value);
  return true;
}

