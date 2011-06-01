/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#include "coveragebrowser_args.h"
#include "csmesfile.h"
#include "options.h"
#include <qstring.h>
#include <qmessagebox.h>
#include "getoption.h"
static QString csexe_filename=QString();
static QString csmes_filename=QString();
static QString error_message=QString();

static bool getoption_csmes_filename(const char *file)
{
  if (!csmes_filename.isEmpty())
  {
    error_message= QObject::tr("No CSMes file already defined.");
    return false;
  }
  if (!CSMESFile::isCSMESFile(file))
  {
    error_message= QObject::tr("Invalid CSMes file:")+QString(file);
    return false;
  }
  else
  {
    csmes_filename=QString(file);
    Options::set_opt(QString(),"CSMES_FILENAME",QString(file));
    return true;
  }
  return false;
}

static bool getoption_csexe_filename(const char *file)
{
  csexe_filename=QString(file);
  return true;
}

QString analyse_coveragebrowser_args(int argc,const char* const* argv)
{
  csexe_filename.clear();
  csmes_filename.clear();
  error_message.clear();
  QString tmp;
  char error_msg[16000];
  error_msg[0]='\0';

  static getoption_t command_line_arg[] = {
    { GETOPT_HELP                 , NULL , NULL      , "CoverageBrowser" , NULL }                    , 
    { GETOPT_OPTION|GETOPT_HASARG , "-m" , "--csmes" , "CSMes file name" , getoption_csmes_filename} , 
    { GETOPT_OPTION|GETOPT_HASARG , "-e" , "--csexe" , "CSExe file name" , getoption_csexe_filename} , 
    { GETOPT_ARGUMENT             , NULL , NULL      , "CSMes file name" , getoption_csmes_filename} , 
    { GETOPT_LAST                 , NULL , NULL      , NULL              , NULL }
  };


  if (!getoption_process(command_line_arg,argc,argv,error_msg,sizeof(error_msg)))
    return QString(error_msg)+"\n"+QString(getoption_help(argv[0],command_line_arg));
  if (csmes_filename.isEmpty() && !csexe_filename.isEmpty())
    return QObject::tr("No CSMes file specified.");
  return error_message;
}

QString arg_csexe_filename()
{
  return csexe_filename;
}

