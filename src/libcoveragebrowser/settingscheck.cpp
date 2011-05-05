/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
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

#include "settingscheck.h"
#include "version.h"
#include "CoverageSettings.h"
#include "options.h"
#include <QMessageBox>
#include <QPushButton>

SettingsCheck::SettingsCheck()
{
  {
    warning_t warning;
    warning.description=QObject::tr("CoverageBrowser is limiting its analysis to the coverage at the branch level.");
    warning.correct_text=QObject::tr("Enable condition/decision coverage");
    warning.warn=branch_method_warn;
    warning.solve=branch_method_solve;
    warnings.append(warning);
  }

  {
    warning_t warning;
    warning.description=QObject::tr("Coverage level is not 1.");
    warning.correct_text=QObject::tr("Set coverage level to 1");
    warning.warn=coverage_level_warn;
    warning.solve=coverage_level_solve;
    warnings.append(warning);
  }


  {
    warning_t warning;
    warning.description=QObject::tr("Test coverage count mode is activated.");
    warning.correct_text=QObject::tr("Disable test coverage count mode");
    warning.warn=test_count_mode_warn;
    warning.solve=test_count_mode_solve;
    warnings.append(warning);
  }
}

SettingsCheck &SettingsCheck::object()
{
   static SettingsCheck instance;
   return instance;
}

void SettingsCheck::check()
{
  if (Options::get_opt_int(QString(),"TESTCOCOON_VERSION")!=TESTCOCOON_VERSION)
  {
    QList<int> warnings_to_ignore = Options::get_opt_intlst(QString(),"WARNING_TO_IGNORE");
    if (!warnings_to_ignore.isEmpty())
    {
      switch (QMessageBox::question(NULL, QObject::tr("TestCocoon"),
            QObject::tr("Do you want to show all configuration warnings?"),QMessageBox::Yes,QMessageBox::No))
      {
        case QMessageBox::Yes:
          clear();
          break;
        default:
          break ;
      }
    }
  }

  QList<int> warnings_to_ignore = Options::get_opt_intlst(QString(),"WARNING_TO_IGNORE");
  int warn_index=0;
  for (QList<warning_t>::Iterator itWarn=warnings.begin();itWarn!=warnings.end();++itWarn)
  {
    if (!warnings_to_ignore.contains(warn_index))
    {
      if ((*itWarn).warn())
      {
        if (!warn((*itWarn)))
        { // ignore this warning
          warnings_to_ignore.append(warn_index);
        }
      }
    }
    warn_index++;
  }
  Options::set_opt(QString(),"WARNING_TO_IGNORE",warnings_to_ignore);
}


void SettingsCheck::clear()
{
  Options::set_opt(QString(),"WARNING_TO_IGNORE",QList<int>());
}

bool SettingsCheck::warn(const SettingsCheck::warning_t &w)
{
  QMessageBox message(QMessageBox::Information,
      QObject::tr("TestCocoon Settings Warning"),
      w.description,
      QMessageBox::Ignore);
  QAbstractButton *correct_issue=message.addButton(w.correct_text,QMessageBox::DestructiveRole );
  QAbstractButton *dont_show_again=message.addButton(QObject::tr("Don't show this message again"),QMessageBox::DestructiveRole );
  message.exec();
  if (message.clickedButton()==dont_show_again)
    return false;
  if (message.clickedButton()==correct_issue)
    w.solve();
    
  return true;
}

bool SettingsCheck::branch_method_warn()
{
  return CoverageSettings::object().getCoverageMethod() == Instrumentation::COVERAGE_BRANCH;
}

void SettingsCheck::branch_method_solve()
{
  return CoverageSettings::object().setCoverageMethod(Instrumentation::COVERAGE_CONDITION);
}


bool SettingsCheck::coverage_level_warn()
{
  return CoverageSettings::object().getCoverageLevel() != 1;
}

void SettingsCheck::coverage_level_solve()
{
  return CoverageSettings::object().setCoverageLevel(1);
}

bool SettingsCheck::test_count_mode_warn()
{
  return CoverageSettings::object().getTestCoverageMode();
}

void SettingsCheck::test_count_mode_solve()
{
  return CoverageSettings::object().setTestCoverageMode(false);
}


bool SettingsCheck::execution_benefit_mode_warn()
{
  return CoverageSettings::object().getExecutionAnalysisMode();
}

void SettingsCheck::execution_benefit_mode_solve()
{
  return CoverageSettings::object().setExecutionAnalysisMode(false);
}

bool SettingsCheck::comparaison_mode_warn()
{
  return CoverageSettings::object().getReleaseComparaisonMode()!=CSMes::COMPARAISON_MODE_NONE;
}

void SettingsCheck::comparaison_mode_solve()
{
  return CoverageSettings::object().setReleaseComparaisonMode(CSMes::COMPARAISON_MODE_NONE);
}
