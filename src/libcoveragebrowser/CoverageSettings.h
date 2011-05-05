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

#ifndef COVERAGE_SETTINGS_H
#define COVERAGE_SETTINGS_H
#include "coveragebrowserpdef.h"
#include "csmesundoredoframework.h"
#include "csmes.h"
#include "instrumentation.h"
#include <QString>
#include <QObject>
#include <QStringList>
class IdleEvent ;
class SettingsCheck ;

class CoverageSettings : public QObject
{
  Q_OBJECT
public:
    enum working_mode_t
    {
      WORKING_MODE_WHITE_BOX,
      WORKING_MODE_BLACK_BOX
    };
  CoverageSettings(QObject *p=NULL);
  static CoverageSettings &object() ;
  ~CoverageSettings();
  
  CSMes::comparaison_mode_t getReleaseComparaisonMode() const { return _data._release_comparaison_mode; }
  bool getExecutionAnalysisMode() const { return _data._execution_analysis_mode; }
  Instrumentation::coverage_method_t getCoverageMethod() const { return _data._coverage_method; }
  working_mode_t getWorkingMode() const { return _data._working_mode; }
  int getCoverageLevel() const { return _data._coverage_level; }
  bool getTestCoverageMode() const { return _data._test_coverage_mode; }
  QStringList getSelectedExecutions() const { return _data._selected_executions; };
  QStringList getSelectedExecutionsComparaison() const { return _data._selected_executions_comparaison; };
  bool selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t m, bool execution_analysis_mode) ;

  QString descriptionOfDifferences (const CoverageSettings &ref) const;

  bool isReleaseComparaisonModeModified(const CoverageSettings &reference) const { return  reference._data._release_comparaison_mode != _data._release_comparaison_mode; }
  bool isExecutionAnalysisModeModified(const CoverageSettings &reference) const { return  reference._data._execution_analysis_mode != _data._execution_analysis_mode; }
  bool isCoverageMethodModified(const CoverageSettings &reference) const { return  reference._data._coverage_method != _data._coverage_method; }
  bool isCoverageLevelModified(const CoverageSettings &reference) const { return  reference._data._coverage_level != _data._coverage_level; }
  bool isTestCoverageModeModified(const CoverageSettings &reference) const { return  reference._data._test_coverage_mode != _data._test_coverage_mode; }
  bool isSelectedExecutionsModified(const CoverageSettings &reference) const ;
  bool isSourceTypeModified(const CoverageSettings &reference) const ;
  bool isSelectedExecutionsComparaisonModified(const CoverageSettings &reference) const ;
  bool isWorkingModeModified(const CoverageSettings &reference) const { return  reference._data._working_mode != _data._working_mode; };
  void setWorkingMode(working_mode_t);
  CSMesUndoRedoFramework::source_type_t getSourceType() const { return _data._source_type; }
  void setSourceType(CSMesUndoRedoFramework::source_type_t ) ;
private:
  void setCoverageMethod(Instrumentation::coverage_method_t m) ;
  void setCoverageLevel(int l) ;
  void setTestCoverageMode(bool b);
  void setSelectedExecutions(const QStringList &)  ;
  void setSelectedExecutionsComparaison(const QStringList &) ;
  void setReleaseComparaisonMode(CSMes::comparaison_mode_t m) ;
  void setExecutionAnalysisMode(bool);
  friend class SettingsCheck;

public:
  void backup(CoverageSettings &) const;
  void restore(const CoverageSettings &);
  bool operator==(const CoverageSettings &s) const ;
  bool operator!=(const CoverageSettings &s) const { return !operator==(s); }

signals:
  void modified();
public slots:
  void notifyModification();
private:
  struct data_t
  {
    Instrumentation::coverage_method_t      _coverage_method;
    CSMes::comparaison_mode_t               _release_comparaison_mode;
    bool                                    _execution_analysis_mode;
    int                                     _coverage_level;
    bool                                    _test_coverage_mode;
    QStringList                             _selected_executions;
    QStringList                             _selected_executions_comparaison;
    working_mode_t                          _working_mode;
    CSMesUndoRedoFramework::source_type_t   _source_type;
  };
  data_t _data;
  bool _save_settings;
  void saveSettings();
  void loadSettings();
  
  IdleEvent *_modificationEvent;
  void notify();
  static void _modification(QObject*);
  void _modification();
} ;

#endif
