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
#include "filename.h"
#include "compilerinterface.h"
#include "option.h"
#include "system.h"
#include "utf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "regexp_custom.h"


CompilerInterface::~CompilerInterface()
{
  FUNCTION_TRACE;
}

void CompilerInterface::init()
{
  FUNCTION_TRACE;
  _language=LANGUAGE_AUTO;
  options_p=NULL;
}

CompilerInterface::CompilerInterface(const Option &t)
{
  FUNCTION_TRACE;
  init();
  options_p = &t;

  setArgs(options_p->param_args(),args_org);
  setArgs(options_p->param_args());
  preprocessorfile[0]=0;
}

void CompilerInterface::setLanguage(language_t l) 
{
  if (l!=LANGUAGE_AUTO)
  {
    switch (_language)
    {
      case LANGUAGE_C:
        if (_language!=l)
        {
          DEBUG1("Switching in C mode\n");
        }
        break;
      case LANGUAGE_CPP:
        if (_language!=l)
        {
          DEBUG1("Switching in C++ mode\n");
        }
        break;
      case LANGUAGE_AUTO:
        break;
    }
  }
  _language=l; 
}

void CompilerInterface::setArgs(const Argv &v)
{
  FUNCTION_TRACE;
  setArgs(v,args);
}

void CompilerInterface::setArgs(const Argv &in,Argv &out)
{
  FUNCTION_TRACE;
  out=in;
}

CompilerInterface::extension_t CompilerInterface::extension(const char *name) const
{
  FUNCTION_TRACE;
  int pos;
  int lg =strlen(name);
  int i;

  for (i=0;ignore_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(ignore_ext()[i]))>0) && ext_strcmp(&name[pos],ignore_ext()[i])==0 )
      return EXT_IGNORE;
  }

  for (i=0;cpp_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(cpp_ext()[i]))>0) && ext_strcmp(&name[pos],cpp_ext()[i])==0 )
    {
      switch (_language)
      {
        case LANGUAGE_C:
          return EXT_SOURCE_C;
        case LANGUAGE_CPP:
        case LANGUAGE_AUTO:
          return EXT_SOURCE_CPP;
      }
    }
  }

  for (i=0;cs_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(cs_ext()[i]))>0) && ext_strcmp(&name[pos],cs_ext()[i])==0 )
      return EXT_SOURCE_CS;
  }

  for (i=0;c_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(c_ext()[i]))>0) && ext_strcmp(&name[pos],c_ext()[i])==0 )
      switch (_language)
      {
        case LANGUAGE_C:
        case LANGUAGE_AUTO:
          return EXT_SOURCE_C;
        case LANGUAGE_CPP:
          return EXT_SOURCE_CPP;
      }
  }

  for (i=0;dyn_lib_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(dyn_lib_ext()[i]))>0) && ext_strcmp(&name[pos],dyn_lib_ext()[i])==0 )
      return EXT_DYN_LIB;
  }

  for (i=0;static_lib_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(static_lib_ext()[i]))>0) && ext_strcmp(&name[pos],static_lib_ext()[i])==0 )
      return EXT_STATIC_LIB;
  }

  for (i=0;obj_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(obj_ext()[i]))>0) && ext_strcmp(&name[pos],obj_ext()[i])==0 )
      return EXT_OBJECT;
  }

  for (i=0;pdb_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(pdb_ext()[i]))>0) && ext_strcmp(&name[pos],pdb_ext()[i])==0 )
      return EXT_FILE_SBR;
  }

  for (i=0;sbr_ext()[i]!=NULL;i++)
  {
    if ( ((pos=lg-strlen(sbr_ext()[i]))>0) && ext_strcmp(&name[pos],sbr_ext()[i])==0 )
      return EXT_FILE_SBR;
  }
  return EXT_UNKNOWN;
}




bool CompilerInterface::sources(FileName *filenames) const
{
  FUNCTION_TRACE;
  int i;
  filenames->clear();
  for (i=1;i<args.argc();i++)
  {
    const char *file=args[i];
    if (!isDestination(file))
      if (isSource(file)) 
        filenames->append(file);
  }

  return true;
}

bool CompilerInterface::objects(FileName *filenames) const
{
  FUNCTION_TRACE;
  int i;
  filenames->clear();
  for (i=1;i<args.argc();i++)
  {
    const char *file=args[i];
    if (!isDestination(file))
      if (isObject(file))
        filenames->append(file);
  }

  return true;
}

bool CompilerInterface::isLibrary(const char *filename) const
{
  FUNCTION_TRACE;
  extension_t e=extension(filename);
  switch(e)
  {
    case EXT_STATIC_LIB:
    case EXT_DYN_LIB:
      return true;
    default: ;
  }
  return false;
}

bool CompilerInterface::isObject(const char *filename) const
{
  FUNCTION_TRACE;
  extension_t e=extension(filename);
  switch(e)
  {
    case EXT_OBJECT:
      return true;
    default: ;
  }
  return false;
}

