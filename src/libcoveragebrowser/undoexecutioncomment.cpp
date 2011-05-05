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

#include "undoexecutioncomment.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdExecutionComment::UndoCmdExecutionComment (CSMes *csmes_p,WMain *wmain_p,const QString &name,const QString &old_comment,const QString &new_comment,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_name=name;
  m_old_comment=old_comment;
  m_new_comment=new_comment;
  m_wmain_p=wmain_p;

  if (m_old_comment==QString() && m_new_comment!=QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Set comment for '%1'.").arg(name))  ;
  else if (m_old_comment!=QString() && m_new_comment==QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Clear comment for '%1'.").arg(name))  ;
  else if (m_old_comment!=QString() && m_new_comment!=QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Change comment for '%1'.").arg(name))  ;
}

void UndoCmdExecutionComment::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setExecutionComment(m_name,m_old_comment);
  m_wmain_p->clearExecutionComment();
}

void UndoCmdExecutionComment::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setExecutionComment(m_name,m_new_comment);
  m_wmain_p->clearExecutionComment();
}




