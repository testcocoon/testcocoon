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
#include "option.h"
#include "tmpfile.h"
#include "argv.h"
#include "version.h"
#include "compiler.h"
#include "compilerinterface.h"
#include "compilerfactory.h"
#include "regexp_int.h"
#include "regexp_custom.h"
#include "../commoncode/commonglobal.h"
#include "fileinfo.h"
#include "system.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "filtersource.h"
static Argv stripped_args;

Option::Option (int argc,const char *const*argv)
{
  FUNCTION_TRACE;
  int i;
#if LOG
  TmpFile::object().setDeleteFiles(false);
#else
  TmpFile::object().setDeleteFiles(true);
#endif
  _verbose=false;
  stdin_script=NULL;
  lock_csexe=false;
  libgen_option=NULL;
  architecture_option=NULL;
  default_csexe=STRDUP("coverage");
  setInstrumentException(true);
  setInstrumentAssignment(true);
  coverage_decision=true;
  preprocess_in_background=true;
  partial_condition=true;
  coverage_condition=true;
  coverage_branch=true;
  coverage_mdcs=false;
  force_active_innactive=false;
  coverage_hit=true;
  setResponseFileUsed(false);

  if (argc==0)
  {
    FATAL1 ("No arguments");
    return ;
  }
  if (argc==1)
  {
    print_usage();
    EXIT(0);
  }

  appli_path=CompilerFactory::application_path(argv[0]);
  char file_abs[MAX_PATH];
  if (realPath(appli_path.c_str(),file_abs)!=NULL)
    appli_path=std::string(file_abs);
  
  filterFile.appendDefault(true,FilterSource::FILE_REGULAR_EXPRESSION ,"^<.*>$");
  filterFile.appendDefault(false,FilterSource::PATH,"");
  filterFile.appendDefault(false,FilterSource::PATH,".");
  /* analyze the program name */
  setInactive();
  DEBUG3("find_tool_type (argv0=%s,appli_path=%s)\n",argv[0],appli_path.c_str());
  CompilerFactory::find_tool_type(argv[0],false,appli_path,option_tool_name, profile_inactive);

  /* finding the compiler supported */
  Argv args(argc,argv);
  processArgs(args,stripped_args,NULL);

  // process the COVERAGESCANNER_ARGS variable
  const char *coveragescanner_args=getenv("COVERAGESCANNER_ARGS");
  if (coveragescanner_args)
  {
    Argv args_env;
    envVarToArgList(coveragescanner_args,args_env);
    Argv args_tmp;
    if (processArgs(args_env,args_tmp,NULL) )
    {
    }
  }

  CompilerInterface  *compiler_p=CompilerFactory::create(*this);
  processArgs(args,stripped_args,compiler_p);

  if (compiler_p)
  {
    bool response_file_found=false;
    int max_responsefile_included=100;
    do
    {
      response_file_found=false;
      max_responsefile_included--;
      if (max_responsefile_included<0)
        FATAL1("Maximum number of included response file reached!");

      DEBUG1("Begin processing RSP files\n");
      i=0;
      const char *rsp_file=NULL;
      int arg_index_min;
      int arg_index_max;
      while ((rsp_file=compiler_p->responseFile(i,arg_index_min,arg_index_max)))
      {
        setResponseFileUsed(true);
        Argv resp_args_orig;
        if (compiler_p->responseFileToArgumentList (rsp_file,resp_args_orig))
        {
          Argv resp_args;
          processArgs(resp_args_orig,resp_args,compiler_p);
          {
            /* Merge the arguments together */
            Argv head(stripped_args);
            head.suppress_args(arg_index_min,head.argc());
            Argv middle(resp_args);
            Argv tail(stripped_args);
            tail.suppress_args(0,arg_index_max);
            Argv result;
            result.append(head);
            result.append(middle);
            result.append(tail);
            stripped_args=result;
            response_file_found=true;
            break;
          }
        }
        i++;
      }
      delete compiler_p;
      DEBUG1("End processing RSP files\n");
      compiler_p=CompilerFactory::create(*this);
    }
    while (response_file_found);
  }

  if (compiler_p)
  {
     DEBUG1("Begin processing __COVERAGESCANNER__ define\n");
     // process the __COVERAGESCANNER__ define
     const char *coveragescanner_define=compiler_p->CoverageScannerDefineOptionInArguments();
     if (coveragescanner_define)
     {
        Argv args_define;
        envVarToArgList(coveragescanner_define,args_define);
        Argv args_tmp;
        if (processArgs(args_define,args_tmp,compiler_p) )
        {
        }

        compiler_p->RemoveCoverageScannerDefine(stripped_args);
     }
     delete compiler_p;
     DEBUG1("End processing __COVERAGESCANNER__ define\n");
     compiler_p=CompilerFactory::create(*this);
  }

  if (compiler_p==NULL)
  {
     if (isActive())
     {
        print_usage();
        FATAL1("Illegal compiler specified");
     }
     return ;
  }
  if (compiler_p->deactivateCoverageScanner(stripped_args))
    setInactive();
  else if (!force_active_innactive)
  {
    if (compiler_p->enableCoveragescannerPerDefault())
      setActive();
  }
  FREE(stdin_script);
  stdin_script=NULL;

  if (compiler_p->stdinMriScript(stripped_args))
  { // Read an parse command string
    int alloc=0;
    while (!feof(stdin) && !ferror(stdin))
    {
      char tmp[CHAINE_LEN];
      size_t rd=fread(tmp,1,sizeof(tmp),stdin);
      if (rd>0)
      {
        alloc+=rd;
        stdin_script=(char*)REALLOC(stdin_script,alloc);
        memcpy(&stdin_script[alloc-rd],tmp,rd);
      }
    }
    if (compiler_p->responseFileContainsToArgumentList (stdin_script,args_mri))
    {
      compiler_p->setArgsStdoutMriScript(args_mri);
    }
  }
  profile_tool_name=compiler_p->toolName();
  delete compiler_p;

#if LOG
  if (isActive())
  {
    for (i=0;i<argc;i++)
    {
      DEBUG3("Argv[%i]=%s\n",i,argv[i]);
    }
    for (i=0;i<param_args().argc();i++)
    {
      DEBUG3("Stripped Argv[%i]=%s\n",i,param_args()[i]);
    }
    filterFile.PDEBUG();
    filterFunction.PDEBUG();
  }
#endif
}

