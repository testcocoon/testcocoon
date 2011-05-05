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

#ifndef CSMES_BACKGROUND_COMPUTATION_THREAD_H
#define CSMES_BACKGROUND_COMPUTATION_THREAD_H
#include "libinstrumentationpdef.h"
#include "csmesexecution.h"
#include "csmes.h"
#include <QThread>
#include <QStringList>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
class CSMesUndoRedoFramework ;
class CSMes ;
#include <QModelIndex> 
class CSMesBackgroundComputations ;

class CSMesBackgroundComputation : public QThread
{
   Q_OBJECT

   public:
      CSMesBackgroundComputation(CSMesBackgroundComputations *t);
      virtual ~CSMesBackgroundComputation();

    void setCSMes(const CSMes *);

      void pause();
      void resume();
   signals:
    void explanation(QString ,QString ,QList<int> ,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,QString );
    void statisticExecution(QStringList ,QStringList,bool execution_analysis,int ,Instrumentation::coverage_method_t ,CSMes::comparaison_mode_t, int ,int ) ;

   protected:
      void run();

   private:
      QMutex csmes_mutex;
      int pause_count;
      bool abort_operation;
      CSMesBackgroundComputations *pool_p;
      const CSMes *csmes_p;
}; 

#endif
