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


CSMesBackgroundComputation::CSMesBackgroundComputation(CSMesBackgroundComputations *p) : QThread(p)
{
  pool_p=p;
   qRegisterMetaType<QModelIndex>("QModelIndex");
   qRegisterMetaType<QStringList>("QStringList");
   qRegisterMetaType<QList<int> >("QList<int>");
   qRegisterMetaType<Instrumentation::coverage_method_t>("Instrumentation::coverage_method_t");
   qRegisterMetaType<CSMes::source_type_t>("CSMes::source_type_t");
   qRegisterMetaType<CSMes::comparaison_mode_t>("CSMes::comparaison_mode_t");
   pause_count=0;
   abort_operation=false;
}


CSMesBackgroundComputation::~CSMesBackgroundComputation()
{
}

void CSMesBackgroundComputation::pause()
{
  ASSERT(pause_count>=0);
  if (pause_count==0)
    abort_operation=true;
  pause_count++;
}

void CSMesBackgroundComputation::resume()
{
  pause_count--;
  if (pause_count==0)
    abort_operation=false;
  ASSERT(pause_count>=0);
}

void CSMesBackgroundComputation::setCSMes(const CSMes *c)
{
  QMutexLocker locker_csmes(&csmes_mutex);
  csmes_p=c;
}

void CSMesBackgroundComputation::run()
{
  for(;;)
  {
    CSMesBackgroundComputations::data_t data;
    data.type=CSMesBackgroundComputations::data_t::NOP;

    while ( pool_p->dequeueRequest(data) )
    {
      switch (data.type)
      {
         case CSMesBackgroundComputations::data_t::NOP:
          break;
         case CSMesBackgroundComputations::data_t::END_THREAD:
          return ;
         case CSMesBackgroundComputations::data_t::STATISTIC_MEASUREMENTS:
          {
            int nb_tested=-1;
            int nb_untested=-1;
            {
              QMutexLocker locker(&csmes_mutex);

              if (csmes_p && !abort_operation)
                csmes_p->statisticExecution(data.measurements,
                    data.comparaisons,
                    data.test_count_mode,
                    data.coverage_level,
                    data.method,
                    data.comparaison_mode,
                    nb_tested,nb_untested,abort_operation);
            }

            bool emit_data = data.type!=CSMesBackgroundComputations::data_t::NOP;
            if (abort_operation)
            {
               pool_p->enqueueRequest(data);
               emit_data=false;
            }
            if (emit_data)
              emit statisticExecution(data.measurements,
                  data.comparaisons,
                  data.test_count_mode,
                  data.coverage_level,
                  data.method,
                  data.comparaison_mode,
                  nb_tested,nb_untested);
            else
              msleep(250);
          }
          break;
         case CSMesBackgroundComputations::data_t::EXPLANATION:
          {
            QString expl;
            {
              QMutexLocker locker(&csmes_mutex);

              if (csmes_p && !abort_operation)
                expl = csmes_p->explanation (data.module,data.source,data.lines_indexes,data.source_type,data.coverage_level,data.method,data.executed_by_limit) ;
            }

            bool emit_data = data.type!=CSMesBackgroundComputations::data_t::NOP;
            if (abort_operation)
            {
               pool_p->enqueueRequest(data);
               emit_data=false;
            }
            if (emit_data)
              emit explanation(data.module,data.source,data.lines_indexes,data.source_type,data.coverage_level,data.method,data.executed_by_limit,expl);
            else
              msleep(250);
          }
          break;
      }
    }
 
    pool_p->waitForRequest();
  }
}
