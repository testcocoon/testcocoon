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

#include "cmreportpdef.h"
#include <stdio.h>
#include "cmreport_args.h"
#include <stdlib.h>
#include <QStringList>
#include <QApplication>

static void MessageOutput( QtMsgType type, const char *msg )
{
  switch ( type ) 
  {
#ifndef NO_DEBUG
    case QtDebugMsg:
      fprintf( stderr, "%s\n", msg );
      break;
    case QtSystemMsg:
      fprintf( stderr, "System Message: %s\n", msg );
      break;
    case QtWarningMsg:
      fprintf( stderr, "Warning: %s\n", msg );
      break;
#endif
    case QtFatalMsg:
      fprintf( stderr, "Fatal Error: %s\n", msg );
      abort();
    default:
       break;
  }
}

/*! \brief application startup */
int main (int argc, char **argv)
{
#ifdef __COVERAGESCANNER__
    __coveragescanner_install(argv[0]);
#endif
    qInstallMsgHandler( MessageOutput );

    QApplication    app (argc,argv,QApplication::Tty);
  
    if (!CMReport::instance().analyse_args(app.argc(),app.argv()))
      return -1;

    return 0;
}
