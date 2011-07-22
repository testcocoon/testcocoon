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

#ifndef MODULE_FILES_H
#define MODULE_FILES_H
#include <QString>
#include <QStringList>
#include <QList>
#include "modulefile.h"
#include <QMetaType>

class ModuleFiles : public QList<ModuleFile>
{
  public:
    ModuleFiles(const QList<ModuleFile> &s) : QList<ModuleFile>(s) {}
    ModuleFiles(const ModuleFiles &s) : QList<ModuleFile>(s) {}
    ModuleFiles() : QList<ModuleFile>() {}
    QStringList toQStringList() const 
    {
      QStringList l; 
      for (QList<ModuleFile>::const_iterator it=begin();it!=end();++it)
        l << *it;
      return l;
    }
};
Q_DECLARE_METATYPE(ModuleFiles);

#endif
