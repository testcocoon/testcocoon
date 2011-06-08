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

#include "undoexecutionrename.h"
#include "csmes.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdExecutionRename::UndoCmdExecutionRename (CSMes *csmes_p,const ExecutionName &old_name,const ExecutionName &new_name,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_new_name=new_name;
  m_old_name=old_name;

  setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Rename execution '%1' in '%2'.").arg(old_name).arg(new_name))  ;
}

UndoCmdExecutionRename::~UndoCmdExecutionRename ()
{
}

void UndoCmdExecutionRename::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->renameExecution(m_new_name,m_old_name);
}

void UndoCmdExecutionRename::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->renameExecution(m_old_name,m_new_name);
}




