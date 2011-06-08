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

#ifndef UNDO_EXECUTION_MERGE_H
#define UNDO_EXECUTION_MERGE_H
#include <QUndoCommand>
#include <QString>
#include <QStringList>
#include "executionnames.h"
class CSMes ;


class UndoCmdExecutionMerge : public QUndoCommand
{
  public:
    UndoCmdExecutionMerge (CSMes *csmes_p,const ExecutionNames &sources,const ExecutionName &execution_name, QUndoCommand *parent);
    virtual ~UndoCmdExecutionMerge ();
    virtual void redo();
    virtual void undo();


  private:

    CSMes *m_csmes_p;
    ExecutionName m_name;
    ExecutionNames m_sources;
};
#endif
