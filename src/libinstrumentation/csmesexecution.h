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

#ifndef CSMES_EXECUTION_H
#define CSMES_EXECUTION_H
#include "libinstrumentationpdef.h"
#include "executions.h"
#include <QMutex>
#include <QMutexLocker>
#include "csmesfunctioninfo.h"
class CSMesExecution : public CSMesFunctionInfo
{
   public:
    void getExecutionCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const
    {
       executions.getCacheStatistics(cache_hit,cache_miss,max_cost,cost);
    }
    void setExecutionCacheMaxCost(long value)
    {
       executions.setCacheMaxCost(value);
    }
  protected:
    CSMesExecution();
    void restoreExecution(const QString &name,const Executions::modules_executions_private_t &exec);
    bool backupExecution(const QString &execution_name,Executions::modules_executions_private_t &execution) const;
    bool isTestCountModeSelected() const { return is_test_count_mode_selected; }
    Instrumentation::coverage_method_t coverageMethod() const { return coverage_method; }
    QStringList executionList() const;
    const QStringList &selectedExecutions() const { return selected_executions; }
    const QStringList &selectedComparaison() const { return selected_comparaison; }
    bool setExecutionComment(const QString &name, const QString &comment) ;
    bool getExecutionComment(const QString &name, QString &comment) const ;
    bool deleteExecution(const QString &) ;
    bool renameExecution(const QString &old_name,const QString &new_name) ;
    bool deleteExecution(const QStringList &ms);
    bool mergeExecutions(const QStringList &sources,const QString &dest);
    bool executionExists(const QString &m) const 
    {
       return executions.exists(m); 
    }
    bool executionPathExists(const QString &m) const ;
    void setExecutionStatus(const QString &name,Executions::execution_status_t); 
    Executions::execution_status_t getExecutionStatus(const QString &name) const; 
    static QStringList executionsStatusStr() ;
    QString getExecutionStatusStr(const QString &name) const; 
    void setExecutionStatusStr(const QString &name,const QString &execution_status) ; 
    bool selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode,const bool &abort_operation) const;
    bool selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method,const bool &abort_operation)
    {
      is_test_count_mode_selected=test_coverage_mode;
      coverage_method=method;
      selected_executions=ms;
      selected_comparaison=comparaison;
      return selectExecutionsComparaison(instrumentations,ms,comparaison,test_coverage_mode,method,abort_operation);
    }
    void mergeInExecution(const QString &name1,const Executions::modules_executions_t &exec2) ;
    QStringList executedBy(ModuleFile module,SourceFile source,int index,bool selected_executions_only) const;
    static bool executionNameValid(const QString &name,QString &explanation);
    static QColor executionStatusColor(Executions::execution_status_t v);
  protected:
    bool createEmptyExecution(Executions::modules_executions_t &) const;
    QString duplicateExecution (const Executions::modules_executions_t &exec,const QStringList &exclude_names) const;
    bool emptyExecution(const Executions::modules_executions_t &exec) const;
    bool sameExecution(const QString &name1,const QString &name2) const;
    bool sameExecution(const QString &name1,const Executions::modules_executions_t &exec2) const;
    bool sameExecution(const Executions::modules_executions_t &exec1,const Executions::modules_executions_t &exec2) const;
    void clear();
    void clearExecutions(CSMesInstrumentations &_instrumentations) const;
    void clearExecutions()
    {
      is_test_count_mode_selected=false;
      coverage_method=Instrumentation::METHOD_UNKNOWN;
      selected_executions.clear();
      selected_comparaison.clear();
      clearExecutions(instrumentations) ;
    }
    Executions executions;
  private:
    bool is_test_count_mode_selected;
    Instrumentation::coverage_method_t coverage_method;
    QStringList selected_executions;
    QStringList selected_comparaison;
    inline void setExecutionState(Instrumentation &ins,const Executions::executions_t &mes, Instrumentation::coverage_method_t , int translation) const;
    bool findModuleSourceForInstrumentation(QString &mod,QString &src,const Instrumentation *equiv) const;
    static Executions::execution_status_t combineExecutionStatus(Executions::execution_status_t,Executions::execution_status_t);
    inline bool _selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &cmp,bool test_coverage_mode,Instrumentation::coverage_method_t method,const bool &abort_operation) const;
  private:
    inline bool _selectExecutionsComparaisonInit(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations) const;
    inline bool _selectExecutionsComparaisonCombileExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const QStringList &ms,bool test_coverage_mode,const bool &abort_operation) const;
    inline bool _selectExecutionsComparaisonCombileExecutionsComparaison(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const QStringList &comparaison,const bool &abort_operation) const;
    inline bool _selectExecutionsComparaisonCombileHideExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const QStringList &comparaison) const;
    inline bool _selectExecutionsComparaisonUpdateInstrumentation(const Executions::modules_executions_t &execution,CSMesInstrumentations &_instrumentations,Instrumentation::coverage_method_t method,const bool &abort_operation) const;
};

#endif
