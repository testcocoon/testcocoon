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

#include "undoexecutionmerge.h"
#include "csmes.h"
#include "WMain.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>
#include "CoverageSettings.h"

UndoCmdExecutionMerge::UndoCmdExecutionMerge (CSMes *csmes_p,const QStringList &sources,const QString &name,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  m_name=name;
  m_sources=sources;

  setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+QObject::tr("Merge execution in '%1'.").arg(name))  ;
}

UndoCmdExecutionMerge::~UndoCmdExecutionMerge ()
{
}

void UndoCmdExecutionMerge::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->deleteExecution(m_name);

  QStringList ms=CoverageSettings::object().getSelectedExecutions();
  QStringList comparaison=CoverageSettings::object().getSelectedExecutionsComparaison();
  if (ms.contains(m_name) || comparaison.contains(m_name))
  {
    ms.removeAll(m_name);
    comparaison.removeAll(m_name);
    bool test_coverage_mode =  CoverageSettings::object().getTestCoverageMode();
    Instrumentation::coverage_method_t method = CoverageSettings::object().getCoverageMethod();
    CSMes::comparaison_mode_t comparaison_mode= CoverageSettings::object().getReleaseComparaisonMode();
    m_csmes_p->selectExecutionsComparaison( ms,comparaison,test_coverage_mode, method, comparaison_mode );//TODO
  }
}

void UndoCmdExecutionMerge::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->mergeExecutions(m_sources,m_name);
}




