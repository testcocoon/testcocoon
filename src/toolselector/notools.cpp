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

#include "notools.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <stdlib.h>

QStringList NoTools::toolList () const 
{
  QStringList toollist;
  toollist+="None";

  return toollist;
}

QString NoTools::uninstall(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString NoTools::install(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString NoTools::directory(const QString &) const
{
  return QString::null;
}

QString NoTools::directoryLabel(const QString &) const
{
  return QString::null;
}

void NoTools::createTutorial(const QString &item,const QString &directory,const QString &filename) const 
{
  createConsole(item,directory,filename,false);

  QFile file(filename);
  if ( file.open( QIODevice::Append ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\r\n" ; 
    stream << "cls" << "\r\n" ; 
    stream << "color d0" << "\r\n" ; 
    stream << "echo Error: no compiler selected." << "\r\n" ; 
    stream << "echo Proceed as follows:" << "\r\n" ; 
    stream << "echo   1) Lauch the \"Build Environment Selector\"" << "\r\n" ; 
    stream << "echo   2) Select a tool suite" << "\r\n" ; 
    stream << "echo   3) Click on \"Create Build Environment Console\"" << "\r\n" ; 
    stream << "pause" << "\r\n" ; 
    stream << "exit" << "\r\n" ; 
    file.close();
  }
}

bool NoTools::setConsole(const QString &/*item*/,const QString &/*directory*/) const
{
  return true;
}

QString NoTools::createConsole(const QString &/*item*/,const QString &/*directory*/,const QString &filename, bool cmd) const
{
  QFile file(filename);
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\n";
    stream << "set PATH=%PATH%;%TESTCOCOON%" << "\n";

    if (cmd)
      stream << "cmd" << "\n";
    file.close();
    return QObject::tr("Build Environment Console created");
  }
  else
    return QObject::tr("Could not write file %1").arg(filename);
}

QString NoTools::installTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

QString NoTools::uninstallTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

