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

#include "cmmerge_args.h"
#include <iostream>
#include "getoption.h"
#include "commonglobal.h"
#include "csmesfile.h"
#include <string>
#include <list>

using namespace std;
CMMerge::CMMerge()
{
  clear();
}

CMMerge &CMMerge::instance()
{
  static CMMerge instance;
  return instance;
}


bool CMMerge::getoption_verbose(const char *)
{
  CMMerge::instance()._verbose=true;
  return true;
}

bool CMMerge::getoption_instrumentation_and_execution(const char *filename)
{
  if (filename)
  {
     CMMerge::instance()._policy=CSMESFile::ONLY_INSTRUMENTED_SOURCES;
     CMMerge::instance()._reference_csmes=std::string(filename);
     return true;
  }
  return false;
}

bool CMMerge::getoption_input(const char *filename)
{
  if (filename)
  {
    CMMerge::instance()._input_filenames.push_back(std::string(filename));
    return true;
  }
  return false;
}

bool CMMerge::getoption_output(const char *file)
{
  if (file)
  {
    CMMerge::instance()._output_filename=std::string(file);
    return true;
  }
  return true;
}

bool CMMerge::getoption_append(const char *)
{
  CMMerge::instance()._access=CSMESFile::APPEND_RW;
  return true;
}


void CMMerge::clear()
{
  _access=CSMESFile::NEW_RW;
  _policy=CSMESFile::FULL;
  _verbose=false;
  _output_filename.clear();
  _input_filenames.clear();
  _reference_csmes.clear();
}

bool CMMerge::getopt_process(int argc,const char*const* argv)
{
  char error_msg[16000];
  clear();

  static getoption_t command_line_arg[] = {
    { GETOPT_HELP, NULL, NULL, "cmmerge" , NULL },
    { GETOPT_OPTION, "-a" , "--append" , "Do not create a new CSMes file, but merge into an existing one" , getoption_append},
    { GETOPT_OPTION, "-v" , "--verbose" , "Verbose output" , getoption_verbose},
    { GETOPT_OPTION|GETOPT_HASARG, "-i" , "--instrumentation-and-execution" , "Merge only instrumentations and executions present in the reference file (for importing unittests)" , getoption_instrumentation_and_execution},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MANDATORY, "-o" , "--output" , "Output database" , getoption_output},
    { GETOPT_ARGUMENT|GETOPT_MULTIPLE, NULL , NULL , "CSMes file" , getoption_input},
    { GETOPT_LAST , NULL , NULL , NULL,  NULL }
  };

  if (! getoption_process(command_line_arg,argc,argv,error_msg,sizeof(error_msg)) )
  {
    cerr << "Invalid Command Line Arguments" << endl;
    cerr << error_msg << endl ;
    cerr << getoption_help(argv[0],command_line_arg) << endl;
    return false;
  }
  return true;
}

bool CMMerge::analyse_args(int argc,const char*const * argv)
{
  if (!getopt_process(argc,argv))
    return false;

  CSMESFile output;
  if (!output.open(_output_filename.c_str(),_access))
  {
    printf("Could not open '%s':%s\n",_output_filename.c_str(),output.getLastErrorMsg().c_str());
    return false;
  }

  if (_policy==CSMESFile::ONLY_INSTRUMENTED_SOURCES)
  {
    if (_verbose)
    {
      printf("Loading '%s'...",_reference_csmes.c_str());
      fflush(stdout);
    }
    CSMESFile input;
    if (!input.open(_reference_csmes.c_str(),CSMESFile::RO))
    {
      printf("\nCould not open '%s':%s\n",_reference_csmes.c_str(),input.getLastErrorMsg().c_str());
      return false;
    }
    std::string errmsg;
    if (!output.merge(input,0,CSMESFile::FULL,errmsg))
    {
      printf("\nCould not merge with '%s'\n",_reference_csmes.c_str());
      printf("%s\n",errmsg.c_str());
      return false;
    }
    if (_verbose)
      printf("done\n");
  }

  bool ret=true;
  for (std::list<std::string>::const_iterator itSrc=_input_filenames.begin();itSrc!=_input_filenames.end();++itSrc)
  {
    std::string input_filename=*itSrc;
    if (_verbose)
    {
      printf("Merging with '%s'...",input_filename.c_str());
      fflush(stdout);
    }
    CSMESFile input;
    if (!input.open(input_filename.c_str(),CSMESFile::RO))
    {
      printf("\nCould not open '%s':%s\n",input_filename.c_str(),input.getLastErrorMsg().c_str());
      ret=false;
      continue;
    }
    std::string errmsg;
    if (!output.merge(input,0,_policy,errmsg))
    {
      printf("\nCould not merge with '%s'\n",input_filename.c_str());
      printf("%s\n",errmsg.c_str());
      ret=false;
      continue;
    }
    if (_verbose)
      printf("done\n");
  }
  if (_verbose)
    printf("File '%s' generated.\n",_output_filename.c_str());
  output.close();

  return ret;
}
