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

#include "intelcompiler.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <stdlib.h>

QStringList IntelCompiler::toolList () const 
{
  QStringList toollist;
#ifdef OS_WIN32

  for (int i1=7;i1<20;i1++)
    for (int i2=0;i2<10;i2++)
    {
      QString envstr=QString::number(i1)+QString::number(i2);
      QString dirname= readRegistry("HKEY_CURRENT_USER\\Software\\Intel\\Compilers\\C++\\"+envstr+"\\IA32", "ProductDir");
      if (dirname!=QString::null && QFile::exists(dirname))
        toollist+=QString("Intel(R) C++ Compiler v%1.%2 for IA-32 applications").arg(i1).arg(i2);
      dirname= readRegistry("HKEY_CURRENT_USER\\Software\\Intel\\Compilers\\C++\\"+envstr+"\\Itanium", "ProductDir");
      if (QFile::exists(dirname))
        toollist+=QString("Intel(R) C++ Compiler v%1.%2 for Itanium(R) applications").arg(i1).arg(i2);
    }
#endif
  return toollist;
}

QString IntelCompiler::uninstall(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString IntelCompiler::install(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString IntelCompiler::directory(const QString &) const
{
  return QString::null;
}

QString IntelCompiler::directoryLabel(const QString &) const
{
  return QString::null;
}

bool IntelCompiler::setConsole(const QString &/*item*/,const QString &/*directory*/) const
{
  return true;
}

void IntelCompiler::createTutorial(const QString &item,const QString &directory,const QString &filename) const 
{
  createConsole(item,directory,filename,false);

  QFile file(filename);
  if ( file.open( QIODevice::Append ) ) 
  {
    QTextStream stream( &file );
    stream << "cls" << "\r\n";
    stream << "@ECHO ON" << "\r\n";
    stream << "csicl --cs-count tutorial.cpp  /nologo /EHsc /Fehello.exe" << "\r\n";
    stream << "@ECHO OFF" << "\r\n";
    stream << "pause" << "\r\n";
    stream << "exit" << "\r\n";
    file.close();
  }
}

QString IntelCompiler::createConsole(const QString &item,const QString &/*directory*/,const QString &filename,bool cmd) const
{
  QFile file(filename);
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\r\n";
    stream << "set PATH=%PATH%;%TESTCOCOON%" << "\r\n";

#ifdef OS_WIN32
    for (int i1=7;i1<20;i1++)
      for (int i2=0;i2<10;i2++)
      {
        QString envstr=QString::number(i1)+QString::number(i2);
        QString dirname=QString::null;
        QString tool=QString("Intel(R) C++ Compiler v%1.%2 for IA-32 applications").arg(i1).arg(i2);
        if (tool==item)
          dirname= readRegistry("HKEY_CURRENT_USER\\Software\\Intel\\Compilers\\C++\\"+envstr+"\\IA32", "ProductDir");
        tool=QString("Intel(R) C++ Compiler v%1.%2 for Itanium(R) applications").arg(i1).arg(i2);
        if (tool==item)
          dirname= readRegistry("HKEY_CURRENT_USER\\Software\\Intel\\Compilers\\C++\\"+envstr+"\\Itanium", "ProductDir");
        if (dirname!=QString::null && QFile::exists(dirname))
          stream << "call \""+dirname+"\\bin\\ICLVars.bat\"" << "\r\n";
      }
#endif

    if (cmd)
      stream << "cmd" << "\r\n";
    file.close();
    return QObject::tr("Build Environment Console created");
  }
  else
    return QObject::tr("Could not write file %1").arg(filename);
}

QString IntelCompiler::installTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

QString IntelCompiler::uninstallTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

