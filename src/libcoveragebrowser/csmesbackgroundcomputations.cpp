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

#include "csmesbackgroundcomputations.h"
#include "csmesbackgroundcomputation.h"
#include "csmesstatistic.h"
#include "csmesundoredoframework.h"
#include <QMutexLocker>
#include <QThread>

CSMesBackgroundComputations & CSMesBackgroundComputations::GetObject()
{
   static CSMesBackgroundComputations instance;
   return instance ;
}

CSMesBackgroundComputations::CSMesBackgroundComputations(QObject *parent) : QObject(parent)
{
   qRegisterMetaType<QModelIndex>("QModelIndex");
   qRegisterMetaType<QStringList>("QStringList");
   qRegisterMetaType<Instrumentation::coverage_method_t>("Instrumentation::coverage_method_t");

   _number_of_threads=QThread::idealThreadCount();
   if (_number_of_threads<0)
     _number_of_threads=1;
   for (int i=0;i<_number_of_threads;i++)
     _threads.append(new CSMesBackgroundComputation(this));

    for (QList<CSMesBackgroundComputation*>::Iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
    {
      connect( *itThread,
          SIGNAL(statisticExecution(QStringList ,QStringList , bool ,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t,int ,int )),
          this,
          SLOT(statisticExecutionSlot(QStringList ,QStringList , bool ,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t,int ,int )));

      connect( *itThread,
          SIGNAL(explanation (QString ,QString,QList<int> ,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,QString )),
          this,
          SLOT(explanationSlot (QString,QString ,QList<int> ,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,QString )));
    }
}


CSMesBackgroundComputations::~CSMesBackgroundComputations()
{
   stop(true);
}

void CSMesBackgroundComputations::explanationSlot (QString module,QString source,QList<int> lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit,QString exp)
{
  emit explanation(module,source,lines_indexs,source_type,coverage_level,method,executed_by_limit,exp);
}

void CSMesBackgroundComputations::statisticExecutionSlot(QStringList mes,QStringList comparaison , bool execution_analysis,
    int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t comparaison_mode,int nb_tested,int nb_untested)
{
  emit statisticExecution(mes,comparaison , execution_analysis, coverage_level,method,comparaison_mode,nb_tested,nb_untested) ;
}


void CSMesBackgroundComputations::pause()
{
  for (QList<CSMesBackgroundComputation*>::Iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
    (*itThread)->pause();
}

void CSMesBackgroundComputations::resume()
{
  for (QList<CSMesBackgroundComputation*>::Iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
    (*itThread)->resume();
}


void CSMesBackgroundComputations::startOneThread()
{
  for (QList<CSMesBackgroundComputation*>::const_iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
  {
    if (!(*itThread)->isRunning())
    {
      (*itThread)->start(QThread::LowPriority);
      return;
    }
  }
}

bool CSMesBackgroundComputations::isAllRunning() const
{
  for (QList<CSMesBackgroundComputation*>::const_iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
  {
    if (!(*itThread)->isRunning())
      return false;
  }
  return true;
}

bool CSMesBackgroundComputations::isRunning() const
{
  for (QList<CSMesBackgroundComputation*>::const_iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
  {
    if ((*itThread)->isRunning())
      return true;
  }
  return false;
}

void CSMesBackgroundComputations::setCSMes(const CSMes *c)
{
  pause();
  stop(true);
  for (QList<CSMesBackgroundComputation*>::Iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
    (*itThread)->setCSMes(c);
  resume();
}

void CSMesBackgroundComputations::stop(bool stop_thread)
{
  {
    QMutexLocker locker(&_mutex);
    if (!isRunning())
      return ;
    pause();
    for (QQueue<data_t>::iterator it=_datas.begin();it!=_datas.end();++it)
    {
      (*it).type=data_t::NOP;
    }
  }
  if (stop_thread)
  {
     data_t end_thread;
     end_thread.type=data_t::END_THREAD;
     for (int i=0;i<_number_of_threads;i++)
        enqueueRequest(end_thread);
     for (QList<CSMesBackgroundComputation*>::Iterator itThread=_threads.begin();itThread!=_threads.end();++itThread)
     {
        if (!(*itThread)->wait(250))
           enqueueRequest(end_thread);
     }
  }
  {
    QMutexLocker locker(&_mutex);
    _datas.clear();
  }
  resume();
}

void CSMesBackgroundComputations::cancelStatisticExecution(const QStringList &mes,const QStringList &comparaisons,bool test_count_mode,int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t comparaison_mode)
{
   data_t data;

   data.type=data_t::STATISTIC_MEASUREMENTS;
   data.measurements=mes;
   data.comparaisons=comparaisons;
   data.coverage_level=coverage_level;
   data.method=method;
   data.test_count_mode=test_count_mode;
   data.comparaison_mode=comparaison_mode;

  cancelRequest(data);
}

void CSMesBackgroundComputations::calculateStatisticExecution(const QStringList &mes,const QStringList &comparaisons,bool test_count_mode,int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t comparaison_mode)
{
   data_t data;

   data.type=data_t::STATISTIC_MEASUREMENTS;
   data.measurements=mes;
   data.comparaisons=comparaisons;
   data.coverage_level=coverage_level;
   data.method=method;
   data.test_count_mode=test_count_mode;
   data.comparaison_mode=comparaison_mode;

  enqueueRequest(data);
}

void CSMesBackgroundComputations::calculateExplanation(const QString &module,const QString &source,const QList<int> &lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit)
{
  data_t data;

  data.type              = data_t::EXPLANATION;
  data.module            = module;
  data.source            = source;
  data.lines_indexes     = lines_indexs;
  data.method            = method;
  data.source_type       = source_type;
  data.coverage_level    = coverage_level;
  data.executed_by_limit = executed_by_limit;

  enqueueRequest(data);
}

bool CSMesBackgroundComputations::dequeueRequest(CSMesBackgroundComputations::data_t &data)
{
  QMutexLocker locker(&_mutex);
  bool not_empty=!_datas.isEmpty();
  if ( not_empty )
     data = _datas.dequeue();
  return not_empty;
}

void CSMesBackgroundComputations::cancelRequest(const data_t &data)
{
  QMutexLocker locker(&_mutex);
  QQueue<data_t>::Iterator itInsertBefore=_datas.begin();

  for (QQueue<data_t>::Iterator it=_datas.begin(); it!=_datas.end();++it)
  {
     const data_t &data_it=*it;
     if (data_it==data)
        (*it).type=data_t::NOP;
  }
}

void CSMesBackgroundComputations::enqueueRequest(const data_t &data)
{
  QMutexLocker locker(&_mutex);
  QQueue<data_t>::Iterator itInsertBefore=_datas.begin();

  for (QQueue<data_t>::Iterator it=_datas.begin(); it!=_datas.end();++it)
  {
     const data_t &data_it=*it;
     if ( data_it < data )
     {
        itInsertBefore=it;
        itInsertBefore++;
     }
     if (data_it==data)
        (*it).type=data_t::NOP;
  }
  _datas.insert(itInsertBefore,data);


  if (!isAllRunning())
    startOneThread();

  _condition.wakeOne();
}

void CSMesBackgroundComputations::waitForRequest()
{
   QMutexLocker locker(&_mutex);
   _condition.wait(&_mutex,30000);
}
