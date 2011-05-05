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

#ifndef CSMES_BACKGROUND_COMPUTATION_THREADS_H
#define CSMES_BACKGROUND_COMPUTATION_THREADS_H
#include "libinstrumentationpdef.h"
#include "csmes.h"
#include "csmesexecution.h"
#include <QThread>
#include <QStringList>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
class CSMesUndoRedoFramework ;
class CSMes ;
class CSMesBackgroundComputation;
#include <QModelIndex>

class CSMesBackgroundComputations : public QObject
{
   Q_OBJECT

      CSMesBackgroundComputations(QObject *parent = 0);
   public:
      class data_t
      {
      public:
         enum type_t
         {
            END_THREAD,
            EXPLANATION,
            STATISTIC_MEASUREMENTS,
            NOP,
         };
        QList<int> lines_indexes;
         type_t  type;
         QStringList measurements;
         QStringList comparaisons;
         bool test_count_mode;
         QString module;
         QString source;
         CSMes::source_type_t source_type;
         Instrumentation::coverage_method_t method;
         CSMes::comparaison_mode_t comparaison_mode;
         int executed_by_limit;
         int coverage_level;
         bool operator==(const data_t &s) const
         {
           if (s.type==END_THREAD || type==END_THREAD)
             return false; // Multiple END_THREAD allowed
           if (s.type==EXPLANATION && type==EXPLANATION)
             return true; // ONly one EXPLANATION in the QUEUE
           return (   (s.type==type)
                      && (s.measurements==measurements)
                      && (s.comparaisons==comparaisons)
                      && (s.coverage_level==coverage_level)
                      && (s.comparaison_mode==comparaison_mode)
                      && (s.test_count_mode==test_count_mode)
                      && (s.method==method) );
         }
         bool operator<(const data_t &s) const
         {
           if (s.type!=type)
              return type<s.type;
           if (s.type==STATISTIC_MEASUREMENTS)
              return measurements.count() < s.measurements.count() ;
           return false;
         }
      };

      class Pauser
      {
         public:
            Pauser () { GetObject().pause() ; }
            virtual ~Pauser () {  GetObject().resume() ; }
      };


      static CSMesBackgroundComputations & GetObject() ;
      virtual ~CSMesBackgroundComputations();
      int numberOfThreads() const { return _number_of_threads; }

    void setCSMes(const CSMes *);
    void stop(bool stop_thread);
    void calculateStatisticExecution(const QStringList &mes,const QStringList &comparaison,bool test_count_mode,int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t comparaison_mode) ;
    void cancelStatisticExecution(const QStringList &mes,const QStringList &comparaison,bool test_count_mode,int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t comparaison_mode) ;
    void calculateExplanation(const QString &module,const QString &source, const QList<int>& lines,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit);
    bool isRunning() const;
    bool isAllRunning() const;

    bool dequeueRequest(data_t &data);
    void cancelRequest(const data_t &data);
    void enqueueRequest(const data_t &data);
    void waitForRequest();

   signals:
    void statisticExecution(const QStringList &,const QStringList &comparaison,bool execution_analysis,int ,Instrumentation::coverage_method_t , CSMes::comparaison_mode_t, int ,int ) ;
    void explanation (const QString &module,const QString &source,const QList<int> &lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit,const QString &exp);


  private slots:
    void statisticExecutionSlot(QStringList ,QStringList comparaison,bool execution_analysis,int ,Instrumentation::coverage_method_t , CSMes::comparaison_mode_t,int ,int ) ;
    void explanationSlot (QString ,QString ,QList<int> ,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int, QString );

   private:
    void startOneThread();
      void pause();
      void resume();
      QQueue<data_t> _datas;
      int _number_of_threads;
      QMutex _mutex;
      QWaitCondition _condition;
      QList<CSMesBackgroundComputation*> _threads;
};

#endif
