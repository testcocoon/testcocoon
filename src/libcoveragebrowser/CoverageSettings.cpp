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

#include "CoverageSettings.h"
#include "csmesbackgroundcomputations.h"
#include "IdleEvent.h"
#include "options.h"
CoverageSettings::CoverageSettings(QObject *p) : QObject(p)
{
  _modificationEvent = new IdleEvent(this,&CoverageSettings::_modification);
  _data._source_type=CSMesUndoRedoFramework::SOURCE_UNDEF;
  _data._selected_executions.clear();
  _data._selected_executions_comparaison.clear();
  _data._coverage_method=Instrumentation::COVERAGE_CONDITION;
  _data._release_comparaison_mode=CSMes::COMPARAISON_MODE_NONE;
  _data._execution_analysis_mode=false;
  _data._coverage_level=1;
  _data._working_mode=(WORKING_MODE_WHITE_BOX);
  _data._test_coverage_mode=false;
  _save_settings=false;
}


CoverageSettings::~CoverageSettings()
{
  delete _modificationEvent;
}

static CSMesUndoRedoFramework::source_type_t str2source_type(const QString &str)
{
   if (str=="ORIGINAL")
      return CSMesUndoRedoFramework::ORIGINAL;
   else if (str=="CSMES")
      return CSMesUndoRedoFramework::CSMES;
   else
      return CSMesUndoRedoFramework::SOURCE_UNDEF;
}

static QString source_type2str(CSMesUndoRedoFramework::source_type_t t)
{
   switch (t)
   {
   case CSMesUndoRedoFramework::ORIGINAL: return "ORIGINAL";
   case CSMesUndoRedoFramework::CSMES:    return "CSMES";
   default:                               return "UNDEF";
   }
}

void CoverageSettings::_modification(QObject *me)
{
  CoverageSettings *p=dynamic_cast<CoverageSettings*> (me);
  if (p)
    p->_modification();
}

void CoverageSettings::_modification()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  emit modified();
}


void CoverageSettings::loadSettings()
{
  setCoverageMethod((Instrumentation::coverage_method_t)Options::get_opt_long(QString(),"COVERAGE_METHOD",(long)Instrumentation::COVERAGE_CONDITION));
  setReleaseComparaisonMode((CSMes::comparaison_mode_t)Options::get_opt_long(QString(),"COVERAGE_COMPARAISON_MODE",(long) CSMes::COMPARAISON_MODE_NONE));
  setCoverageLevel(Options::get_opt_int(QString(),"COVERAGE_LEVEL",1));
  setTestCoverageMode(Options::get_opt_bool(QString(),"TEST_COVERAGE_COUNT_MODE",false));
  setExecutionAnalysisMode(Options::get_opt_bool(QString(),"EXECUTION_ANALYSIS_MODE",false));
  setSelectedExecutions(ExecutionNames(ExecutionNames::fromQStringList(Options::get_opt_strlst(QString(),"SELECTED_EXECUTIONS",QStringList()))));
  setSelectedExecutionsComparaison(ExecutionNames::fromQStringList(Options::get_opt_strlst(QString(),"SELECTED_EXECUTIONS_COMPARAISON",QStringList())));
  setSourceType(str2source_type(Options::get_opt_str(QString(),"SOURCE_TYPE","UNDEF")));
  notifyModification();
  _save_settings=true;
}

void CoverageSettings::saveSettings()
{
  if (_save_settings)
  {
    Options::set_opt(QString(),"TEST_COVERAGE_COUNT_MODE",_data._test_coverage_mode);
    Options::set_opt(QString(),"EXECUTION_ANALYSIS_MODE",_data._execution_analysis_mode);
    Options::set_opt(QString(),"COVERAGE_LEVEL",_data._coverage_level);
    Options::set_opt(QString(),"COVERAGE_METHOD",static_cast<long>(_data._coverage_method));
    Options::set_opt(QString(),"COVERAGE_COMPARAISON_MODE",static_cast<long>(_data._release_comparaison_mode));
    Options::set_opt(QString(),"SELECTED_EXECUTIONS",_data._selected_executions.toQStringList());
    Options::set_opt(QString(),"SELECTED_EXECUTIONS_COMPARAISON",_data._selected_executions_comparaison.toQStringList());
    if (_data._source_type!=CSMesUndoRedoFramework::SOURCE_UNDEF)
      Options::set_opt(QString(),"SOURCE_TYPE",source_type2str(_data._source_type));
  }
}

