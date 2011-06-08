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

#ifndef WCOMMENT_H
#define WCOMMENT_H
#include "ui_WOptimizedExecutionOrderUI.h"
#include "csmesbackgroundcomputations.h"
#include <QMap>

class WOptimizedExecutionOrder : public QDialog, public Ui::WOptimizedExecutionOrderUI
{
    Q_OBJECT

public:
    WOptimizedExecutionOrder(const ExecutionNames &execution_list,Instrumentation::coverage_method_t method,QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~WOptimizedExecutionOrder();

    void calculateStatistics();
    bool isFinished() const { return _finished; }
    QString toHtml() const;

public slots:
  void statisticExecution(const ExecutionNames &mes,const ExecutionNames &comparaisons,bool execution_analysis,
      int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t,int nb_tested,int nb_untested) ;
  void save();


protected slots:
    virtual void languageChange();
private:
  void removeExecution(const QString &);
  void continueCalculateStatistics();
  ExecutionNames _execution_list;
  ExecutionNames _optimized_execution_list;
  Instrumentation::coverage_method_t _method;
  int _best_nb_tested;
  int _optimized_execution_nb_tested;
  QString _best_execution;
  bool _finished;
  QStringList _under_process;
  QMap<QString,int> _nb_tested_info;
  void setNbTestedInfo(const QString &,int);
  void clearNbTestedInfo();
  void cancelRequestNbTestedBelow(int);
};


#endif
