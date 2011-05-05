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
#include "fileinfo.h"
#include <string.h>
#include <stdlib.h>
#if defined(OS_WIN32)
#include <windows.h>
#include <process.h>
#define _fullpath(res,path,size) (GetFullPathNameA ((path), (size), (res), NULL) ? (res) : NULL)
#define realpath(path,resolved_path) _fullpath(resolved_path, path, MAX_PATH)
#endif
#ifndef HAVE_REALPATH
#error "HAVE_REALPATH not defined"
#endif
#if !defined(OS_WIN32) && !defined(OS_LINUX) && !defined(OS_MACX)
#error "NO platform define set"
#endif

#if defined(OS_LINUX) || defined(OS_MACX)
 #include <unistd.h>
#endif

void  extractPath(const char *file,char *path,char *name) 
{
  int file_pos=0;
  for (int i=0;file[i]!='\0';i++)
  {
    if (file[i]==FILE_SEPARATOR_CHAR || file[i]==FILE_SEPARATOR_CHAR_BIS)
      file_pos=i+1;
  }
  strcpy(name,&file[file_pos]);
  strcpy(path,file);
  file_pos--;
  if (file_pos<0) file_pos=0;
  path[file_pos]='\0';
  if (path[0]=='\0')
    strcpy(path,".");
}
 
bool realLink(const char *path,char resolved_path[MAX_PATH])
{
#if HAVE_SYMLINK
  if (readlink(path,resolved_path,MAX_PATH)>=0)
  {
    resolved_path[MAX_PATH-1]='\0';
    return true;
  }
  else
  {
    resolved_path[0]='\0';
    return false;
  }
#else
  return false;
#endif
}

char* realPath(const char *path,char resolved_path[MAX_PATH])
{
  char *ret;

#if defined(HAVE_REALPATH)
  ret = realpath(path,resolved_path);
#else
  char _resolved_path[MAX_PATH];
  if (path[0]==FILE_SEPARATOR_CHAR)
  {
    strncpy(_resolved_path,path,MAX_PATH);
    _resolved_path[MAX_PATH-1]='\0';
  }
  else
  {
    memset(_resolved_path,0,MAX_PATH);
    getcwd(_resolved_path,MAX_PATH);
    _resolved_path[MAX_PATH-1]='\0';
    _resolved_path[strlen(_resolved_path)]=FILE_SEPARATOR_CHAR;
    _resolved_path[MAX_PATH-1]='\0';
    int sz=MAX_PATH-(strlen(_resolved_path)+strlen(path));
    if (sz>0)
      strncat(_resolved_path,path,sz);
    _resolved_path[MAX_PATH-1]='\0';
  }
  cannonicalFileName(_resolved_path,resolved_path);
  ret = resolved_path;
#endif
  if (ret)
  {
    char *it=ret;
    while (*it)
    {
#if defined(OS_WIN32)
       *it=tolower(*it);
#endif
       if ( (*it==FILE_SEPARATOR_CHAR) && ( *(it+1) == '\0' ) )
          *it='\0'; // suppression of the last(s) path separator
       it++;
    }
  }
  //DEBUG3("realPath '%s' -> '%s'\n",path,ret);
  return ret;
}

void cannonicalFileName(const char *input,char *output) 
{
  FUNCTION_TRACE;
  int i;
  bool finished=false;
  int pos=0;

  for (i=0;input[i];i++)
    if (input[i]=='.' && (input[i+1]==FILE_SEPARATOR_CHAR || input[i+1]==FILE_SEPARATOR_CHAR_BIS) )
    {
      i+=1;
      pos = i+1;
    }
    else
      break;
  strcpy(output,&input[pos]);
  while (!finished)
  {
    int pos1=-1;
    int pos2 =-1;
    for (i=0;output[i]!='\0';i++)
    {
      if (output[i]==FILE_SEPARATOR_CHAR || output[i]==FILE_SEPARATOR_CHAR_BIS)
      {
        if (output[i+1]==FILE_SEPARATOR_CHAR || output[i+1]==FILE_SEPARATOR_CHAR_BIS || output[i+1]=='\0')
        {
          pos1=i;
          pos2=i+1;
          break;
        }
        else if (output[i+1]=='.')
        {
          if (output[i+2]==FILE_SEPARATOR_CHAR || output[i+2]==FILE_SEPARATOR_CHAR_BIS || output[i+2]=='\0')
          {
            pos1=i;
            pos2=i+2;
            break;
          }
        }
      }
    }
    finished= output[i]=='\0';
    if (!finished)
    {
      int diff=pos2-pos1;
      int k;
      for(k=pos1;output[k+diff]!='\0';k++)
        output[k]=output[k+diff];
      output[k]='\0';
    }
  }
  char *it=output;
  while (*it)
  {
    if (*it==FILE_SEPARATOR_CHAR_BIS)
      *it=FILE_SEPARATOR_CHAR;
#if defined(OS_WIN32)
    *it=tolower(*it);
#endif
    it++;
  }
}

