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

#include "coveragescannerpdef.h"
#include "compilerwrapper.h"
#include "compilerfactory.h"
#include "debug.h"
#include "system.h"
#include "option.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined(OS_LINUX) || defined(OS_MACX)
#define SUPPRESS_PATH_ENTRY 1
#include <unistd.h>
#elif defined(OS_WIN32)
#define SUPPRESS_PATH_ENTRY 0
#include <windows.h>
#include <process.h>
#define strncasecmp strnicmp
#endif
#include "filtersource.h"

CompilerFactory& CompilerFactory::instance() 
{
  static CompilerFactory _instance;
  return _instance;
}

CompilerInterface* CompilerFactory::create(const Option  &option)
{
  DEBUG3("option.tooName()=%s, option.applicationPath()=%s\n",option.toolName().c_str(),option.applicationPath().c_str());
  if (CompilerFactory::profile_name(option.toolName(),option.applicationPath()).empty())
     return NULL;
  return new CompilerWrapper(option);
}

std::string  CompilerFactory::application_path(const std::string &exec_param)
{
  FUNCTION_TRACE;

  std::string exe_param=System::appendExecSuffix(std::string(exec_param));
  std::string  file_path;

  int slash_count=0;
  int i;
  int lg=exe_param.length();
  for (i=0;i<lg;i++)
    if (exe_param[i]==FILE_SEPARATOR_CHAR || exe_param[i]==FILE_SEPARATOR_CHAR_BIS) 
      slash_count++;

  if ( slash_count>0 )
  { /* absolute of relative path */
    file_path=exe_param; 
  }
  else 
  {
    struct stat info;
#if defined(OS_WIN32)
    if (stat(exe_param.c_str(),&info)==0)
    { /* file exists */
      file_path=exe_param; 
    }
    else
#endif
    {
      char *path = getenv( "PATH" );
      if (!path)
      {
        FATAL1("System variable PATH not defined");
      }
      int path_lg=strlen(path);
      int pos1,pos2;
      DEBUG2("getenv(PATH)=%s\n", path);
      for (pos1=0,pos2=0;pos2<path_lg;pos2++)
      {
        if ( (path[pos2]==PATH_SEPARATOR_CHAR) || (pos2==path_lg-1) )
        {
          char *p;

          p=(char*)MALLOC((pos2-pos1+1+1+lg)*sizeof(char));
          memcpy(p,&path[pos1],pos2-pos1);
          if (p[pos2-pos1-1]!=FILE_SEPARATOR_CHAR && p[pos2-pos1-1]!=FILE_SEPARATOR_CHAR_BIS)
          {
            p[pos2-pos1]=FILE_SEPARATOR_CHAR ;
            p[pos2-pos1+1]='\0';
          }
          else
            p[pos2-pos1]='\0';
          strcat(p,exe_param.c_str());
          pos1=pos2+1;
          if (stat(p,&info)==0)
          { /* file exists */
            file_path=std::string(p); 
            break;
          }
          FREE(p);
        }
      }
    }
  }

  if (file_path.empty())
    file_path=exe_param;

  char resolved_file_path[MAX_PATH];
  realPath(file_path.c_str(),resolved_file_path);

  return std::string(resolved_file_path);
}


bool CompilerFactory::profile_existing(const std::string &prof, const std::string &additional_path) 
{
  std::string profile_file(prof);
  if (profile_file.length()>4 && (strcasecmp(profile_file.substr(profile_file.length()-4).c_str(),".exe")==0))
    profile_file = profile_file.substr(0,profile_file.length()-4);
  profile_file+=".cspro";
  { // Search directly
    if (additional_path.length()>0)
    {
      std::string prof_path=additional_path+FILE_SEPARATOR_CHAR+profile_file;
      FILE *f=fopen(prof_path.c_str(),"r");
      if (f)
      {
        fclose(f);
        return true;
      }
    }
  }

  { // Search in path
    std::string found_file=findFileInPath(profile_file,0);
    if (!found_file.empty())
      return true;
  }
  return false;
}