Option::~Option()
{
  FUNCTION_TRACE;
  FREE(architecture_option);
  FREE(libgen_option);
  FREE(stdin_script);
}


void Option::envVarToArgList(const char *envvar,Argv &args) const
{
  char *text=(char*)MALLOC(sizeof(char)*(strlen(envvar)+2));
  strcpy(text,envvar);
  strcat(text," "); // add a separator at the end to simplify the process
  /* extract all options */
  int lg_text=strlen(text);
  int pos1=0;
  int pos2=0;
  bool in_string=false;

  for (int i=0;i<lg_text;i++)
  {
    if (text[i]=='"')
      in_string = !in_string;
    if ( !in_string 
        && ( text[i]=='\t' || text[i]==' ' ) ) 
    {
      pos2=i;
      if (pos2!=pos1)
      {
        char *item=(char *)MALLOC(pos2-pos1+1);
        memcpy(item,&text[pos1],pos2-pos1);
        item[pos2-pos1]='\0';

        /* add the item */
        System::strip_quotes(item);
        args.append(item);
        FREE(item);
      }
      pos1=pos2+1;
    }
  }
  FREE(text);
}


void Option::suppress_arg(Argv &args,int &index)
{
  FUNCTION_TRACE;
  args.suppress_arg(index);
  index--;
}
 
bool Option::parseArgOptionWithParam(const char *arg,const char *pattern,std::string &param)
{
  int lg=strlen(pattern);
  if (strncmp(arg,pattern,lg)==0)
  {
    param=std::string(&arg[lg]);
    return true;
  }
  return false;
}

