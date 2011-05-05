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

#include "helpclient.h"
#include "coveragebrowserpdef.h"
#include <QDesktopServices>
#include <QUrl>
#include <qmessagebox.h>
#include <qfile.h>
#include <qapplication.h>
#include <qmap.h>
#include <qstring.h>
#include <QFileInfo>


void HelpClient::OpenTestCocoonWebsite()
{
  QDesktopServices::openUrl (QUrl("http://www.testcocoon.org")) ;
}

void HelpClient::OpenPDFHelp()
{
  QUrl url;
  QFileInfo f;
  if (QFile::exists(QString(HELP_PATH)+QString(PDF_HELP)))
    f=QFileInfo ((QString(HELP_PATH)+QString(PDF_HELP))) ;
  else if (QFile::exists(QString(PDF_HELP)))
    f=QFileInfo ((QString(PDF_HELP))) ;
  else if (QFile::exists(qApp->applicationDirPath()+QString(PDF_HELP)))
    f=QFileInfo ((qApp->applicationDirPath()+QString(PDF_HELP))) ;
#if defined(OS_LINUX) || defined(OS_MACX)
  else if (QFile::exists("/opt/TestCocoon/testcocoon.pdf"))
    f=QFileInfo ("/opt/TestCocoon/testcocoon.pdf") ;
#endif
  else
  {
    QMessageBox::warning(NULL,QObject::tr("Error"),QObject::tr("Could not display Help"),QObject::tr("&Close"));
    return;
  }
  url=QUrl::fromLocalFile (f.absoluteFilePath()) ;
  QDesktopServices::openUrl (url) ;
}