void CompilerFactory::extract_tool_name (const std::string &input, std::string &tool_name,bool suppress_exe_extension,bool &inactive)
{
  FUNCTION_TRACE;
  std::string tool_path;
  {
    std::string appli_path=CompilerFactory::application_path(input.c_str());
    char file_abs[MAX_PATH];
    if (realPath(appli_path.c_str(),file_abs)!=NULL)
      appli_path=std::string(file_abs);
    int lg=appli_path.length();
    int i;
    int pos=-1;
    inactive=false;
    for (i=lg-1;i>=0;i--)
    {
      if (appli_path[i]==FILE_SEPARATOR_CHAR || appli_path[i]==FILE_SEPARATOR_CHAR_BIS)
      {
        pos=i;
        break;
      }
    }
    if (pos!=-1)
      tool_path=appli_path.substr(0,pos+1);
  }

  int i;
  int pos=-1;
  inactive=false;
  int lg=input.length();
  for (i=lg;i>=0;i--)
  {
    if (input[i]==FILE_SEPARATOR_CHAR || input[i]==FILE_SEPARATOR_CHAR_BIS)
    {
      pos=i;
      break;
    }
  }
  int input_pos_lg=strlen(&input[pos+1]);
  DEBUG5("input=%s tool_name=%s pos=%i input_pos_lg=%i\n",input.c_str(),tool_name.c_str(),pos,input_pos_lg);
  bool found=false;
  if (strncasecmp(&(input.c_str())[pos+1],"cs",2)==0)
  {
    tool_name=input.substr(pos+3);
    found=profile_existing(tool_name,tool_path.c_str());
    DEBUG3("tool_name=%s profile_existing=%s\n",tool_name.c_str(),(found?"true":"false"));
  }
  if (!found && (input_pos_lg>3 && strncasecmp(&(input.c_str())[pos+1+input_pos_lg-3],"-cs",3)==0))
  {
    tool_name= input.substr(pos+1);
    tool_name= tool_name.substr(0,tool_name.length()-3);
    found=profile_existing(tool_name,tool_path);
    DEBUG3("tool_name=%s profile_existing=%s\n",tool_name.c_str(),(found?"true":"false"));
  }
  if (!found && (input_pos_lg>7 && strncasecmp(&(input.c_str())[pos+1+input_pos_lg-7],"-cs.exe",7)==0))
  {
    tool_name= input.substr(pos+1);
    tool_name= tool_name.substr(0,tool_name.length()-7);
    tool_name+=".exe";
    found=profile_existing(tool_name,tool_path.c_str());
    DEBUG3("tool_name=%s profile_existing=%s\n",tool_name.c_str(),(found?"true":"false"));
  }
  if (!found)
  {
    inactive=true;
    tool_name= input.substr(pos+1);
    DEBUG2("tool_name (%s) not found\n",tool_name.c_str());
  }

  if (suppress_exe_extension)
  { /*suppression of .exe extension */
    tool_name=System::suppressExecSuffix(tool_name);
    DEBUG2("tool_name=%s\n",tool_name.c_str());
  }
  DEBUG2("tool_name=%s\n",tool_name.c_str());
}

std::string CompilerFactory::findFileInPath(const std::string& name,int index)
{
  FUNCTION_TRACE;
  int found_id=0;
  const char *path_env=getenv("PATH");
  if (!path_env)
  {
    FATAL1("System variable PATH not defined");
  }
  DEBUG2("getenv(PATH)=%s\n", path_env);

  /* extract the file name */
  bool inactive=false;
  int name_lg=name.length();
  std::string filename;
  CompilerFactory::extract_tool_name(name,filename,false,inactive);

  int path_lg=strlen(path_env)+name_lg+2;
  char *path=(char*)MALLOC(path_lg);
  std::string file;

  const char *pos1=path_env;
  char sep[2];
  sep[0]=PATH_SEPARATOR_CHAR;
  sep[1]='\0';
  const char *pos2=strstr(path_env,sep );
  for (;pos2!=NULL;pos2=strstr(pos2,sep ))
  {
    /* extract the path */
    size_t plg=(size_t)(pos2)-(size_t)(pos1);
    if (plg>0)
      memcpy(path,pos1,plg);
    path[plg]='\0';

    /* calculate the filename */
    file = std::string(path);
    file+=FILE_SEPARATOR_CHAR ;
    file+=filename;
    file=System::appendExecSuffix(file);

    /* check if existing */
    struct stat info;
    bool found=false;
    if (stat(file.c_str(),&info)==0)
    {
      if (index==found_id)
      {
        FREE(path);
        DEBUG4("findFileInPath(%s,%i)->%s\n",name.c_str(),index,file.c_str());
        return file;
      }
      found_id++;
      found=true;
    }
    pos2++;
    pos1=pos2;
  }
  FREE(path);
  DEBUG3("findFileInPath(%s,%i)->not found\n",name.c_str(),index);
  return std::string();
}