bool Option::processArgs (const Argv &args,Argv &processed_args,const CompilerInterface *compiler_p)
{
  FUNCTION_TRACE;
  int i;
  std::string param;
  bool modifyed = false;
  bool libgen_first_option=true;

  processed_args = args;
  if (args.argc()==0)
    return modifyed;

  /* finding the compiler supported */
  for (i=0;i<processed_args.argc();i++)
  {
    const char *arg;
    arg=processed_args[i];
    int arg_pos=0;
    if ( strlen(arg)>2 )
    {
      if (strncmp("--",arg,2)==0)
        arg_pos=2;
      else if (strncmp("/-",arg,2)==0)
        arg_pos=2;
    }

    if (compiler_p)
    {
      if ( compiler_p->defineMatchCoverageOn( arg ) )
        setActive();
      if ( compiler_p->defineMatchCoverageCondition( arg ) )
        setCoverageCondition();
      if ( compiler_p->defineMatchCoverageLockCsexe( arg ) )
        lock_csexe=true;
      if ( compiler_p->defineMatchCoverageNoLockCsexe( arg ) )
        lock_csexe=false;
      if ( compiler_p->defineMatchCoverageFullInstrumentation( arg ) )
        setPartialCondition(false);
      if ( compiler_p->defineMatchCoveragePartialInstrumentation( arg ) )
        setPartialCondition(true);
      if ( compiler_p->defineMatchCoverageMDCS( arg ) )
        setCoverageMDCS();
      if ( compiler_p->defineMatchCoverageDecition( arg ) )
        setCoverageDecision();
      if ( compiler_p->defineMatchCoverageBranch( arg ) )
        setCoverageBranch();
      if ( compiler_p->defineMatchCoverageHit( arg ) )
        setCoverageHit();
      if ( compiler_p->defineMatchCoverageCount( arg ) )
        setCoverageCount();
      if ( compiler_p->defineMatchCoverageNoExceptions( arg ) )
        setInstrumentException(false);
      if ( compiler_p->defineMatchCoverageNoAssignment( arg ) )
        setInstrumentException(false);
    }
    if (arg_pos==0)
      filterFile.appendDefault(false,FilterSource::FILENAME,arg);
    else if ( (strcmp(&arg[arg_pos],"cs-help")==0) )
    {
      print_usage();
      EXIT(0);
    }
    else if ( (strcmp(&arg[arg_pos],"cs-condition")==0) )
    {
      setCoverageCondition();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-verbose")==0) )
    {
       _verbose=true;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-nopipe")==0) )
    {
       preprocess_in_background=false;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-pipe")==0) )
    {
       preprocess_in_background=true;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-lock-csexe")==0) )
    {
      lock_csexe=true;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-nolock-csexe")==0) )
    {
      lock_csexe=false;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-keep-instrumentation-files")==0) )
    {
      TmpFile::object().setDeleteFiles(false);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-full-instrumentation")==0) )
    {
      setPartialCondition(false);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-partial-instrumentation")==0) )
    {
      setPartialCondition(true);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-mdcs")==0) )
    {
      setCoverageMDCS();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-decision")==0) )
    {
      setCoverageDecision();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-branch")==0) )
    {
      setCoverageBranch();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-off")==0) )
    {
      setInactive();
      force_active_innactive=true;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-hit")==0) )
    {
      setCoverageHit();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-count")==0) )
    {
      setCoverageCount();
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-on")==0) )
    {
      setActive();
      force_active_innactive=true;
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-qt4")==0) )
    {
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::tr$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_check_for_QOBJECT_macro$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::metaObject$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_metacast$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::q_func$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::trUtf8$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_metacall$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,"^qt_plugin_query_verification_data$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,"^qt_plugin_instance$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,"^QMetaTypeId::qt_metatype_id$");

      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-no-assignments")==0) )
    {
      setInstrumentAssignment(false);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-no-exceptions")==0) )
    {
      setInstrumentException(false);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( (strcmp(&arg[arg_pos],"cs-qt3")==0) )
    {
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::metaObject$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::className$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_cast$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_invoke$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_property$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_emit$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qt_static_property$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::staticMetaObject$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::tr$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::qObject$");
      filterFunction.appendDefault(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,".*::trUtf8$");

      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-architecture=",param) )
    {
      FREE(architecture_option);
      architecture_option=STRDUP(param.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-libgen=",param) )
    {
      if (libgen_first_option)
      {
        FREE(libgen_option);
        libgen_option=NULL;
        libgen_first_option=false;
      }

      std::string argument=System::quoteArgument(param);
      if (libgen_option)
      {
        int lg_libgen_option=0;
        lg_libgen_option=strlen(libgen_option);
        lg_libgen_option+=strlen(argument.c_str())+2;
        libgen_option=(char*)REALLOC(libgen_option,lg_libgen_option);
        strcat(libgen_option," ");
        strcat(libgen_option,argument.c_str());
      }
      else
      {
        libgen_option=STRDUP(argument.c_str());
      }

      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-compiler=",param) )
    {
      setInactive();
      DEBUG3("find_tool_type (argv0=%s,appli_path=%s)\n",param.c_str(),appli_path.c_str());
      CompilerFactory::find_tool_type(param.c_str(),true,appli_path,option_tool_name, profile_inactive);
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-function-regex=",param) )
    {
      std::string errmsg=filterFunction.append(false,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-function-regex=",param) )
    {
      std::string errmsg=filterFunction.append(true,FilterFunctions::FUNCTION_REGULAR_EXPRESSION,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-regex=",param) )
    {
      FATAL1("--cs-include-regex is obselete, use --cs-include-file-regex");
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-output=",param) )
    {
      FREE(default_csexe);
      default_csexe=STRDUP(param.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-regex=",param) )
    {
      FATAL1("--cs-exclude-regex is obselete, use --cs-exclude-file-regex");
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-wildcard=",param) )
    {
      FATAL1("--cs-include-wildcard is obselete, use --cs-include-file-wildcard");
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-wildcard=",param) )
    {
      FATAL1("--cs-exclude-wildcard is obselete, use --cs-exclude-file-wildcard");
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-function-wildcard=",param) )
    {
      std::string errmsg=filterFunction.append(false,FilterFunctions::FUNCTION_WILDCARD,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-function-wildcard=",param) )
    {
      std::string errmsg=filterFunction.append(true,FilterFunctions::FUNCTION_WILDCARD,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-file-abs-regex=",param) )
    {
      std::string errmsg=filterFile.append(false,FilterSource::FILE_REGULAR_EXPRESSION_ABS,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-file-abs-regex=",param) )
    {
      std::string errmsg=filterFile.append(true,FilterSource::FILE_REGULAR_EXPRESSION_ABS,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-file-abs-wildcard=",param) )
    {
      std::string errmsg=filterFile.append(false,FilterSource::FILE_WILDCARD_ABS,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-file-abs-wildcard=",param) )
    {
      std::string errmsg=filterFile.append(true,FilterSource::FILE_WILDCARD_ABS,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-file-regex=",param) )
    {
      std::string errmsg=filterFile.append(false,FilterSource::FILE_REGULAR_EXPRESSION,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-file-regex=",param) )
    {
      std::string errmsg=filterFile.append(true,FilterSource::FILE_REGULAR_EXPRESSION,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-file-wildcard=",param) )
    {
      std::string errmsg=filterFile.append(false,FilterSource::FILE_WILDCARD,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-file-wildcard=",param) )
    {
      std::string errmsg=filterFile.append(true,FilterSource::FILE_WILDCARD,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-include-path=",param) )
    {
      std::string errmsg=filterFile.append(false,FilterSource::PATH,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
    else if ( parseArgOptionWithParam(&arg[arg_pos],"cs-exclude-path=",param) )
    {
      std::string errmsg=filterFile.append(true,FilterSource::PATH,param.c_str());
      if (!errmsg.empty())
        FATAL3("Error in command line argument (%s):%s\n",arg,errmsg.c_str());
      suppress_arg(processed_args,i);
      modifyed=true;
    }
  }
  return modifyed;
}

const Argv& Option::param_args() 
{
  FUNCTION_TRACE;
  return stripped_args;
}

void print_usage()
{
  FUNCTION_TRACE;
  printf("CoverageScanner (C) see AUTHORS file\n");
  printf("Usage:\n");
  printf("--cs-branch: code coverage branch support\n");
  printf("--cs-decision: code coverage decision support\n");
  printf("--cs-condition: code coverage condition support\n");
  //printf("--cs-mdcs: code coverage MD/CS support\n");
  printf("--cs-full-instrumentation: complete instrumentation of conditions\n");
  printf("--cs-partial-instrumentation: partial instrumentation of conditions (default)\n");
  printf("--cs-on: activation of CoverageScanner\n");
  printf("--cs-off: deactivation of CoverageScanner\n");
  printf("--cs-help: this help\n");
  printf("--cs-compiler=<string>: compiler used (g++,gcc,cl)\n");
  printf("--cs-libgen=<string>: additional linker options to generate the CoverageScanner library\n");
  printf("--cs-no-exceptions: do not instrument exceptions (C++/C# catch block)\n");
  printf("--cs-qt4: ignore symbols generated by Qt4 library\n");
  printf("--cs-qt3: ignore symbols generated by Qt3 library\n");
  printf("--cs-verbose: verbose output\n");
  printf("--cs-include-function-regex=<string>: include function using regular expression\n");
  printf("--cs-exclude-function-regex=<string>: exclude function using regular expression\n");
  printf("--cs-include-file-regex=<string>: include file using regular expression\n");
  printf("--cs-exclude-file-regex=<string>: exclude file using regular expression\n");
  printf("--cs-include-file-abs-regex=<string>: include file using regular expression on the absolute path\n");
  printf("--cs-exclude-file-abs-regex=<string>: exclude file using regular expression on the absolute path\n");
  printf("--cs-include-function-wildcard=<string>: include function using wildcards\n");
  printf("--cs-exclude-function-wildcard=<string>: exclude function using wildcards\n");
  printf("--cs-include-file-wildcard=<string>: include files using wildcards\n");
  printf("--cs-exclude-file-wildcard=<string>: exclude files using wildcards\n");
  printf("--cs-include-file-abs-wildcard=<string>: include files using wildcards on the absolute file path\n");
  printf("--cs-exclude-file-abs-wildcard=<string>: exclude files using wildcards on the absolute file path\n");
  printf("--cs-include-path=<string>: include files in a specific path\n");
  printf("--cs-exclude-path=<string>: exclude files in a specific path\n");
  printf("--cs-output=<string>: name of the .csexe file\n");
  printf("--cs-lock-csexe: CoverageScanner library creates a lock file before generating a coverage report\n");
  printf("--cs-nolock-csexe: iusage of lock files are disabled (default)\n");
  printf("--cs-pipe: use a pipe to exchange data between the preprocessor and CoverageScanner\n");
  printf("--cs-nopipe: use a file to exchange data between the preprocessor and CoverageScanner\n");
  printf("--cs-keep-instrumentation-files: do not delete temporary instrumentation source files.\n");
  printf("\n");
  printf("Version: %i.%i.%i\n",
      (TESTCOCOON_VERSION>>16) & 0xFF,
      (TESTCOCOON_VERSION>>8) & 0xFF,
      (TESTCOCOON_VERSION) & 0xFF);
  printf("Date:%s\n",__DATE__ );
}

void Option::operator=(const Option &src)
{
  FUNCTION_TRACE;
  appli_path=src.appli_path;

  option_tool_name=src.option_tool_name;
}


instrumentation_option_t Option::getInstrumentationOption() const 
{
  FUNCTION_TRACE;
  instrumentation_option_t option;
  memset(&option,0,sizeof(option));

  if (isCoverageMDCS())
    option.coverage_mdcs=1;
  if (isCoverageBranch())
    option.coverage_branch=1;
  if (isCoverageCondition())
    option.coverage_condition=1;
  if (isCoverageDecision())
    option.coverage_decision=1;
  if (isPartialCondition())
    option.partial_condition=1;

  option.instrument_exception=instrumentExceptions();
  option.instrument_assignment=instrumentAssignments();
  if (isCoverageCount())
  {
    option.coverage_count=1;
    option.coverage_enable=1;
  }
  if (isCoverageHit())
  {
    option.coverage_hit=1;
    option.coverage_enable=1;
  }

  return option;
}
