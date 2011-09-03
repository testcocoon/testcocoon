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

#include "executions.h"
#include "csmesfilethread.h"
#include <QMutexLocker>

Executions::Executions():locker(QMutex::Recursive)
{
  csmes_p=NULL;
  clear();
}


void Executions::setExecution(const ExecutionName &name,const modules_executions_t &exec)
{
  AccessLockerWrite lock(&access_locker);
  executions[name].data=exec;
  executions[name].load_state=LOAD_STATE_DIRTY;
  executions[name].execution_section_hint=0;
}


Executions::modules_executions_t Executions::getExecution(const ExecutionName &name) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  ensureLoaded(name);
  Executions::modules_executions_t exec =executions.value(name).data;
  if (executions.value(name).load_state==LOAD_STATE_NOT_LOADED)
  {
    ASSERT(cache.contains(name));
    exec.executions=*cache.object(name);
    ASSERT(cache.contains(name));
  }
  return exec;
}

void Executions::clear()
{
   AccessLockerWrite lock(&access_locker);
  executions.clear();
  clearExecutionCache();
}

void Executions::clearExecutionCache()
{
   AccessLockerWrite lock(&access_locker);
  cache_miss=0;
  cache_hit=0;
  cache.clear();
}

void Executions::getCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const
{
   cache_hit=this->cache_hit;
   cache_miss=this->cache_miss;
   cost=cache.totalCost();
   max_cost=cache.maxCost();
}

void Executions::setCacheMaxCost(long value)
{
   AccessLockerWrite lock(&access_locker);
  cache.setMaxCost(value);
}


bool Executions::isLoadedInRAM(const ExecutionName &name) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  if (!executions.contains(name))
    return false;
  switch (executions[name].load_state)
  {
    case LOAD_STATE_LOADED:
    case LOAD_STATE_DIRTY:
      return true;
    case LOAD_STATE_NOT_LOADED:
       return cache.contains(name);
      break;
    case LOAD_STATE_INVALID:
      ASSERT(true);
      break;
  }
  return false;
}

bool Executions::exists(const ExecutionName &name) const
{
   AccessLockerRead lock(&access_locker);
  return executions.contains(name);
}

void Executions::remove(const ExecutionName &name)
{
   AccessLockerWrite lock(&access_locker);
  ASSERT(exists(name));
  executions.remove(name);
  cache.remove(name);
  ASSERT(!executions.contains(name));
}

bool Executions::moduleExists(const ExecutionName &name,const ModuleFile &module) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  ensureLoaded(name);
  bool ret;
  if (executions.value(name).load_state!=LOAD_STATE_NOT_LOADED)
     ret=executions[name].data.executions.contains(module);
  else
     ret=cache.object(name)->contains(module);
  return ret;
}

void Executions::setExecutionStatus(const ExecutionName &name,execution_status_t v)
{
   AccessLockerWrite lock(&access_locker);
  ASSERT(exists(name));
  ensureLoaded(name);
  executions[name].data.setExecutionStatus(v);
}

void Executions::setExecutionComment(const ExecutionName &name,const QString &comment)
{
   AccessLockerWrite lock(&access_locker);
  ASSERT(exists(name));
  ensureLoaded(name);
  executions[name].data.comment=comment;
  if (executions[name].load_state==LOAD_STATE_NOT_LOADED)
     executions[name].data.executions=*cache.object(name);
  executions[name].load_state=LOAD_STATE_DIRTY;
}

void Executions::getExecutionComment(const ExecutionName &name,QString &comment) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  comment=executions.value(name).data.comment;
}

ExecutionNames Executions::getExecutionList() const
{
   AccessLockerRead lock(&access_locker);
  return executions.keys();
}

ModuleFiles Executions::getModuleList(const ExecutionName &name) const
{
  AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  ensureLoaded(name);
  ModuleFiles ret = executions.value(name).data.executions.keys();
  return ret;
}

Executions::execution_status_t Executions::getExecutionStatus(const ExecutionName &name) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  return executions.value(name).data.executionStatus();
}


