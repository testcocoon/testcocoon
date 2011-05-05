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

#include "undomanuallyvalidated.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdManuallyValidated::UndoCmdManuallyValidated (CSMes *csmes_p,const QString &module,const QString &source,int index,const bool &old_manually_validated,const bool &new_manually_validated,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_module=module;
  m_source=source;
  m_index=index;
  m_old_manually_validated=old_manually_validated;
  m_new_manually_validated=new_manually_validated;

  if (m_new_manually_validated)
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Set manually validated flag in '%1'.").arg(source))  ;
  else 
    setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("clear manually validated flag in '%1'.").arg(source))  ;
}

void UndoCmdManuallyValidated::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setManuallyValidated(m_module,m_source,m_index,m_old_manually_validated);
}

void UndoCmdManuallyValidated::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->setManuallyValidated(m_module,m_source,m_index,m_new_manually_validated);
}