void CoverageSettings::setExecutionAnalysisMode(bool m) 
{
  if (_data._execution_analysis_mode!=m)
  {
    _data._execution_analysis_mode=m;
    notify();
  }
}

void CoverageSettings::setReleaseComparaisonMode(CSMes::comparaison_mode_t m) 
{
  if (_data._release_comparaison_mode!=m)
  {
    _data._release_comparaison_mode=m;
    notify();
  }
}

void CoverageSettings::setCoverageMethod(Instrumentation::coverage_method_t m)
{
  if (_data._coverage_method!=m)
  {
    _data._coverage_method=m;
    notify();
  }
}

void CoverageSettings::setCoverageLevel(int l) 
{
  if (l<1) l=1;
  if (_data._coverage_level!=l)
  {
    _data._coverage_level=l;
    saveSettings();
  }
}

void CoverageSettings::setTestCoverageMode(bool b) 
{
  if (_data._test_coverage_mode!=b)
  {
    _data._test_coverage_mode=b;
    notify();
  }
}

void CoverageSettings::setWorkingMode(working_mode_t b) 
{
  if (_data._working_mode!=b)
  {
    _data._working_mode=b;
    notify();
  }
}

void CoverageSettings::setSelectedExecutions(const ExecutionNames &l)  
{
  if (_data._selected_executions!=l)
  {
    _data._selected_executions=l;
    qSort(_data._selected_executions);
    notify();
  }
}
void CoverageSettings::setSelectedExecutionsComparaison(const ExecutionNames &l) 
{
  if (_data._selected_executions_comparaison!=l)
  {
    _data._selected_executions_comparaison=l;
    qSort(_data._selected_executions_comparaison);
    notify();
  }
}



CoverageSettings& CoverageSettings::object() 
{
  static CoverageSettings *instance_p=NULL;
  if (instance_p==NULL)
  {
    instance_p=new CoverageSettings();
    instance_p->loadSettings();
  }
  return *instance_p;
}

bool CoverageSettings::selectExecutionsComparaison(const ExecutionNames &ms,const ExecutionNames &comparaison,bool test_coverage_mode, int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t m, bool execution_analysis_mode) 
{
  setCoverageLevel(coverage_level);
  setExecutionAnalysisMode(execution_analysis_mode);
  setSelectedExecutions(ms);
  setSelectedExecutionsComparaison(comparaison);
  setTestCoverageMode(test_coverage_mode);
  setCoverageMethod(method);
  setReleaseComparaisonMode(m);
  return true;
}

void CoverageSettings::notify()
{
  notifyModification();
  saveSettings();
}

void CoverageSettings::backup(CoverageSettings &_backup) const
{
  _backup._data=_data;
}

bool CoverageSettings::operator==(const CoverageSettings &s) const 
{
  if (s._data._coverage_method!=_data._coverage_method)
    return false;
  if (s._data._release_comparaison_mode!=_data._release_comparaison_mode)
    return false;
  if (s._data._execution_analysis_mode!=_data._execution_analysis_mode)
    return false;
  if (s._data._working_mode!=_data._working_mode)
    return false;
  if (s._data._coverage_level!=_data._coverage_level)
    return false;
  if (s._data._test_coverage_mode!=_data._test_coverage_mode)
    return false;
  if (s._data._selected_executions!=_data._selected_executions)
    return false;
  if (s._data._selected_executions_comparaison!=_data._selected_executions_comparaison)
    return false;
  if (s._data._source_type!=_data._source_type)
    return false;
  return true;
}


void CoverageSettings::restore(const CoverageSettings &_backup)
{
  if (*this != _backup)
  {
    _data = _backup._data;
    notify();
  }
}