Executions::executions_t Executions::getExecutionModule(const ExecutionName &name, const ModuleFile & module) const
{
  AccessLockerRead lock(&access_locker);
  ASSERT(exists(name));
  ASSERT(moduleExists(name,module));
  ensureLoaded(name);
  Executions::executions_t exec ;
  if (executions.value(name).load_state!=LOAD_STATE_NOT_LOADED)
     exec=executions[name].data.executions[module];
  else
     exec=(*cache.object(name))[module];
  return exec;
}

void Executions::setExecutionModule(const ExecutionName &name, const ModuleFile & module,const Executions::executions_t &e)
{
   AccessLockerWrite lock(&access_locker);
  ASSERT(exists(name));
  ASSERT(moduleExists(name,module));
  ensureLoaded(name);
  executions[name].data.executions[module]=e;
  executions[name].load_state=LOAD_STATE_DIRTY;
}

void Executions::copy(const ExecutionName &dest,const ExecutionName &src)
{
   AccessLockerWrite lock(&access_locker);
  ASSERT(!exists(dest));
  ASSERT(exists(src));
  ensureLoaded(src);
  executions[dest]=executions.value(src);
  if (executions[src].load_state==LOAD_STATE_NOT_LOADED)
     executions[dest].data.executions=*cache.object(src);
  executions[dest].load_state=LOAD_STATE_DIRTY;
}

void Executions::setCSMes(CSMESFileThread *file)
{
   AccessLockerWrite lock2(&access_locker);
  QMutexLocker lock(&locker);
  csmes_p=file;
}

bool Executions::preload(const ExecutionName &execution_name)
{
  ASSERT(csmes_p);
  ASSERT (!execution_name.isEmpty());
  ASSERT (executions.contains(execution_name));

  int itry;
  int execution_section_hint=0;
  int max_tries=1;
  if (executions.contains(execution_name))
  {
    execution_section_hint=executions[execution_name].execution_section_hint;
    max_tries=2;
  }
  for (itry=0;itry<max_tries;itry++)
  {
    bool found=false;
    for (int sec_id=execution_section_hint;!found && sec_id<csmes_p->nbSections();sec_id++)
    {
      switch (csmes_p->sectionType(sec_id))
      {
        case CSMESFile::EXECUTION_COMPACT:
          {
            QString name(QString::fromUtf8(csmes_p->sectionName(sec_id)).toAscii());
            if (execution_name==name)
            {
              if (!executions.contains(name))
              {
                modules_executions_private_t v;
                v.execution_section_hint=sec_id;
                executions[name]=v;
              }
              list_modules_executions_t *execs=new list_modules_executions_t (executions[name].data.executions);
              executions[name].load_state=LOAD_STATE_NOT_LOADED;
              long nbModules;
              csmes_p->openSectionID(sec_id);
              csmes_p->readLong(nbModules);
              for (long iModule=0;iModule<nbModules;iModule++)
              {
                int nb=0;
                long *exec=NULL;
                char *module=NULL;
                if (csmes_p->readExecutionCompact(& module,& exec,& nb))
                {
                  QString mod(module);
                  if (!execs->contains(mod))
                  {
                    executions_t v;
                    (*execs)[mod]=v;
                  }
                  executions_t *inst_p=&(*execs)[mod];
                  inst_p->resize(nb);
                  for (int j=0;j<nb;j++)
                  {
                    (*inst_p)[j]=static_cast<Instrumentation::execution_state_t>(exec[j]);
                  }
                }
                free (exec);
                free (module);
              }
              if (!cache.insert(name,execs,1)) 
              {
                 ASSERT(false); 
              }
              csmes_p->closeSection();
              found = true;
            }
            if (executions.contains(name))
              executions[name].execution_section_hint=sec_id;
          }
          break;
        case CSMESFile::EXECUTION:
          {
            QString name(QString::fromUtf8(csmes_p->sectionName(sec_id)).toAscii());
            if (execution_name==name)
            {
              if (!executions.contains(name))
              {
                modules_executions_private_t v;
                v.execution_section_hint=0;
                executions[name]=v;
              }
              QString mod(csmes_p->sectionModule(sec_id));
              modules_executions_private_t execs=executions[name];
              if (!executions[name].data.executions.contains(mod))
              {
                executions_t v;
                execs.data.executions[mod]=v;
                executions[name]=execs;
              }
              executions_t *inst_p=&execs.data.executions[mod];
              csmes_p->openSectionID(sec_id);
              int nb=csmes_p->nbExecution();
              Instrumentation::execution_state_t *exec=(Instrumentation::execution_state_t*)malloc(sizeof(Instrumentation::execution_state_t)*nb);
              if (csmes_p->readExecution(reinterpret_cast<long*>(exec),nb))
              {
                inst_p->resize(nb);
                for (int j=0;j<nb;j++)
                {
                  (*inst_p)[j]=exec[j];
                }
              }
              executions[name]=execs;
              free (exec);
              csmes_p->closeSection();
            }
          }
          break;
        default:
          break;
      }
    }
    if (found)
      break;
    execution_section_hint=0;
  }

  return true;
}


