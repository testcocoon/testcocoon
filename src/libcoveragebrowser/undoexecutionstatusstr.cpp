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

#include "undoexecutionstatusstr.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdExecutionStatusStr::UndoCmdExecutionStatusStr (CSMes *csmes_p,const ExecutionName &name, const QString & e, QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_name=name;
  m_execution_status_new=e;
  m_execution_status_old=m_csmes_p->getExecutionStatusStr(name);

  setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Change execution status of '%1'.").arg(name))  ;
}

UndoCmdExecutionStatusStr::~UndoCmdExecutionStatusStr ()
{
}

void UndoCmdExecutionStatusStr::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setExecutionStatusStr(m_name,m_execution_status_old);
}

void UndoCmdExecutionStatusStr::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setExecutionStatusStr(m_name,m_execution_status_new);
}




