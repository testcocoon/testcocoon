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

#include "undocomment.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdComment::UndoCmdComment (CSMes *csmes_p,const ModuleFile &module,const SourceFile &source,int index,const QString &old_comment,const QString &new_comment,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_module=module;
  m_source=source;
  m_index=index;
  m_old_comment=old_comment;
  m_new_comment=new_comment;

  if (m_old_comment==QString() && m_new_comment!=QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Set comment in '%1'.").arg(source))  ;
  else if (m_old_comment!=QString() && m_new_comment==QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Clear comment in '%1'.").arg(source))  ;
  else if (m_old_comment!=QString() && m_new_comment!=QString())
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Change comment in '%1'.").arg(source))  ;
}

void UndoCmdComment::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setComment(m_module,m_source,m_index,m_old_comment);
}

void UndoCmdComment::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setComment(m_module,m_source,m_index,m_new_comment);
}