bool Executions::load()
{
   AccessLockerWrite lock2(&access_locker);
  QMutexLocker lock(&locker);
  ASSERT(csmes_p);

  for (int sec_id=0; sec_id<csmes_p->nbSections();sec_id++)
  {
    switch (csmes_p->sectionType(sec_id))
    {
      case CSMESFile::EXECUTION_COMPACT:
        {
          QString name(QString::fromUtf8(csmes_p->sectionName(sec_id)).toAscii());
          if (!executions.contains(name))
          {
            modules_executions_private_t v;
            v.load_state=LOAD_STATE_NOT_LOADED;
            v.execution_section_hint=sec_id;
            executions[name]=v;
          }
          long execution_status;
          if (!csmes_p->readExecutionStatus(sec_id,execution_status))
          {
            return false;
          }
          executions[name].data.setExecutionStatus(static_cast<Executions::execution_status_t>(execution_status));
          executions[name].execution_section_hint=sec_id;
        }
        break;
      case CSMESFile::EXECUTION:
        {
          QString name(QString::fromUtf8(csmes_p->sectionName(sec_id)).toAscii());
          if (!executions.contains(name))
          {
            modules_executions_private_t v;
            v.load_state=LOAD_STATE_NOT_LOADED;
            v.execution_section_hint=0;
            executions[name]=v;
          }
          long execution_status;
          if (!csmes_p->readExecutionStatus(sec_id,execution_status))
          {
            return false;
          }
          executions[name].data.setExecutionStatus(static_cast<Executions::execution_status_t>(execution_status));
        }
        break;
      default:
        break;
    }
  }

  for (int sec_id=0;sec_id<csmes_p->nbSections();sec_id++)
  {
    switch (csmes_p->sectionType(sec_id))
    {
      case CSMESFile::EXECUTION_COMMENT:
        {
          QString name(QString::fromUtf8(csmes_p->sectionModule(sec_id)).toAscii());
          QRegExp mergeRx;
          mergeRx.setCaseSensitivity(Qt::CaseInsensitive);
          mergeRx.setMinimal(true);
          mergeRx.setPattern("</HTML>.*<HTML>");
          mergeRx.setPatternSyntax(QRegExp::RegExp);

          if (!executions.contains(name)) 
            break;

          csmes_p->openSectionID(sec_id);
          char *text;
          if (csmes_p->readExecutionComment(&text))
          {
            QString comment=QString::fromUtf8(text);
            free(text);
            executions[name].data.comment=comment;
          }
          csmes_p->closeSection();
        }
        break;
      default:
        break;
    }
  }

  return true;
}

bool Executions::save()
{
   AccessLockerWrite lock2(&access_locker);
  bool ret = save_compact_format();
  return ret;
}


