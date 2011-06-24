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
#include "coveragescanner_main.h"
#include "coveragescanner_main.h"
#include "compiler.h"
#include "version.h"
#include "option.h"
#include "tmpfile.h"
#include "compilerfactory.h"
#include "csmesfile.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(OS_WIN32)
#include <windows.h>
#endif

#include <signal.h>
#if LOG
static void sighandler(int sig)
{
  DEBUG2("\n\nSignal %i received\n",sig);
  MEMORY_REPORT;
  CALLSTACK;
  CLOSE_LOG ();
  exit(-1);
}
#endif

int coveragescanner(int argc,char **argv)
{
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif
#if LOG
  signal(SIGABRT,sighandler);
  signal(SIGTERM,sighandler);
  signal(SIGFPE,sighandler);
  signal(SIGILL,sighandler);
  signal(SIGINT,sighandler);
  signal(SIGSEGV,sighandler);
  signal(SIGTERM,sighandler);
#endif
#ifdef SEFLTEST
  setenv("EF_PROTECT_FREE","1",0);
  setenv("EF_FREE_WIPE","1",0);
#endif
#if 0
  if (strcmp(argv[0],"link.exe")==0)
    DebugBreak();
#endif
#if LOG
  OPEN_LOG();
  DEBUG5("TestCocoon v%i.%i.%i date:%s\n",(TESTCOCOON_VERSION>>16),(TESTCOCOON_VERSION>>8)&0xFF, TESTCOCOON_VERSION&0xFF ,__DATE__);
  std::string cmd;
  for (int i=0;i<argc;i++)
  {
    cmd+=" ";
    cmd+=System::quoteArgument(argv[i]);
  }
  DEBUG2("cmd=%s\n",cmd.c_str());
  for (int ilog=0;ilog<argc;ilog++)
  {
    DEBUG3("Argv[%i]=%s\n",ilog,argv[ilog]);
  }
#endif

  bool coveragescanner_disable_env=CompilerInterface::isCoverageScannerDisabledPerEnvironmentVariable();
  if (coveragescanner_disable_env)
  {
    DEBUG1("COVERAGESCANNER_DISABLE is set\n");
  }

  Option option(argc,argv);
  CompilerInterface::disableCoverageScannerPerEnvironmentVariable();
  if (coveragescanner_disable_env || option.isInactive())
  {
    DEBUG1("Do not instrument source file, call the native tool.\n");
    DEBUG3("call_native_tool(%s,%s)\n",option.profileToolName().c_str(),option.param_args()[0]);
    CompilerInterface  *compiler_p=CompilerFactory::create(option);
    if (compiler_p==NULL)
    {
      FATAL1("Could not find CoverageScanner profile!");
    }
    int ret = compiler_p->callNativeTool();
    TmpFile::object().deleteFiles();
    return ret;
  }
  //DebugBreak();
#if defined(LOG) || !defined(NO_DEBUG)
  fprintf(stderr,"CoverageScanner (\"%s\") is build in debug mode.\n",argv[0]);
#endif

  DEBUG1("Instrument source file, not calling the native tool.\n");
  Compiler compiler(option);


  if (!compiler.generate())
  {
    FATAL2("Error:%s\n",compiler.error());
  }
  MEMORY_REPORT;
  return (0);
}

int coveragescanner_main(int argc,char **argv)
{
  FUNCTION_TRACE;
  try
  {
    EXIT ( coveragescanner(argc,argv) );
  }
  catch (std::string e)
  {
    fprintf(stderr,"Fatal:%s\n",e.c_str());
    EXIT(-1);
  }
}
