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

#include "csmesexecution.h"
#include "Service.h"

CSMesExecution::CSMesExecution() :CSMesFunctionInfo()
{
}

void CSMesExecution::clear()
{
  executions.clear();
  _is_test_count_mode_selected=false;
  CSMesInstrumentation::clear();
  _selected_executions.clear();
  _selected_comparaison.clear();
}

void CSMesExecution::setExecutionState(Instrumentation &ins,const Executions::executions_t &mes, Instrumentation::coverage_method_t method,int translation) const
{
  int m=ins.getMaxIndex();
  for (int id=ins.getMinIndex();id<=m;id++)
  {
    if (method==Instrumentation::COVERAGE_BRANCH)
    {
      if (ins.isConditionInstrumentation())
      {
        ins.setExecution(id,Instrumentation::EXECUTION_STATE_NONE);
        continue;
      }
    }
    Instrumentation::execution_state_t  mes_state=mes[id-translation];
    switch (mes_state)
    {
      case Instrumentation::EXECUTION_STATE_NOT_EXECUTED:
        {
          Instrumentation::execution_state_t state=ins.getExecution(id);
          if (state==Instrumentation::EXECUTION_STATE_UNKNOWN)
            ins.setExecution(id,Instrumentation::EXECUTION_STATE_NOT_EXECUTED);
        }
        break;
      default:
        ASSERT(static_cast<int>(mes_state)>0);
        /* fall through */
      case Instrumentation::EXECUTION_STATE_HIDDEN:
      case Instrumentation::EXECUTION_STATE_NONE:
        ins.setExecution(id,mes_state);
        break;
      case Instrumentation::EXECUTION_STATE_UNKNOWN:
        break;
    }
  }
}


void CSMesExecution::clearExecutions(CSMesInstrumentations &_instrumentations) const
{
  CSMesInstrumentations::Modules::Iterator modit;
  for ( modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
  {
    CSMesInstrumentations::Sources::Iterator srcit;
    for ( srcit = modit.value().sources.begin(); srcit != modit.value().sources.end(); ++srcit )
    {
      CSMesInstrumentations::Instrumentations *inst_p=&(srcit.value().instrumentations);
      int sz=inst_p->size();
      for (int i=0;i<sz;i++)
      {
          (*inst_p)[i].clearExecution();
      }
    }
  }
}

bool CSMesExecution::selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const ExecutionNames &ms,const ExecutionNames &cmp,bool test_coverage_mode,Instrumentation::coverage_method_t method,const bool &abort_operation) const
{
  bool ret = _selectExecutionsComparaison(_instrumentations,ms,cmp,test_coverage_mode,method,abort_operation) ;
  return ret;
}

