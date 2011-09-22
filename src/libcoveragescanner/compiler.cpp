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
#include "compiler.h"
#include "csmesfile.h"
#include "filename.h"
#include "tmpfile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <list>
#if defined(OS_WIN32)
#include <windows.h>
#include <process.h>
#define strncasecmp strnicmp
#endif
#include "source.h"
#include "system.h"
#include "expressions.h"
#include "filename.h"
#include "functions.h"
#include "parser.h"
#include "libgen.h"
#include "c_cpp_libgen.h"
#include "cs_libgen.h"
#include <sys/types.h>
#include "regexp_custom.h"
#include "compilerinterface.h"
#include "compilerfactory.h"
#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <sys/stat.h>
#if defined(OS_MACX) || defined(OS_LINUX) || !YY_NO_UNISTD_H 
#include <unistd.h>
#endif

#define  PREPROCESS_BACKGROUND 1

Compiler::Compiler(Option &t)
{
  FUNCTION_TRACE;
  option_p=&t;
  cslib_p = NULL;
  compiler_p = CompilerFactory::create(t);
}

Compiler::~Compiler()
{
  FUNCTION_TRACE;
  if (cslib_p)
    delete cslib_p ;
  delete compiler_p;
}

LibGen * Compiler::cslib()
{
  if (cslib_p==NULL)
  {
    switch (compiler_p->codeStyle())
    {
      case Expression::CODESTYLE_CPP:
        cslib_p =  new CppLibGen();
        break;
      case Expression::CODESTYLE_CS:
        cslib_p =  new CsLibGen();
        break;
      default:
        FATAL1("Internal error");
    }
  }
  return cslib_p;
}

bool  Compiler::generate()
{
  FUNCTION_TRACE;
  int i;
  switch (compiler_p->type())
  {
    case CompilerInterface::LINKER:
    case CompilerInterface::COMPILER_LINKER:
      return generate_one();
    case CompilerInterface::COMPILER:
      for (i=0;i<compiler_p->param_args().argc();i++)
      {
        if (compiler_p->isSource(compiler_p->param_args()[i]))
        {
          char *source_file=STRDUP(compiler_p->param_args()[i]);
          Argv args;
          compiler_p->restrictArgs(source_file,args);
          if (!generate_one())
            return false;
          compiler_p->setArgs(args);
          FREE(source_file);
        }
      }
      break;
    default:
      return false;
  }
  return true;
}

bool  Compiler::generate_one()
{
  FUNCTION_TRACE;
#if LOG
  DEBUG1("Command Line:");
  for (int j=0; compiler_p->param_args()[j] ; j++)
  {
    DEBUG2("%s ",compiler_p->param_args()[j]);
  }
  DEBUG1("\n");
#endif
  char *destination;
  bool in_cmd_line;
  compiler_p->destination(&destination,in_cmd_line);
  std::string csmes_filename=Source::csmes_filename(destination);
  CSMESFile csmes_file;
  bool ret=csmes_file.open(csmes_filename.c_str(),CSMESFile::NEW_RW);
  if (!ret)
    FATAL3("Could not write file '%s':%s\n",csmes_filename.c_str(),csmes_file.getLastErrorMsg().c_str());

  DEBUG1("Add intrumentation to source code\n");
  add_instrumentation_to_source_files(csmes_file,option_p->preprocessInBackground());

  DEBUG1("Add intrumentation to objects\n");
  add_instrumentation_to_object_files(csmes_file);

  DEBUG1("Add missing sources\n");
  add_missing_sources(csmes_file);

  FREE(destination);
  bool no_cslib_generation=csmes_file.empty();

  /* compile */
  switch (compiler_p->type())
  {
    case CompilerInterface::LINKER:
      DEBUG1("link all files\n");
      compile_and_link_all_files(true,no_cslib_generation);
      break;
    case CompilerInterface::COMPILER_LINKER:
      DEBUG1("Compile and link all files\n");
      compile_and_link_all_files(false,no_cslib_generation);
      break;
    case CompilerInterface::COMPILER:
      DEBUG1("Compile all files\n");
      compile_file();
      break;
    default:
      FATAL1("Wrong compiler type\n");
  }

  // The csmes file needs to be closed at the end to
  // ensure that it is not outdated in comparison to the oject file
  csmes_file.close();
  if (no_cslib_generation)
  {
    DEBUG2("No instrumentations delete .csmes file '%s'\n",csmes_filename.c_str());
    System::delete_file(csmes_filename.c_str());
  }

  return true;
}

static System *system_p=NULL;
static int yy_input_func(char *buf ,int max_buf)
{
  FUNCTION_TRACE;
  return system_p->readStdout(buf,max_buf);
}

