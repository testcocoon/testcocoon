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
#include "source.h"
#include "compiler.h"
#include "filename.h"
#include "parsemisc.h"
#include "compilerinterface.h"
#include "libgen.h"
#include "system.h"
#include "fileinfo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LibGen::LibGen()
{
  FUNCTION_TRACE;
    datas=NULL;
    nb_data=0;
    clear();
}

LibGen::~LibGen()
{
  FUNCTION_TRACE;
  clear();
}

void LibGen::clear()
{
  FUNCTION_TRACE;
  for (int i=0;i<nb_data;i++)
  {
    FREE(datas[i].filename_rel);
    FREE(datas[i].filename_abs);
  }

  if (datas!=NULL) FREE(datas);
  datas=NULL;
  nb_data=0;
}

bool LibGen::isAppend(const char *filename_rel) const
{
  FUNCTION_TRACE;
  int count = nb_data;
  for (int i=count-1;i>=0;i--)
  {
    if (strcmp(datas[i].filename_rel,filename_rel)==0) 
    {
      char filename_abs[MAX_PATH];
      realPath(filename_rel,filename_abs);
      if (strcmp(datas[i].filename_abs,filename_abs)==0)
        return true;
    }
  }
  return false;
}

bool LibGen::appendSource(const char *filename_rel,bool import_symbols,unsigned long signature)
{
  FUNCTION_TRACE;
  ASSERT(text!=NULL);
  DEBUG3("CSLIB:append %s [sig=%lx]\n",filename_rel,signature);
  if (isAppend(filename_rel))
  {
    DEBUG3("CSLIB:NOT append %s [sig=%lx]\n",filename_rel,signature);
    return false;
  }
  char filename_abs[MAX_PATH];
  realPath(filename_rel,filename_abs);
  if ( (nb_data&0xFF) == 0 )
     datas=(data_t*)REALLOC((void*)datas,sizeof(data_t)*(nb_data+257));
  datas[nb_data].signature=signature;
  datas[nb_data].import_symbols=import_symbols;
  datas[nb_data].filename_rel=STRDUP(filename_rel);
  datas[nb_data].filename_abs=STRDUP(filename_abs);
  nb_data++;
  DEBUG4("CSLIB:append %s(%s) [sig=%lx]:recorded\n",filename_rel,filename_abs,signature);
  return true;
}


void LibGen::generate(CompilerInterface &compiler,const char *default_csexe,bool lock_csexe,bool compile)
{
  FUNCTION_TRACE;
  char *command;
  std::string ofile= compiler.csmes_library_object_name();
  std::string filename= csmes_library_source_name();

  save_source(filename.c_str(),compiler,default_csexe,lock_csexe);
  if (compiler.useCompilerToGenerateObjects() && compile)
  {
    if (!compiler.compile_file(&command,filename.c_str(),ofile.c_str()))
    {
      DEBUG2("Filename=%s\n",filename.c_str());
      DEBUG2("OFile=%s\n",ofile.c_str());
      FATAL1("Could not generate the internal CoverageScanner compiling command\n");
    }
    if ( System::exec(command,NULL)!=0 )
    {
      compiler.display_temporary_files();
      DEBUG2("Command=%s\n",command);
      FATAL1("could not generate the CoverageScanner library\n");
    }
    FREE (command);
  }
}

void LibGen::escape(const char*text,char *text_escape)
{
  FUNCTION_TRACE;
    for (int i=0;text[i]!='\0';i++)
    {
        if (text[i]=='\\')
        {
            *text_escape='\\';
            text_escape++;
            *text_escape='\\';
        }
        else
            *text_escape=text[i];
        text_escape++;
    }
    *text_escape='\0';
}

void LibGen::fputs_trace(const char *str,FILE *f) const
{
  FUNCTION_TRACE;
	fputs(str,f);
#ifdef LOG
	DEBUG2("%s",str);
#endif
}


#ifdef LOG
void LibGen::PDEBUG() const
{
  FUNCTION_TRACE;
}
#endif