bool CSMesExecution::_selectExecutionsComparaisonInit(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations) const
{
  CSMesInstrumentations::Modules::const_iterator  modit ;
  for (modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
  {
    ASSERT(!execution.executions.contains(modit.key()));
    Executions::executions_t &mes=execution.executions[modit.key()];
    int mes_sz=(*modit).nb_measurements_items;
    mes.resize(mes_sz);
    for (int i=0;i<mes_sz;i++)
      mes[i]=Instrumentation::EXECUTION_STATE_UNKNOWN;
  }
  return true;
}


bool CSMesExecution::_selectExecutionsComparaisonCombileHideExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &comparaison) const
{
  CSMesInstrumentations::Modules::const_iterator  modit ;
  if (!comparaison.isEmpty())
  { // comparaison mode: all unexecuted lines are hidden
    for (modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
    {
      Executions::executions_t &mes=execution.executions[modit.key()];
      int mes_sz=(*modit).nb_measurements_items;
      for (int i=0;i<mes_sz;i++)
        if (static_cast<int>(mes[i])<=0)
          mes[i]=Instrumentation::EXECUTION_STATE_HIDDEN;
    }
  }
  return true;
}

bool CSMesExecution::_selectExecutionsComparaisonCombileExecutionsComparaison(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &comparaison,const bool &abort_operation) const
{
  CSMesInstrumentations::Modules::const_iterator  modit ;
  ExecutionNames mes=executions.optimizeExecutionListForCaching(comparaison);
  for ( ExecutionNames::const_iterator comparaisonit = mes.begin(); comparaisonit != mes.end(); ++comparaisonit )
  {
    if (abort_operation)
      return false;
    Executions::modules_executions_t modmes=executions.getExecution(*comparaisonit);
    for (modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
    {
      Executions::executions_t &mes=execution.executions[modit.key()];
      int mes_sz=(*modit).nb_measurements_items;
      const Executions::executions_t &mesp= modmes.executions[modit.key()];
      ASSERT(mesp.size()==mes_sz);
      for (int i=0;i<mes_sz;i++)
      {
        if (static_cast<int>(mesp.at(i))>0)
          mes[i]=Instrumentation::EXECUTION_STATE_HIDDEN;
      }
    }
  }

  return true;
}

bool CSMesExecution::_selectExecutionsComparaisonCombileExecutions(Executions::modules_executions_t &execution,const CSMesInstrumentations &_instrumentations,const ExecutionNames &ms,bool test_coverage_mode,const bool &abort_operation) const
{
  CSMesInstrumentations::Modules::const_iterator  modit ;
  ExecutionNames mes=executions.optimizeExecutionListForCaching(ms);
  for ( ExecutionNames::const_iterator msit = mes.begin(); msit != mes.end(); ++msit )
  {
    if (abort_operation)
      return false;
    Executions::modules_executions_t modmes=executions.getExecution(*msit);
    for (modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
    {
      int i;
      const QString &mod=modit.key();
      ASSERT(!mod.isEmpty());
      const Executions::executions_t &mesp= modmes.executions[mod] ;
      int mesp_sz=mesp.size();
      Executions::executions_t &mes=execution.executions[mod];
      int mes_sz=(*modit).nb_measurements_items;
      ASSERT(mesp_sz<=mes_sz);
      for (i=0;i<mesp_sz;i++)
      {
        Instrumentation::execution_state_t e=Instrumentation::adjustExecutionCount(mesp.at(i),test_coverage_mode);
        mes[i]=Instrumentation::combineExecution(e,mes.at(i));
        ASSERT( mes[i]!=Instrumentation::EXECUTION_STATE_UNKNOWN);
      }
      for (;i<mes_sz;i++)
      {
        Instrumentation::execution_state_t e=Instrumentation::adjustExecutionCount(Instrumentation::EXECUTION_STATE_NOT_EXECUTED,test_coverage_mode);
        mes[i]=Instrumentation::combineExecution(e,mes.at(i));
        ASSERT( mes[i]!=Instrumentation::EXECUTION_STATE_UNKNOWN);
      }
    }
  }

  return true;
}

bool CSMesExecution::_selectExecutionsComparaisonUpdateInstrumentation(const Executions::modules_executions_t &execution,CSMesInstrumentations &_instrumentations,Instrumentation::coverage_method_t method,const bool &abort_operation) const
{
  CSMesInstrumentations::Modules::Iterator  modit ;
  for (modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
  {
    if (abort_operation)
      return false;
    const Executions::executions_t &mes=execution.executions[modit.key()];
    // Propagate the state in the equivalent measurements
    CSMesInstrumentations::Sources::Iterator srcit;
    for ( srcit = modit.value().sources.begin(); srcit != modit.value().sources.end(); ++srcit )
    {
      QString cur_source=srcit.key();
      CSMesInstrumentations::Instrumentations *inst_p=&(srcit.value().instrumentations);
      int sz=inst_p->size();
      int instrument_id;

      for (instrument_id=0;instrument_id<sz;instrument_id++)
      {
        /* set the current instrumentation */
        Instrumentation &ins=(*inst_p)[instrument_id];
        setExecutionState(ins,mes,method, 0);
        ASSERT(ins.status(1,method)!=Instrumentation::STATUS_UNKNOWN);

        for (Instrumentation *equiv=ins.getEquivalent();equiv!=&ins;equiv=equiv->getEquivalent())
          equiv->copyExecutionState(ins);
      }
    }
  }
  return true;
}

bool CSMesExecution::_selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const ExecutionNames &_ms,const ExecutionNames &_cmp,bool test_coverage_mode,Instrumentation::coverage_method_t method,const bool &abort_operation) const
{
  ExecutionNames ms;
  for ( ExecutionNames::const_iterator it = _ms.begin(); it != _ms.end(); ++it )
  {
    if ( executions.exists(*it) && (!ms.contains(*it)))
      ms += *it;
  }
  ExecutionNames cmp;
  for ( ExecutionNames::const_iterator _it = _cmp.begin(); _it != _cmp.end(); ++_it )
  {
    if ( executions.exists(*_it) && (!cmp.contains(*_it)))
      cmp += *_it;
  }

  clearExecutions(_instrumentations);
  if (ms.isEmpty())
    return true;
  Executions::modules_executions_t execution;

  _selectExecutionsComparaisonInit(execution,_instrumentations) ;

  _selectExecutionsComparaisonCombileExecutions(execution,_instrumentations,ms,test_coverage_mode,abort_operation);

  if (!abort_operation)
    _selectExecutionsComparaisonCombileExecutionsComparaison(execution,_instrumentations,cmp,abort_operation);

  if (!abort_operation)
    _selectExecutionsComparaisonCombileHideExecutions(execution,_instrumentations,cmp);

  if (!abort_operation)
    _selectExecutionsComparaisonUpdateInstrumentation(execution,_instrumentations,method,abort_operation);
  return true;
}

ExecutionNames CSMesExecution::executionList() const
{
  return executions.getExecutionList();
}

bool CSMesExecution::deleteExecution(const ExecutionNames &ms)
{
  bool removed=false;
  for ( ExecutionNames::const_iterator it = ms.begin(); it != ms.end(); ++it )
  {
    removed = deleteExecution(*it) || removed ;
  }
  return removed;
}

Executions::execution_status_t CSMesExecution::combineExecutionStatus(Executions::execution_status_t a,Executions::execution_status_t b)
{
   if (a!=b)
      return Executions::EXECUTION_STATUS_UNKNOWN;
   else
      return a;
}


bool CSMesExecution::mergeExecutions(const ExecutionNames &sources,const ExecutionName &dest)
{
   if (executions.exists(dest))
      return false;

   Executions::modules_executions_t mod_exec;
   executions.setExecution(dest,mod_exec);
   ModuleFiles modules=Modules();

   bool first=true;
   for (ExecutionNames::const_iterator execit=sources.begin();
      execit!=sources.end();
      ++execit)
   {
      if (first)
         executions.setExecutionStatus(dest,executions.getExecutionStatus(*execit));
      else
         executions.setExecutionStatus(dest,combineExecutionStatus(executions.getExecutionStatus(dest),executions.getExecutionStatus(*execit)));
      first=false;
   }
   for (ModuleFiles::const_iterator modit=modules.begin();
      modit!=modules.end();
      ++modit)
   {
     Executions::executions_t exec;
      int nb_exec=instrumentations.modules[*modit].nb_measurements_items;
      exec.resize(nb_exec);
      for (int i=0;i<nb_exec;i++)
         exec[i]=Instrumentation::EXECUTION_STATE_NOT_EXECUTED;

      for (ExecutionNames::const_iterator execit=sources.begin();
         execit!=sources.end();
         ++execit)
      {
         Executions::executions_t e=executions.getExecutionModule(*execit,*modit);
         for (int iexec=0;iexec<nb_exec;iexec++)
            exec[iexec]=Instrumentation::combineExecution(exec[iexec],e[iexec]);
      }
      executions.setExecutionModule(dest,*modit,exec);
   }

   return true;
}

bool CSMesExecution::deleteExecution(const ExecutionName &m)
{
  if (executions.exists(m))
  {
    executions.remove(m);
    setModificationFlag();
    return true;
  }
  return false;
}


bool CSMesExecution::renameExecution(const ExecutionName &old_name,const ExecutionName &new_name)
{
  if (!executionExists(old_name))
    return false;
  if (executionExists(new_name))
    return false;
  setModificationFlag();
  executions.copy(new_name,old_name);
  return deleteExecution(old_name);
}

bool CSMesExecution::executionPathExists(const ExecutionName &m) const
{
  QString path=m;
  if (m.right(1)!="/")
    path+="/";

  ExecutionNames execution_list=executions.getExecutionList();
  for (ExecutionNames::const_iterator it = execution_list.begin(); it != execution_list.end() ; ++it )
    if ((*it).indexOf(path)==0)
      return true;
  return false;
}


ExecutionName CSMesExecution::duplicateExecution (const Executions::modules_executions_t &exec,const ExecutionNames &exclude_names) const
{
  ExecutionNames execution_list=executions.optimizeExecutionListForCaching(executions.getExecutionList());
  for (ExecutionNames::const_iterator mesit = execution_list.begin(); mesit != execution_list.end() ; ++mesit )
  {
    if (!exclude_names.contains(*mesit))
    {
      if (sameExecution(*mesit,exec))
        return *mesit;
    }
  }
  return ExecutionName();
}


bool CSMesExecution::emptyExecution(const Executions::modules_executions_t &exec) const
{
  const Executions::list_modules_executions_t &name_mes=exec.executions;
  ModuleFiles modlist=name_mes.keys();
  qSort(modlist);


  for (Executions::list_modules_executions_t::const_iterator modit = name_mes.begin(); modit != name_mes.end() ; ++modit )
  {
    QString n=modit.key();
    const Executions::executions_t &mes=modit.value();
    int sz=mes.count();

    for (int i=0;i<sz;i++)
    {
      if (mes[i]!=Instrumentation::EXECUTION_STATE_NOT_EXECUTED)
        return false;
    }
  }
  return true;
}


void CSMesExecution::mergeInExecution(const ExecutionName &name1,const Executions::modules_executions_t &exec2)
{
  if (!executions.exists(name1))
  {
    executions.setExecution(name1,exec2);
    return;
  }
  Executions::modules_executions_t exec1=executions.getExecution(name1);
  const Executions::list_modules_executions_t &name2_mes=exec2.executions;
  Executions::list_modules_executions_t &name1_mes=exec1.executions;

  for (Executions::list_modules_executions_t::Iterator modit1 = name1_mes.begin(); modit1 != name1_mes.end() ; ++modit1 )
  {
    QString n=modit1.key();
    Executions::executions_t &mes1=modit1.value();
    const Executions::executions_t &mes2=name2_mes[n];
    int sz1=mes1.count();

    ASSERT (sz1==mes2.count());
    for (int i=0;i<sz1;i++)
    {
      mes1[i]=Instrumentation::combineExecution( mes1[i] , mes2[i] ) ;
    }
  }
  executions.setExecution(name1,exec1);
}


bool CSMesExecution::sameExecution(const Executions::modules_executions_t &exec1,const Executions::modules_executions_t &exec2) const
{
  const Executions::list_modules_executions_t &name2_mes=exec2.executions;
  const Executions::list_modules_executions_t &name1_mes=exec1.executions;
  ModuleFiles modlist1=name1_mes.keys();
  ModuleFiles modlist2=name2_mes.keys();
  qSort(modlist1);
  qSort(modlist2);

  if (modlist1!=modlist2)
    return false;

  for (Executions::list_modules_executions_t::const_iterator modit1 = name1_mes.begin(); modit1 != name1_mes.end() ; ++modit1 )
  {
    QString n=modit1.key();
    const Executions::executions_t &mes1=modit1.value();
    const Executions::executions_t &mes2=name2_mes[n];
    int sz1=mes1.count();
    int sz2=mes2.count();

    if (sz1!=sz2)
      return false;
    for (int i=0;i<sz1;i++)
    {
      if (mes1[i]!=mes2[i])
        return false;
    }
  }
  return true;
}

bool CSMesExecution::sameExecution(const ExecutionName &name1,const ExecutionName &name2) const
{
  if (!executions.exists(name1))
    return false;
  if (!executions.exists(name2))
    return false;
  Executions::modules_executions_t exec2=executions.getExecution(name2);
  Executions::modules_executions_t exec1=executions.getExecution(name1);
  return sameExecution(exec1,exec2);
}


bool CSMesExecution::sameExecution(const ExecutionName &name1,const Executions::modules_executions_t &exec2) const
{
  if (!executions.exists(name1))
    return false;
  Executions::modules_executions_t exec1=executions.getExecution(name1);
  return sameExecution(exec1,exec2);
}

Executions::execution_status_t CSMesExecution::getExecutionStatus(const ExecutionName &name) const
{
   Executions::execution_status_t  ret=Executions::EXECUTION_STATUS_UNKNOWN;
  if (executions.exists(name))
  {
    ret=executions.getExecutionStatus(name);
  }
  return ret;
}

QStringList CSMesExecution::executionsStatusStr()
{
  QStringList l;
  l+=QObject::tr("Unknown");
  l+=QObject::tr("Failed");
  l+=QObject::tr("Passed");
  l+=QObject::tr("To be checked manually");

  return l;
}

QColor CSMesExecution::executionStatusColor(Executions::execution_status_t v)
{
   switch (v)
   {
   case Executions::EXECUTION_STATUS_PASSED:
      return COL_GREEN;
   case Executions::EXECUTION_STATUS_FAILED:
      return COL_RED;
   case Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY:
      return COL_ORANGE;
   default:
      return COL_WHITE;
   }
}

QString CSMesExecution::getExecutionStatusStr(const ExecutionName &name) const
{
  Executions::execution_status_t st= getExecutionStatus(name);
  int index=static_cast<int>(st);
  return executionsStatusStr()[index];
}

void CSMesExecution::setExecutionStatusStr(const ExecutionName &name,const QString &execution_status)
{
  int index=executionsStatusStr().indexOf(execution_status);
  ASSERT(index!=-1);
  Executions::execution_status_t st=static_cast<Executions::execution_status_t>(index);
  setExecutionStatus(name,st);
}


void CSMesExecution::setExecutionStatus(const ExecutionName &name,Executions::execution_status_t t)
{
  if (executions.exists(name))
  {
    executions.setExecutionStatus(name,t);
    setModificationFlag();
  }
}

bool CSMesExecution::findModuleSourceForInstrumentation(ModuleFile &mod,SourceFile &src,const Instrumentation *equiv) const
{
  for (CSMesInstrumentations::Modules::const_iterator modit = instrumentations.modules.begin();
      modit != instrumentations.modules.end();
      ++modit )
  {
    for ( CSMesInstrumentations::Sources::const_iterator srcit = modit.value().sources.begin();
        srcit != modit.value().sources.end();
        ++srcit )
    {
      const CSMesInstrumentations::Instrumentations &inst_lst=srcit.value().instrumentations;
      for (CSMesInstrumentations::Instrumentations::const_iterator it_inst=inst_lst.begin();
          it_inst!=inst_lst.end();
          ++it_inst)
        if ( &(*it_inst)==equiv )
        {
          mod=modit.key();
          src=srcit.key();
          return true;
        }
    }
  }
  return false;
}

ExecutionNames CSMesExecution::executedBy(ModuleFile module,SourceFile source,int instrument_id,bool selected_executions_only, unsigned int max_executions_listed) const
{
  findSourceModule(module,source);
  ASSERT(module!="");
  ASSERT(source!="");
  ExecutionNames execList;
  const CSMesInstrumentations::Instrumentations &inst_lst=instrumentations.modules[module].sources[source].instrumentations;
  const Instrumentation *inst_p=NULL;
  for (CSMesInstrumentations::Instrumentations::const_iterator it=inst_lst.begin();it!=inst_lst.end();++it)
    if ( (*it).validIndex(instrument_id) )
      inst_p=&(*it);

  unsigned int nb_execution_listed=0;
  ASSERT(inst_p);
  int offset=instrument_id-inst_p->getMinIndex();
  bool first=true;
  for (const Instrumentation *equiv=inst_p->getEquivalent();first || (equiv!=inst_p);equiv=equiv->getEquivalent())
  {
    first=false;
    int index=equiv->getMinIndex()+offset;
    ModuleFile mod;
    SourceFile src;
    if (findModuleSourceForInstrumentation(mod,src,equiv))
    {
      ExecutionNames execution_list;
      if (selected_executions_only)
        execution_list=executions.optimizeExecutionListForCaching(_selected_executions);
      else
        execution_list=executions.optimizeExecutionListForCaching(executions.getExecutionList());
      for (ExecutionNames::const_iterator it=execution_list.begin();it!=execution_list.end();++it)
      {
        const QString &execName=(*it);
        if (executions.exists(execName))
        {
          Executions::modules_executions_t exec=executions.getExecution(execName);
          const Executions::executions_t &exec_mod=exec.executions[mod];
          if (index<exec_mod.size())
          {
            Instrumentation::execution_state_t exec_state=exec_mod[index];
            if (exec_state>0)
              if (!execList.contains(execName))
              {
                nb_execution_listed++;
                execList+=execName;
                if (nb_execution_listed>=max_executions_listed)
                  return execList;
              }
          }
        }
      }
    }
  }
  return execList;
}

bool CSMesExecution::executionNameValid(const ExecutionName &name,QString &explanation)
{
   explanation.clear();
  if (name.isEmpty())
  {
     explanation=QObject::tr("Execution name is empty");
     return false;
  }

  if (name.right(1)=="/")
  {
     explanation=QObject::tr("Execution name ends with '/'");
     return false;
  }

  if (name.left(1)=="/")
  {
     explanation=QObject::tr("Execution name starts with '/'");
     return false;
  }

  if (name.contains("//"))
  {
     explanation=QObject::tr("Execution name contains '//'");
     return false;
  }

  return true;
}

bool CSMesExecution::setExecutionComment(const ExecutionName &name, const QString &comment)
{
  if (executions.exists(name))
  {
    executions.setExecutionComment( name , comment );
    setModificationFlag();
    return true;
  }
  return false;
}

bool CSMesExecution::getExecutionComment(const ExecutionName &name, QString &comment) const
{
  if (executions.exists(name))
  {
    executions.getExecutionComment(name,comment) ;
    return true;
  }
  return false;
}

bool CSMesExecution::createEmptyExecution(Executions::modules_executions_t &mts) const
{
  mts.execution_status=Executions::EXECUTION_STATUS_UNKNOWN;
  const QList<ModuleFile> &modules_list=instrumentations.modules.keys();
  for (QList<ModuleFile>::const_iterator mod=modules_list.begin();mod!=modules_list.end();++mod)
  {
    const ModuleFile module=*mod;
    const QList<SourceFile> &sources_list=instrumentations.modules[module].sources.keys();
    int max_index=0;
    for (QList<SourceFile>::const_iterator src=sources_list.begin();src!=sources_list.end();++src)
    {
      const SourceFile source=*src;
      const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[module].sources[source].instrumentations;
      int sz=inst.count();
      for (int i=0;i<sz;i++)
      {
        int maxIndex=inst.at(i).getMaxIndex();
        if (maxIndex>max_index)
          max_index=maxIndex;
      }
    }
    max_index++;
    QVector<Instrumentation::execution_state_t> &mes=mts.executions[module];
    mes.resize(max_index);
    for (int i=0;i<max_index;i++)
      mes[i]=Instrumentation::EXECUTION_STATE_NOT_EXECUTED;
    for (QList<SourceFile>::const_iterator src=sources_list.begin();src!=sources_list.end();++src)
    {
      const QString source=*src;
      const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[module].sources[source].instrumentations;
      int sz=inst.count();
      for (int i=0;i<sz;i++)
      {
        if (inst.at(i).getType()==Instrumentation::NOP)
          mes[inst.at(i).getMinIndex()]=Instrumentation::EXECUTION_STATE_HIDDEN;
      }
    }
  }
  return true;
}

void CSMesExecution::restoreExecution(const ExecutionName &name,const Executions::modules_executions_private_t &exec)
{
  executions.restoreExecution(name,exec);
}

bool CSMesExecution::backupExecution(const ExecutionName &name,Executions::modules_executions_private_t &exec) const
{
  return executions.backupExecution(name,exec);
}