bool CompilerFactory::name_check(const std::string &exe_str,const std::string &compiler_name, bool arg0) 
{
  FUNCTION_TRACE;
  std::string command;

  int lg_arg = exe_str.length();

  if (arg0)
  {
    if (compiler_name.empty())
    {
      if ( (lg_arg==2) && exe_str[0]=='c' && exe_str[1]=='s')
        return true;
      else if (lg_arg>2)
      {
        command =  exe_str;
        for (int i=0;i<lg_arg-1;i++)
        {
          if (exe_str[i]==FILE_SEPARATOR_CHAR || exe_str[i]==FILE_SEPARATOR_CHAR_BIS)
            command=  exe_str.substr(i+1);
        }
        return true;
      }
    }
    else
    {
      int lg_compiler_name = compiler_name.length();

      if ( (lg_arg==lg_compiler_name+2) && (exe_str.substr(2)==compiler_name) && exe_str[0]=='c' && exe_str[1]=='s')
        return true;
      else if ( (lg_arg==lg_compiler_name) && (exe_str==compiler_name) )
        return true;
      else if (lg_arg>lg_compiler_name+2)
      {
        command = exe_str.substr(lg_arg-lg_compiler_name);
        return true;
      }
    }
  }
  else
  {
    if (compiler_name.empty())
      return true;
    else
      if ( exe_str==compiler_name )
        return true;
  }

  return false;
}

std::string  CompilerFactory::findNativeTool(const std::string &profile_tool_name,const Argv & _args) 
{
  FUNCTION_TRACE;
  Argv arg(_args);
  bool tool_found=false;
  std::string file;
  struct stat info_me;
  std::string exe_param=System::appendExecSuffix(arg[0]);
  std::string appli=application_path(exe_param);
  DEBUG3("exe_param %s -> %s\n",exe_param.c_str(),appli.c_str());
  if (profile_tool_name.empty())
  {
     file=appli;
  }
  else
  {
     if (stat(appli.c_str(),&info_me)!=0)
        FATAL2("Could not get information from %s\n",appli.c_str());
     exe_param=System::appendExecSuffix(profile_tool_name);
     int index=-1;
     file=application_path(exe_param);
     while (!file.empty())
     {
        struct stat info;
        DEBUG2("Tool: %s\n",file.c_str());
        char file_abs[MAX_PATH];
        if (realPath(file.c_str(),file_abs)!=NULL)
        {
           DEBUG2("Tool abs: %s\n",file_abs);
           if (stat(file_abs,&info)==0)
           {
              if ( ! (info.st_ctime==info_me.st_ctime
                       && info.st_size==info_me.st_size) )
              { // tool found
                 tool_found=true;
                 DEBUG2("Tool found: %s\n",file.c_str());
                 break;
              }
              else
              { // suppress path entry
#if SUPPRESS_PATH_ENTRY
                 suppressPathEntry(file.c_str());
#endif
                 setenv("COVERAGESCANNER_ARGS","",1);
              }
           }
        }
        index++;
        file=findFileInPath(exe_param,index);
     } 

     if (!tool_found)
        FATAL2("Could not find application: %s\n",exe_param.c_str());
  }
  return file;
}


bool CompilerFactory::find_tool_type (const char* argv0,bool option,const std::string &appli_path,std::string &option_tool_name, bool &profile_inactive) 
{
  FUNCTION_TRACE;
  DEBUG4("find_tool_type (argv0=%s,option=%s,appli_path=%s)\n",argv0,(option?"true":"false"),appli_path.c_str());
  std::string exe_param=System::appendExecSuffix(argv0);
  if (name_check(exe_param,std::string(),!option))
  {
    std::string name;
    profile_inactive=false;
    if (option)
      name=exe_param;
    else
      extract_tool_name(exe_param,name,true,profile_inactive);
    option_tool_name=name;
    struct stat info;
    DEBUG2("option_tool_name=%s\n",option_tool_name.c_str());
    std::string pf=profile_name(option_tool_name,appli_path);
    DEBUG2("pf=%s\n",pf.c_str());
    if (stat(pf.c_str(),&info)!=0)
    {
      option_tool_name=findFileInPath(name);
      DEBUG2("option_tool_name=%s\n",option_tool_name.c_str());
      std::string pf=profile_name(option_tool_name,appli_path);
      DEBUG2("pf=%s\n",pf.c_str());
      if (pf.length()>0 && (stat(pf.c_str(),&info)!=0) )
      {
         DEBUG2("profile (%s) not found\n",option_tool_name.c_str());
         option_tool_name.clear();
      }
    }
    return true;
  }
  return false;
}

