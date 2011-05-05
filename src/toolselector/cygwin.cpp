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

#include "cygwin.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <stdlib.h>

QStringList CygWin::toolList () const 
{
  QStringList toollist;

  toollist+="CygWin - www.cygwin.com";

  return toollist;
}

void CygWin::setDirectory(const QString &directory)
{
  directorypath=directory;
}

QString CygWin::install(const QString &,const QString &directory) const
{
  if (!QFile::exists(directory+"\\cygwin.bat"))
    return QString::null;
  QFile config(directory+"\\cygwin.bat");
  if (config.open(QIODevice::ReadWrite))
  {
    QString data=config.readAll();
    bool installed= data.contains("@set PATH=%PATH%;%TESTCOCOON%") ;
    config.close();
    if (installed)
      return QString::null;
    else
      return QObject::tr("Install CygWin Support");
  }
  return QString::null;
}

QString CygWin::uninstall(const QString &,const QString &directory) const
{
  if (!QFile::exists(directory+"\\cygwin.bat"))
    return QString::null;
  QFile config(directory+"\\cygwin.bat");
  if (config.open(QIODevice::ReadWrite))
  {
    QString data=config.readAll();
    bool installed= data.contains("@set PATH=%PATH%;%TESTCOCOON%") ;
    config.close();
    if (!installed)
      return QString::null;
    else
      return QObject::tr("Remove CygWin Support");
  }
  return QString::null;
}

QString CygWin::directory(const QString &) const
{
  return directorypath;
}

QString CygWin::directoryLabel(const QString &) const
{
  return "CygWin directory";
}

bool CygWin::setConsole(const QString &/*item*/,const QString &/*directory*/) const
{
  return false;
}

void CygWin::createTutorial(const QString &item,const QString &directory,const QString &filename) const 
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

QString CygWin::createConsole(const QString &/*item*/,const QString &directory,const QString &filename,bool cmd) const
{
  QFile file(filename);
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\r\n";
    stream << "@set PATH=%PATH%;%TESTCOCOON%" << "\r\n";
    stream << "set PATH=%PATH%;"+directory+"\\bin" << "\r\n";

    if (cmd)
      stream << "cmd" << "\r\n";
    file.close();
    return QObject::tr("Build Environment Console created");
  }
  else
    return QObject::tr("Could not write file %1").arg(filename);
}

QString CygWin::installTool(const QString &/*item*/,const QString &directory) const 
{
  QFile config(directory+"\\cygwin.bat");
  if (config.open(QIODevice::ReadOnly))
  {
    QString data=config.readAll();
    config.close();
    if (config.open(QIODevice::WriteOnly))
    {
      QTextStream stream( &config );
      stream << "@set PATH=%PATH%;%TESTCOCOON%" << "\r\n";
      stream << data;
      config.close();

      return QObject::tr("<QT><TT>csg++</TT> and <TT>csgcc</TT> are now available under the Cygwin environment.<QT>");
    }
  }
  return QString::null;
}

QString CygWin::uninstallTool(const QString &/*item*/,const QString &directory) const 
{
  QFile config(directory+"\\cygwin.bat");
  if (config.open(QIODevice::ReadOnly))
  {
    QString data=config.readAll();
    QString pattern="@set PATH=%PATH%;%TESTCOCOON%\r\n";
    int pos= data.indexOf(pattern,0) ;
    data=data.left(pos)+data.mid(pos+pattern.length());
    config.close();
    if (config.open(QIODevice::WriteOnly))
    {
      QTextStream stream( &config );
      stream << data;
      config.close();

      return QString::null;
    }
  }
  return QString::null;
}

