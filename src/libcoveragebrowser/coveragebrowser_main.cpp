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

#include "coveragebrowserpdef.h"
#include "coveragebrowser_main.h"
#include <qapplication.h>
#include <stdio.h>
#include <qwidget.h>
#include "WMain.h"
#include "Service.h"
#include "settingscheck.h"
#include <qobject.h>
#include <qimage.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include "options.h"
#include "coveragebrowser_args.h"
#include "version.h"
#include <qdir.h>
#include <qbitmap.h>
#include <QCleanlooksStyle>
#include <QMessageBox>
#include <QtDebug>
#include <QTranslator>
#include <QErrorMessage>
#include <QAbstractButton>

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
int coveragebrowser_main (int argc, char **argv)
{
  int             result=-1;
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif
  qInstallMsgHandler( MessageOutput );
#ifndef NO_DEBUG
  { // Forcing loading debug functions
    QString s;
    QDebug d(&s);
    d << "CoverageBrowser:" << TESTCOCOON_VERSION << "("<<__DATE__<<")";
  }
#endif

  QApplication::setColorSpec (QApplication::NormalColor);
  QApplication    app (argc,argv);
  QApplication::addLibraryPath(QCoreApplication::applicationDirPath());

  Options::read_options ();
  setTabSize(Options::get_opt_long(QString(),QString("TAB_SIZE")));
  QTranslator translator;
  QString translationFile=QCoreApplication::applicationDirPath()+"/"+Options::get_opt_str(QString(),"LANGUAGE",DEF_LANGUAGE)+".qm";
  if (translator.load(translationFile))
  {
    app.installTranslator(&translator);   
  }

  QString error_msg=analyse_coveragebrowser_args(app.argc(),app.argv());
  if ( error_msg.isEmpty() )
  {
    SettingsCheck::object().check();

    WMain *mainwindow_p = new WMain ;
    mainwindow_p->show ();

    app.connect (&app, SIGNAL (lastWindowClosed ()), &app, SLOT (quit ()));
    result = app.exec ();

    if (!Options::get_opt_bool(QString(),"RELOAD_PROJECT"))
      Options::set_opt(QString(),"CSMES_FILENAME",QString::null);

    delete mainwindow_p;
    Options::set_opt(QString(),"TESTCOCOON_VERSION",TESTCOCOON_VERSION);
    Options::save_options ();
  }
  else
  {
    QMessageBox::critical(NULL,QObject::tr("Invalid Command Line Arguments"),
        error_msg,
        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    return -1;
  }
  return result;
}
