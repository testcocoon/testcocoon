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

#include <qapplication.h>
#include <stdio.h>
#include <qwidget.h>
#include "WMainUI.h"
#include <qobject.h>
#include <qimage.h>
#include <qpixmap.h>

static void MessageOutput( QtMsgType type, const char *msg )
{
  switch ( type ) 
  {
    case QtDebugMsg:
#ifndef NO_DEBUG
      fprintf( stderr, "%s\n", msg );
#endif
      break;
    case QtWarningMsg:
#ifndef NO_DEBUG
      fprintf( stderr, "Warning: %s\n", msg );
#endif
      break;
    case QtFatalMsg:
      fprintf( stderr, "Fatal: %s\n", msg );
      Q_ASSERT(true);
    default:
      break;
  }
}

/*! \brief application startup */
int
main (int argc,
      char **argv)
{
    int             result;
#ifdef __COVERAGESCANNER__
    __coveragescanner_install(argv[0]);
#endif
    qInstallMsgHandler( MessageOutput );

    QApplication::setColorSpec (QApplication::NormalColor);
    QApplication    app (argc,argv);
    QApplication::addLibraryPath(QCoreApplication::applicationDirPath());

    WMainUI          *mainwindow_p = new WMainUI ();
    mainwindow_p->show ();

    app.connect (&app, SIGNAL (lastWindowClosed ()), &app, SLOT (quit ()));
    result = app.exec ();

    return result;
}
