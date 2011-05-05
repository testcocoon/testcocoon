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

#include "visualstudio.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <stdlib.h>

QStringList VisualStudio::toolList () const 
{
  QStringList toollist;
  char*  env=NULL;

  if ( (env=getenv("VS71COMNTOOLS")) )
    toollist+="Microsoft(R) Visual Studio(R) .NET 2003";
  if ( (env=getenv("VS80COMNTOOLS")) )
    toollist+="Microsoft(R) Visual Studio(R) 2005";
  if ( (env=getenv("VS100COMNTOOLS")) )
    toollist+="Microsoft(R) Visual Studio(R) 2010";
  if ( (env=getenv("VS90COMNTOOLS")) )
    toollist+="Microsoft(R) Visual Studio(R) 2008";
  if ( (env=getenv("VSCOMNTOOLS")) )
    toollist+="Microsoft(R) Visual Studio(R) .NET 2002";
  if ( (env=getenv("VCToolkitInstallDir")) )
    toollist+="Microsoft(R) Visual C++ ToolKit 2003";
  if ( (env=getenv("MSDevDir")) )
    toollist+="Microsoft(R) Visual Studio(R) 6.0";
  return toollist;
}

QString VisualStudio::uninstall(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString VisualStudio::install(const QString &/*item*/,const QString &/*directory*/) const
{
  return QString::null;
}

QString VisualStudio::directory(const QString &) const
{
  return QString::null;
}

QString VisualStudio::directoryLabel(const QString &) const
{
  return QString::null;
}

void VisualStudio::createTutorial(const QString &item,const QString &directory,const QString &filename) const 
{
  createConsole(item,directory,filename,false);

  QFile file(filename);
  if ( file.open( QIODevice::Append ) ) 
  {
    QTextStream stream( &file );
    stream << "cls" << "\r\n";
    stream << "@ECHO ON" << "\r\n";
    stream << "cscl --cs-count tutorial.cpp  /nologo /EHsc /Fehello.exe" << "\r\n";
    stream << "@ECHO OFF" << "\r\n";
    stream << "pause" << "\r\n";
    stream << "exit" << "\r\n";
    file.close();
  }
}

bool VisualStudio::setConsole(const QString &,const QString &/*directory*/) const
{
  return true;
}

QString VisualStudio::createConsole(const QString &item,const QString &/*directory*/,const QString &filename,bool cmd) const
{
  QFile file(filename);
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream stream( &file );
    stream << "@echo OFF" << "\r\n";
    stream << "set PATH=%PATH%;%TESTCOCOON%" << "\r\n";

    if (item=="Microsoft(R) Visual Studio(R) 2005")
      stream << "call \"%VS80COMNTOOLS%\\..\\..\\VC\\vcvarsall.bat\" x86" << "\r\n";
    else if (item=="Microsoft(R) Visual Studio(R) 2008")
      stream << "call \"%VS90COMNTOOLS%\\..\\..\\VC\\vcvarsall.bat\" x86" << "\r\n";
    else if (item=="Microsoft(R) Visual Studio(R) 2010")
      stream << "call \"%VS100COMNTOOLS%\\..\\..\\VC\\vcvarsall.bat\" x86" << "\r\n";
    else if (item=="Microsoft(R) Visual Studio(R) .NET 2003")
      stream << "call \"%VS71COMNTOOLS%\\vsvars32.bat\"" << "\r\n";
    else if (item=="Microsoft(R) Visual Studio(R) .NET 2002")
      stream << "call %VSCOMNTOOLS%\\vsvars32.bat" << "\r\n";
    else if (item=="Microsoft(R) Visual C++ ToolKit 2003")
      stream << "call \"%VCToolkitInstallDir%\\vcvars32.bat\"" << "\r\n";
    else if (item=="Microsoft(R) Visual Studio(R) 6.0")
      stream << "call \"%MSDevDir%\\..\\..\\vc98\\bin\\vcvars32.bat\"" << "\r\n";

    if (cmd)
      stream << "cmd" << "\r\n";
    file.close();
    return QObject::tr("Build Environment Console created");
  }
  else
    return QObject::tr("Could not write file %1").arg(filename);
}

QString VisualStudio::installTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

QString VisualStudio::uninstallTool(const QString &/*item*/,const QString &/*directory*/) const 
{
  return QString::null;
}

