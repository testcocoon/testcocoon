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

#include "cmcsexeimport_args.h"
#include <qmessagebox.h>
#include <QFile>
#include <QTextDocument>
#include <iostream>
#include "getoption.h"
using namespace std;
CMCsExeImport::CMCsExeImport()
{
  clear();
}

CMCsExeImport &CMCsExeImport::instance()
{
  static CMCsExeImport instance;
  return instance;
}

void CMCsExeImport::clear()
{
  csmes.clear();
  csexe_filename=QString();
  csmes_filename=QString();
  title=QString();
  policy=CSMes::CSEXE_POLICY_IGNORE_DUPLICATES;
  default_execution_status=Executions::EXECUTION_STATUS_UNKNOWN;
}

bool CMCsExeImport::getoption_passed(const char *)
{
  CMCsExeImport::instance().default_execution_status=Executions::EXECUTION_STATUS_PASSED;
  return true;
}

bool CMCsExeImport::getoption_failed(const char *)
{
  CMCsExeImport::instance().default_execution_status=Executions::EXECUTION_STATUS_FAILED;
  return true;
}


bool CMCsExeImport::getoption_check_manually(const char *)
{
  CMCsExeImport::instance().default_execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
  return true;
}

bool CMCsExeImport::getoption_csmes_filename(const char *file)
{
  CMCsExeImport::instance().csmes_filename=QString(file);
  bool ret = CMCsExeImport::instance().csmes.loadCSMes(CMCsExeImport::instance().csmes_filename);
  if (!ret)
  {
    cerr 
      << QObject::tr("Error: could not open csmes file").toStdString() 
      << " '" << file << "':" 
      << CMCsExeImport::instance().csmes.error().toStdString() 
      << endl;
  }
  return ret;
}

bool CMCsExeImport::getoption_title(const char *file)
{
  CMCsExeImport::instance().title=QString(file);
  return true;
}

bool CMCsExeImport::getoption_csexe_filename(const char *file)
{
  CMCsExeImport::instance().csexe_filename=QString(file);
  return true;
}

static CSMes::csexe_import_policy_t StringToPolicy(const QString &str)
{
  if (str=="import_duplicates")
    return CSMes::CSEXE_POLICY_IMPORT_DUPLICATES;
  else if (str=="ignore_duplicates")
    return CSMes::CSEXE_POLICY_IGNORE_DUPLICATES;
  else if (str=="import_duplicates_and_empty")
    return CSMes::CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY;
  else if (str=="merge")
    return CSMes::CSEXE_POLICY_MERGE_ALL_TOGETHER;
  else
    return CSMes::CSEXE_POLICY_INVALID;
}

bool CMCsExeImport::getoption_import_policy(const char *p)
{
  CMCsExeImport::instance().policy=StringToPolicy(QString(p));
  return CMCsExeImport::instance().policy!=CSMes::CSEXE_POLICY_INVALID;
}

bool CMCsExeImport::getopt_process(int argc,const char*const* argv)
{
  char error_msg[16000];
  clear();

  static getoption_t command_line_arg[] = {
    { GETOPT_HELP, NULL, NULL, "cmcsexeimport" , NULL },
    { GETOPT_OPTION, "-P" , "--passed" , "Mark all imported tests as passed" , getoption_passed},
    { GETOPT_OPTION, "-F" , "--failed" , "Mark all imported tests as failed" , getoption_failed},
    { GETOPT_OPTION, "-C" , "--check-manually" , "Mark all imported tests as to be checked manually" , getoption_check_manually},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MANDATORY, "-m" , "--csmes" , "CSMes file name" , getoption_csmes_filename},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MANDATORY, "-e" , "--csexe" , "CSExe file name" , getoption_csexe_filename},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MANDATORY, "-t" , "--title" , "Execution title" , getoption_title},
    { GETOPT_OPTION|GETOPT_HASARG, "-p" , "--policy" , "Import policy (ignore_duplicates, import_duplicates_and_empty, import_duplicates or merge)" , getoption_import_policy},
    { GETOPT_ARGUMENT, NULL , NULL , NULL , NULL},
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


bool CMCsExeImport::analyse_args(int argc,const char* const* argv)
{
  QString tmp;
  if (!getopt_process(argc,argv))
    return false;

  QString short_status;
  QString info;
  QStringList new_executions;
  QFile f(csexe_filename);
  QString err;
  if (!csmes.loadCSExe(f,title,policy,default_execution_status,new_executions,info,short_status,err,NULL,NULL))
  {
    cerr << QObject::tr("Error: could not load execution.").toStdString() << endl;
    return false;
  }

  if (!csmes.saveCSMes())
  {
    cerr << QObject::tr("Error: could not save csmes file.").toStdString() << endl;
    return false;
  }

  if (!err.isEmpty())
    cerr << err.toStdString() << endl;

  return true;
}
