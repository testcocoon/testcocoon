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

#ifndef UNDO_EXECUTION_STATUS_H
#define UNDO_EXECUTION_STATUS_H
#include <QUndoCommand>
#include <QString>
#include "executions.h"
class CSMes ;


class UndoCmdExecutionStatus : public QUndoCommand
{
  public:
    UndoCmdExecutionStatus (CSMes *csmes_p,const QString &execution_name, Executions::execution_status_t e, QUndoCommand *parent);
    virtual ~UndoCmdExecutionStatus ();
    virtual void redo();
    virtual void undo();


  private:

    CSMes *m_csmes_p;
    QString m_name;
    Executions::execution_status_t m_execution_status_new;
    Executions::execution_status_t m_execution_status_old;
};
#endif