bool Executions::save_executions_compact_format()
{
  /* Addition of new executions */
  list_executions_t::const_iterator execution_it;
  for (execution_it= executions.begin(); execution_it != executions.end(); ++execution_it )
  {
    if (execution_it.value().load_state==LOAD_STATE_DIRTY)
    {
      QString name=execution_it.key();
      QByteArray name_cstr=name.toUtf8();
      ModuleFiles module_list=executions[name].data.executions.keys();
      if (csmes_p->sectionExists("",name_cstr.constData(),"",CSMESFile::EXECUTION_COMPACT))
      {
        if (!csmes_p->deleteSection("",name_cstr.constData(),"",CSMESFile::EXECUTION_COMPACT))
        {
          return false;
        }
      }
      csmes_p->openSection("","",name_cstr.constData(),"","",CSMESFile::EXECUTION_COMPACT,0,CSMESFile::NEW_RW,0);
      csmes_p->writeLong(module_list.count());
      for (ModuleFiles::const_iterator mod_it= module_list.begin(); mod_it != module_list.end(); ++mod_it )
      {
        QString mod=*mod_it;
        QByteArray mod_cstr=mod.toAscii();
        Executions::executions_t execs=executions[name].data.executions[mod];
        int nb=execs.size();
        long *exec=(long*)malloc(nb*sizeof(long));
        bool empty=true;
        for (int j=0;j<nb;j++)
        {
          long e=execs[j];
          exec[j]=e;
          if (e!=0)
            empty=false;
        }
        if (!empty)
          csmes_p->writeExecutionCompact(mod_cstr.constData(),exec,nb);
        free(exec);
      }
      csmes_p->closeSection();
    }
  }

  for (int i=0;i<csmes_p->nbSections();i++)
  {
    if (csmes_p->sectionType(i)==CSMESFile::EXECUTION || csmes_p->sectionType(i)==CSMESFile::EXECUTION_COMPACT)
    {
      QString name(QString::fromUtf8(csmes_p->sectionName(i)).toAscii());
      if (executions.contains(name))
      {
        execution_status_t exec_status=executions.value(name).data.executionStatus();
        long iexecst;
        if (csmes_p->readExecutionStatus(i,iexecst))
        {
          if (static_cast<long>(exec_status)!=iexecst)
            csmes_p->writeExecutionStatus(i,static_cast<long>(exec_status));
        }
      }
    }
  }
  return true;
}

bool Executions::save_executions_old_format()
{
  /* Addition of new executions */
  list_executions_t::const_iterator execution_it;
  for (execution_it= executions.begin(); execution_it != executions.end(); ++execution_it )
  {
    QString name=execution_it.key();
    QByteArray name_cstr=name.toUtf8();
    if (execution_it.value().load_state==LOAD_STATE_DIRTY)
    {
      ModuleFiles module_list=executions[name].data.executions.keys();
      for (ModuleFiles::const_iterator mod_it= module_list.begin(); mod_it != module_list.end(); ++mod_it )
      {
        QString mod=*mod_it;
        QByteArray mod_cstr=mod.toAscii();
        if (csmes_p->sectionExists(mod_cstr.constData(),name_cstr.constData(),"",CSMESFile::EXECUTION))
        {
          if (!csmes_p->deleteSection(mod_cstr.constData(),name_cstr.constData(),"",CSMESFile::EXECUTION))
          {
            return false;
          }
        }
        csmes_p->openSection(mod_cstr.constData(),"",name_cstr.constData(),"","",CSMESFile::EXECUTION,0,CSMESFile::NEW_RW,0);
        Executions::executions_t execs=executions[name].data.executions[mod];
        int nb=execs.size();
        long *exec=(long*)malloc(nb*sizeof(long));
        bool empty=true;
        for (int j=0;j<nb;j++)
        {
          Instrumentation::execution_state_t e=execs[j];
          if (e!=Instrumentation::EXECUTION_STATE_NOT_EXECUTED)
            empty=false;
          exec[j]=e;
        }
        if (!empty)
          csmes_p->writeExecution(exec,nb);
        free(exec);
        csmes_p->closeSection();
      }
    }
    csmes_p->writeExecutionStatus(name_cstr.constData(),static_cast<long>(executions[name].data.executionStatus()));
  }
  return true;
}

