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

#ifndef UNDO_EXECUTION_COMMENT_H
#define UNDO_EXECUTION_COMMENT_H
#include <QUndoCommand>
#include <QString>
#include "executionname.h"
class CSMes ;
class WMain ;


class UndoCmdExecutionComment : public QUndoCommand
{
  public:
    UndoCmdExecutionComment (CSMes *csmes_p,WMain *wmain_p,const ExecutionName &name,const QString &old_comment,const QString &new_comment, QUndoCommand *parent);
    virtual void redo();
    virtual void undo();


  private:

    CSMes *m_csmes_p;
    ExecutionName m_name;
    QString m_old_comment;
    QString m_new_comment;
    WMain *m_wmain_p;
};
#endif
