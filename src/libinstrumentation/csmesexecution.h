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
    void restoreExecution(const ExecutionName &name,const Executions::modules_executions_private_t &exec);
    bool backupExecution(const ExecutionName &execution_name,Executions::modules_executions_private_t &execution) const;
    bool isTestCountModeSelected() const { return _is_test_count_mode_selected; }
    Instrumentation::coverage_method_t coverageMethod() const { return _coverage_method; }
    ExecutionNames executionList() const;
    const ExecutionNames &selectedExecutions() const { return _selected_executions; }
    const ExecutionNames &selectedComparaison() const { return _selected_comparaison; }
    bool setExecutionComment(const ExecutionName &name, const QString &comment) ;
    bool getExecutionComment(const ExecutionName &name, QString &comment) const ;
    bool deleteExecution(const ExecutionName &) ;
    bool renameExecution(const ExecutionName &old_name,const ExecutionName &new_name) ;
    bool deleteExecution(const ExecutionNames &ms);
    bool mergeExecutions(const ExecutionNames &sources,const ExecutionName &dest);
    bool executionExists(const ExecutionName &m) const 
    {
       return executions.exists(m); 
    }
    bool executionPathExists(const ExecutionName &m) const ;
    void setExecutionStatus(const ExecutionName &name,Executions::execution_status_t); 
    Executions::execution_status_t getExecutionStatus(const ExecutionName &name) const; 
    static QStringList executionsStatusStr() ;
    QString getExecutionStatusStr(const ExecutionName &name) const; 
    void setExecutionStatusStr(const ExecutionName &name,const QString &execution_status) ; 
    bool selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const ExecutionNames &ms,const ExecutionNames &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode,const bool &abort_operation) const;
    bool selectExecutionsComparaison(const ExecutionNames &ms,const ExecutionNames &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method,const bool &abort_operation)
    {
      _is_test_count_mode_selected=test_coverage_mode;
      _coverage_method=method;
      _selected_executions=ms;
      _selected_comparaison=comparaison;
      return selectExecutionsComparaison(instrumentations,ms,comparaison,test_coverage_mode,method,abort_operation);
    }
    void mergeInExecution(const ExecutionName &name1,const Executions::modules_executions_t &exec2) ;
    ExecutionNames executedBy(ModuleFile module,SourceFile source,int index,bool selected_executions_only,unsigned int max_executions_listed) const;
    static bool executionNameValid(const ExecutionName &name,QString &explanation);
    static QColor executionStatusColor(Executions::execution_status_t v);
  protected:
    bool createEmptyExecution(Executions::modules_executions_t &) const;
    ExecutionName duplicateExecution (const Executions::modules_executions_t &exec,const ExecutionNames &exclude_names) const;
    bool emptyExecution(const Executions::modules_executions_t &exec) const;
    bool sameExecution(const ExecutionName &name1,const ExecutionName &name2) const;
    bool sameExecution(const ExecutionName &name1,const Executions::modules_executions_t &exec2) const;
    bool sameExecution(const Executions::modules_executions_t &exec1,const Executions::modules_executions_t &exec2) const;
    void clear();
    void clearExecutions(CSMesInstrumentations &_instrumentations) const;
    void clearExecutions()
    {
      _is_test_count_mode_selected=false;
      _coverage_method=Instrumentation::METHOD_UNKNOWN;
      _selected_executions.clear();
      _selected_comparaison.clear();
      clearExecutions(instrumentations) ;
    }
    Executions executions;
  private:
    bool                                 _is_test_count_mode_selected;
    Instrumentation::coverage_method_t   _coverage_method;
    ExecutionNames                       _selected_executions;
    ExecutionNames                       _selected_comparaison;
    inline void setExecutionState(Instrumentation &ins,const Executions::executions_t &mes, Instrumentation::coverage_method_t) const;
    bool findModuleSourceForInstrumentation(ModuleFile &mod,SourceFile &src,const Instrumentation *equiv) const;
    static Executions::execution_status_t combineExecutionStatus(Executions::execution_status_t,Executions::execution_status_t);
    inline bool _selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const ExecutionNames &ms,const ExecutionNames &cmp,bool test_coverage_mode,Instrumentation::coverage_method_t method,const bool &abort_operation) const;
  private:
    inline bool _selectExecutionsComparaisonInit(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations) const;
    inline bool _selectExecutionsComparaisonCombileExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &ms,bool test_coverage_mode,const bool &abort_operation) const;
    inline bool _selectExecutionsComparaisonCombileExecutionsComparaison(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &comparaison,const bool &abort_operation) const;
    inline bool _selectExecutionsComparaisonCombileHideExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &comparaison) const;
    inline bool _selectExecutionsComparaisonUpdateInstrumentation(const Executions::modules_executions_t &execution,CSMesInstrumentations &_instrumentations,Instrumentation::coverage_method_t method,const bool &abort_operation) const;
};

#endif