bool CompilerInterface::isSource(const char *filename) const
{
  FUNCTION_TRACE;
  extension_t e=extension(filename);
  switch(e)
  {
    case EXT_IGNORE:
    case EXT_SOURCE_C:
    case EXT_SOURCE_CS:
    case EXT_SOURCE_CPP:
      return true;
    default: ;
  }
  return false;
}

bool CompilerInterface::isExitFunction(const char *symbol) const 
{
  FUNCTION_TRACE;
  int i;
  const char *s;
  for (i=0;(s=exit_funct_ext()[i])!=NULL;i++)
  {
    if ( strcmp(s,symbol)==0 )
      return true;
  }
  return false;
}

bool CompilerInterface::restrictArgs(const char* selected_source,Argv &backup_args ) 
{
  FUNCTION_TRACE;
  int i;
  backup_args=param_args();

  Argv new_args;
  for (i=0;i<backup_args.argc();i++)
  {
    const char *file=backup_args[i];
    if (!isSource(file) || filename_strcmp(selected_source,file)==0)
      new_args.append(file);
  }

  setArgs(new_args);
  return true;
}

void CompilerInterface::disableCoverageScannerPerEnvironmentVariable()
{
  setenv("COVERAGESCANNER_DISABLE","111",1);
}

bool CompilerInterface::isCoverageScannerDisabledPerEnvironmentVariable()
{
  const char *coveragescanner_disable_env=getenv("COVERAGESCANNER_DISABLE");
  return coveragescanner_disable_env!=NULL;
}

bool CompilerInterface::responseFileContainsToArgumentList (const std::string &text,Argv &args) const
{
  FUNCTION_TRACE;
  /* extract all options */
  int pos1=0;
  int pos2=0;
  bool in_string=false;
  int text_lg=text.length();

  for (int i=0;i<text_lg;i++)
  {
    char c=text[i];
    if (c=='"')
      in_string = !in_string;
    if ( !in_string 
        && ( c=='\t' || c==' ' || c=='\n' || c=='\r' ) ) 
    {
      pos2=i;
      if (pos2!=pos1)
      {
        std::string item=text.substr(pos1,pos2-pos1);
        /* add the item */
        System::stripQuotes(item);
        args.append(item.c_str());
      }
      pos1=pos2+1;
    }
  }
  return true;
}


bool CompilerInterface::responseFileToArgumentList (const std::string& rsp_file,Argv &args) const
{
  FUNCTION_TRACE;
  bool res=false;
  args.clear();
  FILE *f=fopen(rsp_file.c_str(),"r");
  if (f)
  {
    int sz;
    std::string text;
    char tmp[CHAINE_LEN*16];
    text.clear();

    while ( (sz=fread(tmp,sizeof(char),CHAINE_LEN*16,f)) > 0)
      text+=std::string(tmp,sz);
    
    fclose(f);
    if (!text.empty())
    {
      std::string rsptext;
      switch (text_encoding(text))
      {
        case ENC_UTF16:
          { // Windows .NET 2005 workaround
            DEBUG1("UTF16 Response file\n");
            wchar_t tmp[CHAINE_LEN];
            wchar_t *textws=NULL;
            int sz;
            int tmp_sz=0;
            char *text=NULL;
            f=fopen(rsp_file.c_str(),"rb");
            if (f==NULL) return false;

            while ( (sz=fread(tmp,sizeof(wchar_t),CHAINE_LEN,f)) > 0)
            {
              textws=(wchar_t *)REALLOC(textws,sizeof(wchar_t)*(sz+tmp_sz));
              memcpy(&textws[tmp_sz],tmp,sz*sizeof(wchar_t));
              tmp_sz+=sz;
            }
            fclose(f);
            text=(char*)MALLOC(sizeof(char)*(tmp_sz+1+CHAINE_LEN));
            memset(text,0,tmp_sz+1);
            int offset=0;
            while (textws[offset]==0xFEFF)
              offset++;
            DEBUG2("offset=%i\n",offset);
            wcstombs(text,&textws[offset],tmp_sz-offset);
            text[tmp_sz-offset]='\0';
            DEBUG2("Response file contain:%s\n",text);
#if LOG
            for (int i=0;i<tmp_sz;i++)
            {
               DEBUG5("textws[%i]=%4.4X text[%i]=%c\n",i,textws[i],i,text[i]);
            }
#endif
            FREE(textws);
            rsptext=std::string(text);
            FREE(text);
          }     
          break;
        case ENC_UTF8:
          DEBUG1("UTF8 Response file\n");
          rsptext=utf16_to_ascii(utf8_to_utf16(text));
          break;
        case ENC_ASCII:
          DEBUG1("ASCII Response file\n");
          rsptext=text;
          break;
      }
      rsptext=ascii_correct_eol(rsptext);


      if (!rsptext.empty())
      {
        DEBUG2("Response file contain:%s\n",rsptext.c_str());
        rsptext+="\n"; // add a separator at the end to simplify the process
        res=responseFileContainsToArgumentList(rsptext,args);
      }
    }
  }
  return res;
}

