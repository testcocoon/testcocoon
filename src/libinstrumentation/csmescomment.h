/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#ifndef CSMES_COMMENT_H
#define CSMES_COMMENT_H
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <QList>
#include "csmesexecution.h"
class CSMesComment : public CSMesExecution
{
  protected:
    CSMesComment();
  protected:
    void clear();
    bool setComment(QString module,QString source,int instrument_index,const QString &comment);
    QString getComment(QString module,QString source,int instrument_index) const;
  protected:
    typedef QHash<int,QString> comments_t;
    typedef QHash<QString,comments_t> sources_comments_t;
    typedef QHash<QString,sources_comments_t> modules_comments_t;
    modules_comments_t comments;
  protected:
    QList<int> commentedIndexes(QString mod,QString src) const;
};

#endif
