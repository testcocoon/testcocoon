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


#include "undoexecutionsettings.h"
#include "csmes.h"
#include "WMain.h"
#include "CoverageSettings.h"
#include "csmesbackgroundcomputations.h"
#include <QTime>

UndoCmdExecutionSettings::UndoCmdExecutionSettings (CSMes *csmes_p,const ExecutionNames &ms,const ExecutionNames &comparaison,bool test_coverage_mode, int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t m, bool execution_analysis_mode,QUndoCommand *parent) : QUndoCommand("",parent)
{
  m_csmes_p=csmes_p;
  redo_settings.selectExecutionsComparaison(ms,comparaison,test_coverage_mode, coverage_level,method, m, execution_analysis_mode) ;
  CoverageSettings::object().backup(undo_settings);
  setText(QTime::currentTime().toString(UNDO_TIME_FORMAT)+redo_settings.descriptionOfDifferences(undo_settings))  ;
}

UndoCmdExecutionSettings::~UndoCmdExecutionSettings ()
{
}

void UndoCmdExecutionSettings::undo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->selectExecutionsComparaison(
      undo_settings.getSelectedExecutions(),
      undo_settings.getSelectedExecutionsComparaison(),
      undo_settings.getTestCoverageMode(),
      undo_settings.getCoverageMethod(),
      undo_settings.getReleaseComparaisonMode()
      ) ;
  CoverageSettings::object().restore(undo_settings);
}

void UndoCmdExecutionSettings::redo()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  m_csmes_p->selectExecutionsComparaison(
      redo_settings.getSelectedExecutions(),
      redo_settings.getSelectedExecutionsComparaison(),
      redo_settings.getTestCoverageMode(),
      redo_settings.getCoverageMethod(),
      redo_settings.getReleaseComparaisonMode()
      ) ;
  CoverageSettings::object().restore(redo_settings);
}

bool UndoCmdExecutionSettings::nop() const 
{
  return undo_settings==redo_settings;
}
