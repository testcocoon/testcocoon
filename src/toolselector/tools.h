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

#ifndef TOOLS_H
#define TOOLS_H

#include <qstringlist.h>

class Tools {
  public:
    virtual ~Tools() {}
    virtual QStringList toolList () const =0 ;
    virtual QString uninstall(const QString &,const QString &directory) const=0;
    virtual QString install(const QString &,const QString &directory) const=0;
    virtual QString directory(const QString &) const=0;
    virtual QString directoryLabel(const QString &) const=0;
    virtual bool setConsole(const QString &,const QString &directory) const=0;
    virtual QString createConsole(const QString &item,const QString &directory,const QString &file, bool cmd) const =0;
    virtual void createTutorial(const QString &item,const QString &directory,const QString &file) const =0;
    virtual QString installTool(const QString &item,const QString &directory) const =0;
    virtual QString uninstallTool(const QString &item,const QString &directory) const =0;
    virtual void setDirectory(const QString &directory)=0;

  protected:
#ifdef OS_WIN32
    static QString readRegistry(QString path,QString value);
    static void writeRegistry( QString keyname, QString valuename, QString valueexpr);
#endif
};

#endif
