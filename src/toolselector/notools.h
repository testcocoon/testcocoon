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

#ifndef NO_TOOLS_H
#define NO_TOOLS_H
#include "tools.h"

class NoTools : public Tools {
  public:
    virtual ~NoTools() {}
    virtual QStringList toolList () const ;
    virtual QString uninstall(const QString &,const QString &directory) const;
    virtual QString install(const QString &,const QString &directory) const;
    virtual QString directory(const QString &) const;
    virtual QString directoryLabel(const QString &) const;
    virtual bool setConsole(const QString &,const QString &directory) const;
    virtual QString createConsole(const QString &item,const QString &directory,const QString &file,bool cmd) const;
    virtual void createTutorial(const QString &item,const QString &directory,const QString &file) const ;
    virtual QString installTool(const QString &item,const QString &directory) const ;
    virtual QString uninstallTool(const QString &item,const QString &directory) const ;
    virtual void setDirectory(const QString &/*directory*/) {} 
};

#endif
