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


#include "undoexecutionbackup.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdExecutionBackup::UndoCmdExecutionBackup (CSMes *csmes_p,const ExecutionName &name,const Executions::modules_executions_private_t &exec,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_name=name;
  m_exec = exec;
  setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Backup execution '%1'.").arg(name))  ;
}

UndoCmdExecutionBackup::~UndoCmdExecutionBackup ()
{
}

void UndoCmdExecutionBackup::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->restoreExecution(m_name,m_exec);
}

void UndoCmdExecutionBackup::redo()
{
}