bool Compiler::add_instrumentation_to_source_files(CSMESFile &csmes_file,bool preprocess_background) 
{
  FUNCTION_TRACE;
  int nb_files;
  FileName source_files ;

  compiler_p->sources(&source_files);
  nb_files = source_files.count();
  cslib()->clear();

  /* preprocess to add the instrumentation */
  for (int i=0;i<nb_files;i++)
  {
    const char *name = source_files.at(i);
    if (compiler_p->ignoreSource(name))
      continue;

    Functions *functions_p=new Functions(&option_p->getFilterFunction());
    Defines defines=compiler_p->defineFromCommandLine();
    LinePos *source_filenames_p=new LinePos;
    Expressions *expressions_p = new Expressions;
    Source *instrumentation_p= new Source();
    DEBUG2("Begin processing file %s\n",name);
    std::string csmes=Source::csmes_c_filename(name);
    Source *source_p=new Source();
    if (compiler_p->usePreprocessor())
    {
      char *command;

      if ( !compiler_p->preprocessor(name,&command))
      {
        FATAL2("Could not generate preprocessor command for processing the file %s\n",name);
      }

      DEBUG2("Preprocessor CMD csmes file:%s\n",csmes.c_str());
      DEBUG2("Preprocessor CMD:%s\n",command);
      if (preprocess_background && !compiler_p->preprocessor_use_temp_filename())
      {
        DEBUG1("Preprocessing in background\n");
        System sys;
        system_p=&sys;
        if ( ! sys.call(command,true) )
        {
          ERROR2("Could not preprocess the file %s\n",name);
          Compiler::display_temporary_files();
          EXIT(-1);
        }
        FREE(command);
        command=NULL;
        if (!analyse(name,csmes.c_str(),*expressions_p,*source_p,*source_filenames_p,*functions_p,*option_p,defines,*compiler_p,yy_input_func))
        {
          ERROR2("Could not insert insert instrumentation in file %s\n",name);
          expressions_p->clear();
          EXIT(-1);
        }
        int exit_val=sys.exitValue();
        if ( exit_val!=0 )
        {
          ERROR3("Could not preprocess the file '%s', exit value %i\n",name,exit_val);
          Compiler::display_temporary_files();
          EXIT(exit_val);
        }
      }
      else
      {
        const char *outfile=compiler_p->preprocessor_use_temp_filename()?NULL:csmes.c_str();
        if ( System::exec(command,outfile)!=0 )
        {
          ERROR2("Could not preprocess the file %s\n",name);
          display_temporary_files();
          if (outfile)
            System::delete_file(outfile);
          EXIT(-1);
        }
        FREE(command);
        command=NULL;
        std::string preprocessed_file_to_analyze=compiler_p->preprocessor_use_temp_filename()?compiler_p->preprocessor_temp_filename(name):csmes;
        if (!analyse(name,preprocessed_file_to_analyze.c_str(),*expressions_p,*source_p,*source_filenames_p,*functions_p,*option_p,defines,*compiler_p,NULL))
        {
          ERROR2("Could not insert insert instrumentation in file %s\n",name);
          expressions_p->clear();
          System::delete_file(preprocessed_file_to_analyze.c_str());
          EXIT(-1);
        }
        System::delete_file(preprocessed_file_to_analyze.c_str());
      }
    }
    else
    { // No preprocessor used
      FILE *fcsmes=fopen(csmes.c_str(),"wb");
      if (fcsmes==NULL)
      {
        FATAL2("Could not open file '%s'\n",csmes.c_str());
      }
      FILE *forig=fopen(name,"rb");
      if (forig==NULL)
      {
        FATAL2("Could not open file '%s'\n",name);
      }
      while (!feof(forig))
      {
        char tmp[CHAINE_LEN];
        int nb_read=fread(tmp,1,sizeof(tmp),forig);
        fwrite(tmp,1,nb_read,fcsmes);
      }
      fclose(forig);
      fclose(fcsmes);

      if (!analyse(name,csmes.c_str(),*expressions_p,*source_p,*source_filenames_p,*functions_p,*option_p,defines,*compiler_p,NULL))
      {
        ERROR2("Could not insert insert instrumentation in file %s\n",name);
        expressions_p->clear();
        EXIT(-1);
      }
    }
    
    expressions_p->filterFunctions(*functions_p);
    expressions_p->optimize(compiler_p->codeStyle());
    unsigned long signature = source_files.signature(i);
#if LOG
    source_p->PDEBUG("preprocessor output");
#endif
    DEBUG2("Source code instrumentation, name='%s'\n",name);
    source_p->instrument(compiler_p->codeStyle(),name,*expressions_p,*instrumentation_p,*compiler_p);
    DEBUG2("Saving measurements instrumentation, name='%s'\n",name);
    source_p->savemeasures(compiler_p->codeStyle(),name,&source_filenames_p->files(),expressions_p,signature,csmes_file);
    DEBUG2("Saving function information, name='%s'\n",name);
    functions_p->save(name,&source_filenames_p->files(),expressions_p,signature,csmes_file);
    DEBUG2("Appending '%s' in the cslib\n",name);
    if (!cslib()->appendSource(name,false,signature))
    {
      INFO2("File '%s' is compiled/linked twice into the project.\n",name);
      WARNING2("File '%s' may not be correctly instrumented\n",name);
    }
    DEBUG1("saving instrumentation\n");
    instrumentation_p->save(csmes.c_str());
#if LOG
    instrumentation_p->PDEBUG("instrumented preprocessed output");
    expressions_p->PDEBUG();
    functions_p->PDEBUG();
#endif

    delete functions_p;
    delete source_filenames_p;
    delete expressions_p;
    delete source_p;
    delete instrumentation_p;
    DEBUG2("End processing file %s\n",name);
  }
  return true;
}


