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

#ifndef SOURCE_FILES_H
#define SOURCE_FILES_H
#include <QString>
#include <QStringList>
#include <QList>
#include "sourcefile.h"
#include <QMetaType>

class SourceFiles : public QList<SourceFile>
{
  public:
    SourceFiles(const QList<SourceFile> &s) : QList<SourceFile>(s) {}
    SourceFiles(const SourceFiles &s) : QList<SourceFile>(s) {}
    SourceFiles() : QList<SourceFile>() {}
    QStringList toQStringList() const 
    {
      QStringList l; 
      for (QList<SourceFile>::const_iterator it=begin();it!=end();++it)
        l << *it;
      return l;
    }
};
Q_DECLARE_METATYPE(SourceFiles);

#endif