QString CoverageSettings::descriptionOfDifferences (const CoverageSettings &ref) const
{
  QStringList descriptions;
  if (ref._data._coverage_method!=_data._coverage_method)
  {
    switch (_data._coverage_method)
    {
      case Instrumentation::COVERAGE_BRANCH:
        descriptions+=tr("Coverage limited to branches");
        break;
      default:
      case Instrumentation::COVERAGE_CONDITION:
        descriptions+=tr("Coverage branches, conditions and decision");
        break;
    }
  }
  if (ref._data._release_comparaison_mode!=_data._release_comparaison_mode)
  {
    switch (_data._release_comparaison_mode)
    {
      case CSMes::COMPARAISON_MODE_NONE:
        {
          descriptions+=tr("Comparison of modified functions deactivated");
          break;
        }
      case CSMes::COMPARAISON_MODE_MODIFIED_FUNCTIONS:
        {
          descriptions+=tr("Only modified functions are displayed");
          break;
        }
      case CSMes::COMPARAISON_MODE_NOT_MODIFIED_FUNCTIONS:
        descriptions+=tr("Only unmodified functions are displayed");
        break;
    }
  }
  if (ref._data._execution_analysis_mode!=_data._execution_analysis_mode)
  {
    if (_data._execution_analysis_mode)
      descriptions+=tr("Compare executions together");
    else
      descriptions+=tr("Stop comparing executions together");
  }
  if (ref._data._coverage_level!=_data._coverage_level)
  {
    descriptions+=tr("Set coverage level to %1").arg(_data._coverage_level);
  }
  if (ref._data._working_mode!=_data._working_mode)
  {
    switch (_data._working_mode)
    {
      case WORKING_MODE_BLACK_BOX:
        descriptions+=tr("Black box testing");
        break;
      case WORKING_MODE_WHITE_BOX:
        descriptions+=tr("White box testing");
        break;
    }
  }
  if (ref._data._test_coverage_mode!=_data._test_coverage_mode)
  {
    if (_data._test_coverage_mode) 
      descriptions+=tr("Test coverage mode activated");
    else
      descriptions+=tr("Test coverage mode disabled");
  }
  if (ref._data._selected_executions!=_data._selected_executions)
  {
    int n_selected=_data._selected_executions.count()-ref._data._selected_executions.count();
    if (n_selected>0)
    {
      if (n_selected>1)
        descriptions+=tr("Select %1 executions").arg(n_selected);
      else
      {
        for (ExecutionNames::const_iterator it=_data._selected_executions.begin();it!=_data._selected_executions.end();++it)
        {
          if (!ref._data._selected_executions.contains(*it))
          {
            descriptions+=tr("Select execution '%1'").arg(*it);
            break;
          }
        }
      }
    }
    else if (n_selected<0)
    {
      if (n_selected<-1)
         descriptions+=tr("Deselect %1 executions").arg(-n_selected);
      else
      {
        for (ExecutionNames::const_iterator it=ref._data._selected_executions.begin();it!=ref._data._selected_executions.end();++it)
        {
          if (!_data._selected_executions.contains(*it))
          {
            descriptions+=tr("Deselect execution '%1'").arg(*it);
            break;
          }
        }
      }
    }
    else
      descriptions+=tr("Selection changed");
  }
  if (ref._data._selected_executions_comparaison!=_data._selected_executions_comparaison)
  {
    if (_data._selected_executions_comparaison.count()==0)
      descriptions+=tr("Stop comparing");
    else
      descriptions+=tr("Compare with %1 executions").arg(_data._selected_executions_comparaison.count());
  }

  QString description;
  for (QStringList::const_iterator it=descriptions.begin();it!=descriptions.end();++it)
  {
    if (!description.isEmpty())
      description += ", ";
    description += *it;
  }
  return description;
}

void CoverageSettings::notifyModification()
{
  if (_modificationEvent)
    _modificationEvent->send();
}

bool CoverageSettings::isSelectedExecutionsModified(const CoverageSettings &reference) const 
{
  return  (reference._data._selected_executions != _data._selected_executions) ;
}

bool CoverageSettings::isSelectedExecutionsComparaisonModified(const CoverageSettings &reference) const 
{
  return  (reference._data._selected_executions_comparaison != _data._selected_executions_comparaison);
}

bool CoverageSettings::isSourceTypeModified(const CoverageSettings &reference) const 
{
  return (reference._data._source_type != _data._source_type);
}

void CoverageSettings::setSourceType(CSMesUndoRedoFramework::source_type_t t ) 
{
  if (_data._source_type!=t)
  {
    _data._source_type=t;
    notify();
  }
}