bool Compiler::add_missing_sources(CSMESFile &csmes_file) 
{
  FUNCTION_TRACE;
  csmes_file.add_missing_sources();
  return true;
}

bool Compiler::add_instrumentation_to_object_files(CSMESFile &csmes_file) 
{
  FUNCTION_TRACE;
  int i;
  std::string errmsg;
  FileName object_files;
  compiler_p->objects(&object_files);
  /* treat objects */
  int nb_files = object_files.count();

  std::list<std::string> messages;
  bool measure_added=false;
  for (i=0;i<nb_files;i++)
  {
    const char * name = object_files.at(i);
    if ( addmeasures(name,false,false,csmes_file,errmsg) )
      measure_added=true;
    else
      messages.push_back(std::string("Could not extract instrumentation from '")+std::string(name)+"':\n"+errmsg);
  }

  /* treat libraries */
  FileName library_files ;
  compiler_p->libraries(&library_files);
  nb_files = library_files.count();
  for (i=0;i<nb_files;i++)
  {
    const char * name = library_files.at(i);
    if ( addmeasures(name,true,true,csmes_file,errmsg) )
      measure_added=true;
    else
      messages.push_back(std::string("Could not extract instrumentation from '")+std::string(name)+"':\n"+errmsg);
  }
  if (measure_added)
  {
    for (std::list<std::string>::const_iterator itMessage=messages.begin();itMessage!=messages.end();++itMessage)
    {
      if (option_p->isVerbose())
      {
        INFO2("%s\n",itMessage->c_str());
      }
    }
  }

  return true;
}

bool Compiler::compile_and_link_all_files(bool compile_cs_library,bool no_cslib_generation)
{
  FUNCTION_TRACE;
  char *new_command;
  char *compiler_command=NULL;

  DEBUG1("Compiler+Linker configuration\n");
  bool hide_arguments=true;
  FileName source_files ;
  compiler_p->sources(&source_files);
  int nb_files = source_files.count();
  /* preprocess to add the instrumentation */
  for (int i=0;i<nb_files;i++)
  {
    const char *name = source_files.at(i);
    if (compiler_p->ignoreSource(name))
      hide_arguments=false;
  }
  bool cslib_generation=(!no_cslib_generation) && compiler_p->generateCoverageScannerLibrary();

  if (cslib_generation)
    cslib()->generate(*compiler_p,option_p->defaultCsexeOutput(),option_p->lockCsexe(),compile_cs_library);
  if ( !compiler_p->compiler(&compiler_command,hide_arguments) )
  {
    FATAL1("Could not generate compiler command\n");
  }

  if (cslib_generation)
  { // insert library
    std::string libname;
    if (compiler_p->useCompilerToGenerateObjects() && compile_cs_library)
      libname= compiler_p->csmes_library_object_name();
    else
      libname= cslib()->csmes_library_source_name();

    compiler_p->append_cslibrary(&new_command,compiler_command,libname.c_str());
  }
  else
    new_command=STRDUP(compiler_command);

  if ( System::exec(new_command,NULL,option_p->stdinScript())!=0 )
  {
    display_temporary_files();
    ERROR1("Could not generate object file.\n");
    ERROR2("cmd:%s\n",new_command);
    ERROR2("compiler cmd:%s\n",compiler_command);
    if (option_p->stdinScript())
    {
      ERROR2("Stdin input:%s\n",option_p->stdinScript());
    }
    EXIT(-1);
  }
  FREE(compiler_command);
  compiler_command=NULL;
  return true;
}