bool Executions::delete_obselete_executions()
{
  /* Deletion of obselete executions */
  for (int sec_id=0;sec_id<csmes_p->nbSections();sec_id++)
  {
    switch (csmes_p->sectionType(sec_id))
    {
    case CSMESFile::EXECUTION_COMPACT:
    case CSMESFile::EXECUTION:
      {
        QString name=QString::fromUtf8(csmes_p->sectionName(sec_id));
        if (!executions.contains(name))
        {
          csmes_p->deleteSectionID(sec_id);
          sec_id=0;
        }
      }
      break;
    case CSMESFile::EXECUTION_COMMENT:
      {
        QString name(QString::fromUtf8(csmes_p->sectionModule(sec_id)).toAscii());
        if (!executions.contains(name))
        {
          csmes_p->deleteSectionID(sec_id);
          sec_id=0;
        }
      }
      break;
    default:
      break;
    }
  }
  return true;
}

bool Executions::save_comment()
{
  list_executions_t::const_iterator execution_it;

  // Saving execution comments...
  for (execution_it= executions.begin(); execution_it != executions.end(); ++execution_it )
  {
    Executions::list_modules_executions_t::const_iterator mod_it;
    QString name=execution_it.key();
    QByteArray name_cstr=name.toUtf8();
    bool overwrite = (execution_it.value().load_state==LOAD_STATE_DIRTY) ;

    if (overwrite)
    {
      QString comment=execution_it.value().data.comment;
      if (!comment.isEmpty())
      {
        csmes_p->openSection(name_cstr.constData(),"","","","",CSMESFile::EXECUTION_COMMENT,0,CSMESFile::NEW_RW,0);
        QByteArray comment_cstr=comment.toUtf8();
        csmes_p->writeExecutionComment(comment_cstr.constData());
        csmes_p->closeSection();
      }
    }
  }
  return true;
}

bool Executions::save_compact_format()
{
  QMutexLocker lock(&locker);
  ASSERT(csmes_p);

  if (!save_executions_compact_format()) return false;
  if (!delete_obselete_executions()) return false;
  if (!save_comment()) return false;

  list_executions_t::iterator execution_it;
  for (execution_it= executions.begin(); execution_it != executions.end(); ++execution_it )
    execution_it.value().load_state=LOAD_STATE_NOT_LOADED;
  return true;
}

bool Executions::save_old_format()
{
  QMutexLocker lock(&locker);
  ASSERT(csmes_p);

  if (!save_executions_old_format()) return false;
  if (!delete_obselete_executions()) return false;
  if (!save_comment()) return false;

  return true;
}


void Executions::ensureLoaded(const ExecutionName &execution_name) const
{
  QMutexLocker lock(&locker);
  ASSERT(executions.contains(execution_name));
  bool to_load= (executions[execution_name].load_state==LOAD_STATE_NOT_LOADED);
  if (to_load)
  {
    if (!cache.contains(execution_name))
    {
      const_cast<Executions*>(this)->preload(execution_name);
      cache_miss++;
    }
    else
      cache_hit++;
  }
  else
    cache_hit++;
}


ExecutionNames Executions::optimizeExecutionListForCaching(const ExecutionNames &l) const 
{
  AccessLockerWrite lock2(&access_locker);
  ExecutionNames r;
  for (ExecutionNames::const_iterator it=l.begin();it!=l.end();++it)
  {
    if (cache.contains(*it))
      r.prepend(*it);
    else
      r.append(*it);
  }
  return r;
}



void Executions::restoreExecution(const ExecutionName &execution_name,const modules_executions_private_t &execution) 
{
   AccessLockerWrite lock(&access_locker);
  if (execution.load_state==LOAD_STATE_INVALID)
    return ;
  executions[execution_name]=execution;
}


bool Executions::backupExecution(const ExecutionName &execution_name,modules_executions_private_t &execution) const
{

   AccessLockerRead lock(&access_locker);
  if (!executions.contains(execution_name))
  {
    execution.load_state=LOAD_STATE_INVALID;
    return false;
  }
  execution=executions.value(execution_name);
  return true;
}

void Executions::getExecution(const ExecutionName &execution_name,modules_executions_t &execution) const
{
   AccessLockerRead lock(&access_locker);
  ASSERT(exists(execution_name));
  ensureLoaded(execution_name);
  execution=executions.value(execution_name).data;
  if (executions[execution_name].load_state==LOAD_STATE_NOT_LOADED)
     execution.executions=*cache.object(execution_name);
}
