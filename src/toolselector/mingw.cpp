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

#include "mingw.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <stdlib.h>

QStringList MinGW::toolList () const 
{
  QStringList toollist;

  toollist+="MinGW - www.mingw.org";

  return toollist;
}

void MinGW::setDirectory(const QString &directory)
{
  directorypath=directory;
}

QString MinGW::uninstall(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString MinGW::install(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString MinGW::directory(const QString &) const
{
  return directorypath;
}

QString MinGW::directoryLabel(const QString &) const
{
  return "MinGW directory";
}

void MinGW::createTutorial(const QString &item,const QString &directory,const QString &filename) const 
{
  createConsole(item,directory,filename,false);

  QFile file(filename);
  if ( file.open( QIODevice::Append ) ) 
  {
    QTextStream stream( &file );
    stream << "cls" << "\r\n";
    stream << "@ECHO ON" << "\r\n";
    stream << "csg++ --cs-count tutorial.cpp -o hello.exe" << "\r\n";
    stream << "@ECHO OFF" << "\r\n";
    stream << "pause" << "\r\n";
    stream << "exit" << "\r\n";
    file.close();
  }

}

bool MinGW::setConsole(const QString &,const QString &directory) const
{
  return QFile::exists(directory+"\\bin\\gcc.exe");
}

QString MinGW::createConsole(const QString &/*item*/,const QString &directory,const QString &filename,bool cmd) const
{
  QFile file(filename);
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\r\n";
    stream << "set PATH=%PATH%;%TESTCOCOON%" << "\r\n";
    stream << "set PATH=%PATH%;"+directory+"\\bin" << "\r\n";

    if (cmd)
      stream << "cmd" << "\r\n";
    file.close();
    return QObject::tr("Build Environment Console created");
  }
  else
    return QObject::tr("Could not write file %1").arg(filename);
}

QString MinGW::installTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

QString MinGW::uninstallTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