bool Compiler::compile_file()
{
  FUNCTION_TRACE;
  char *compiler_command=NULL;
  DEBUG1("Compiler configuration\n");
  bool hide_arguments=true;
  FileName source_files ;
  compiler_p->sources(&source_files);
  int nb_files = source_files.count();
  /* preprocess to add the instrumentation */
  for (int i=0;i<nb_files;i++)
  {
    const char *name = source_files.at(i);
    if (compiler_p->ignoreSource(name))
      hide_arguments=false;
  }
  if ( !compiler_p->compiler(&compiler_command,hide_arguments) )
  {
    FATAL1("Could not generate compiler command\n");
  }
  if ( System::exec(compiler_command,NULL)!=0 )
  {
    display_temporary_files();
    FATAL2("Could not generate object file\ncmd:%s\n",compiler_command);
  }
  FREE(compiler_command);
  return true;
}

const char*  Compiler::error()
{
  FUNCTION_TRACE;
  return "??";
}


bool Compiler::analyse(const char *filename_orig,const char *finput,Expressions &expressions,Source &source,LinePos &filename,Functions& function,const Option &option, const Defines &defines, const CompilerInterface &comp, int (*yy_input_func)(char *,int))
{
  FUNCTION_TRACE;
  CompilerInterface::extension_t e = compiler_p->extension(finput);
  switch(e)
  {
    case CompilerInterface::EXT_SOURCE_CS:
    case CompilerInterface::EXT_SOURCE_CPP:
    case CompilerInterface::EXT_SOURCE_C:
      return c_cs_cpp_analyse(filename_orig,finput,expressions,source,filename,function,option,comp,e,option.getInstrumentationOption(),defines,yy_input_func,compiler_p->errorFormat())==0;
      break;
    case CompilerInterface::EXT_IGNORE:
      FATAL2("Internal error: trying to analyze a source file (%s) to ignore",filename_orig);
      return false;
    default:
      ASSERT(false);
      return false;
  }
  return false;
}

time_t Compiler::modification_time(const char *name)
{
  FUNCTION_TRACE;
  struct stat attr;
  time_t t=0;
  if (stat(name,&attr)==0)
  {
    t=attr.st_mtime;
    DEBUG3("Modification time  %s:%i\n",name,t);
    return t;
  }
  else
  {
    DEBUG2("File size of '%s' cannot be calculated\n",name);
    return 0;
  }
}


bool Compiler::addmeasures(const char  *filename_in,bool import_symbols,bool insert_library,CSMESFile &csmes_file,std::string &errmsg)
{
  FUNCTION_TRACE;

  std::string filename=Source::csmes_filename(filename_in);
  time_t modification_time_csmes = modification_time(filename.c_str());

  DEBUG2("Opening file:%s\n",filename.c_str());
  CSMESFile f;
  if (!f.open(filename.c_str(),CSMESFile::RO))
  {
    DEBUG3("Not an instrumentation file '%s':%s\n",filename.c_str(),f.getLastErrorMsg().c_str());
    return false;
  }
  DEBUG2("file open:%s\n",filename.c_str());
  time_t modification_time_object = modification_time(filename_in);
  if (modification_time_object > modification_time_csmes + 10)
  { // .csmes file is more than 10s older than the object
    FATAL2("File '%s' is ignored (outdated).\n",filename.c_str());
    return false;
  }

  std::set<std::string> added_sections;
  for (int i=0;i<f.nbSections();i++)
  {
	if ( (f.sectionType(i)==CSMESFile::_INSTRUMENTATION_V1) || (f.sectionType(i)==CSMESFile::_INSTRUMENTATION_V2))
    {
      bool ret;
      const char *name=f.sectionModule(i);
      if (added_sections.find(name)!=added_sections.end())
        continue;

      if (insert_library && (f.flags(i)&CSMES_FILE_FLAG_FORCE_DLL_EXPORT) )
        ret = cslib()->appendSource(name,false,f.sectionSignature(i));
      else
        ret = cslib()->appendSource(name,import_symbols,f.sectionSignature(i));
      if (!ret)
      {
        INFO2("File '%s' is compiled/linked twice into the project.\n",name);
        WARNING2("File '%s' may not be correctly instrumented\n",name);
      }
      added_sections.insert(name);
    }
  }
  unsigned long flags=0;
  if ( !insert_library )
  {
     if (compiler_p->forceDllExport())
        flags |= CSMES_FILE_FLAG_FORCE_DLL_EXPORT;
  }
  DEBUG2("Merging with file:%s\n",filename.c_str());
  errmsg.clear();
  bool ret= csmes_file.merge(f,flags,CSMESFile::FULL,errmsg);
  DEBUG3("Merging with file done:%s %s\n",filename.c_str(),errmsg.c_str());
  return ret;
}