std::string CompilerFactory::profile_name(const std::string &option_tool_name,const std::string &appli_path) 
{
  FUNCTION_TRACE;
  int i,lg;
  std::string init_file;
  DEBUG3("option_tool_name=%s, appli_path=%s\n",option_tool_name.c_str(),appli_path.c_str());

  const std::string &tool_name=option_tool_name;
  if (tool_name.empty())
  {
    DEBUG1("tool_name empty\n");
    return std::string();
  }
  DEBUG2("tool_name=%s\n",tool_name.c_str());
  lg=appli_path.length();
  for (i=lg-1;i>=0;i--)
  {
    if (appli_path[i]==FILE_SEPARATOR_CHAR || appli_path[i]==FILE_SEPARATOR_CHAR_BIS)
    {
      init_file=appli_path.substr(0,i);
      init_file+=FILE_SEPARATOR_CHAR;
      init_file+=tool_name ;
      init_file=System::suppressExecSuffix(init_file);
      init_file+=".cspro";
      DEBUG2("init_file=%s\n",init_file.c_str());
      return init_file;
    }
  }

  /* no path in the appli_name */
  init_file=tool_name;
  init_file=System::suppressExecSuffix(init_file);
  init_file+=".cspro";
  DEBUG2("init_file=%s\n",init_file.c_str());
  return init_file;
}

void CompilerFactory::suppressPathEntry(const char *abs_filename)
{
  FUNCTION_TRACE;
  char *filepath=STRDUP(abs_filename);
  char *rel=NULL;
  for (char *pos=filepath;*pos!='\0';pos++)
  {
    if (*pos==FILE_SEPARATOR_CHAR || *pos==FILE_SEPARATOR_CHAR_BIS)
      rel=pos;
  }
  if (rel!=NULL)
  {
    *rel='\0';
  }
  else
  {
    FREE(filepath);
    return;
  }

  const char *path_env=getenv("PATH");
  int path_env_lg=strlen(path_env);
  int filepath_lg=strlen(filepath);
  char *new_path=(char*)MALLOC(path_env_lg+1);
  memset(new_path,0,path_env_lg);
  char *new_path_pos=new_path;
  for (const char *path_env_pos=path_env;*path_env_pos!='\0';path_env_pos++)
  {
    bool copy_src=true;
    if (path_env_pos==path_env)
    {
      if (strncmp(path_env_pos,filepath,filepath_lg)==0 && (*(path_env_pos+filepath_lg)=='\0' || *(path_env_pos+filepath_lg)==PATH_SEPARATOR_CHAR))
      { // This path should be execluded
        path_env_pos+=filepath_lg;
        copy_src=false;
      }
    }
    else if (*path_env_pos==FILE_SEPARATOR_CHAR || *path_env_pos==FILE_SEPARATOR_CHAR_BIS)
    {
      if (strncmp(path_env_pos+1,filepath,filepath_lg)==0 && (*(path_env_pos+1+filepath_lg)=='\0' || *(path_env_pos+1+filepath_lg)==PATH_SEPARATOR_CHAR))
      { // This path should be execluded
        path_env_pos+=1+filepath_lg;
        copy_src=false;
      }
    }
    if (copy_src)
    {
      *new_path_pos=*path_env_pos;
      new_path_pos++;
    }
  }

  if (setenv("PATH",new_path,1)==0)
  {
    DEBUG2("setenv(PATH)=%s\n",new_path);
  }
  else
  {
    DEBUG2("FAILURE: setenv(PATH)=%s\n",new_path);
  }
  FREE(new_path);
  FREE(filepath);
}

