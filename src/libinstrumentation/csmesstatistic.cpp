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

#include "libinstrumentationpdef.h"
#include "csmesstatistic.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTime>
#include "Service.h"

CSMesStatistic::CSMesStatistic() : CSMesRTF()
{
}

bool CSMesStatistic::statistic(int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  nb_tested=0;
  nb_untested=0;
  const SourceFiles sources=Sources(ALL);
  for (SourceFiles::const_iterator source_it=sources.begin();source_it!=sources.end();++source_it)
  {
    SourceFile source=*source_it;
    ModuleFile module(QString::null);
    int nb_tested_src=-1;
    int nb_untested_src=-1;
    if (!statistic(module,source,coverage_level,method,nb_tested_src,nb_untested_src,_instrumentations))
    {
      nb_tested=-1;
      nb_untested=-1;
      return false;
    }
    if (nb_tested==-1 || nb_untested==-1)
    {
      nb_tested=-1;
      nb_untested=-1;
      return true;
    }
    nb_tested+=nb_tested_src;
    nb_untested+=nb_untested_src;
  }
  return true;
}

bool CSMesStatistic::statisticFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  if (!findSourceModule(module,source))
  {
    nb_tested=-1;
    nb_untested=-1;
    return false;
  }

  nb_tested=0;
  nb_untested=0;

  const CSMesInstrumentations::Instrumentations &inst=_instrumentations.modules[module].sources[source].instrumentations;
  for (CSMesInstrumentations::Instrumentations::const_iterator itinst=inst.begin();itinst!=inst.end();++itinst)
  {
    if ( 
        ( (*itinst).startLinePre()>start_line 
          || ( (*itinst).startLinePre()==start_line &&  (*itinst).startColumnPre()>=start_column )
        )
        && 
        ( (*itinst).endLinePre()<end_line
          || ( (*itinst).endLinePre()==end_line &&  (*itinst).endColumnPre()<=end_column )
        )
       )
    {
      int v;
      if ( (v=(*itinst).nb_tested(coverage_level,method)) >=0 )
        nb_tested+=v;
      else
      {
        nb_tested=-1;
        nb_untested=-1;
        return true;
      }

      if ( (v=(*itinst).nb_untested(coverage_level,method)) >=0 )
        nb_untested+=v;
      else
      {
        nb_tested=-1;
        nb_untested=-1;
        return true;
      }
    }
  }
  return true;
}

bool CSMesStatistic::statisticModule(const ModuleFile &module,const SourceFile &source,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  nb_tested=0;
  nb_untested=0;
  const CSMesInstrumentations::Instrumentations &inst=_instrumentations.modules[module].sources[source].instrumentations;
  for (CSMesInstrumentations::Instrumentations::const_iterator itinst=inst.begin();itinst!=inst.end();++itinst)
  {
    int v;
    if ( (v=(*itinst).nb_tested(coverage_level,method)) >=0 )
      nb_tested+=v;
    else
    {
      nb_tested=-1;
      nb_untested=-1;
      return true;
    }

    if ( (v=(*itinst).nb_untested(coverage_level,method)) >=0 )
      nb_untested+=v;
    else
    {
      nb_tested=-1;
      nb_untested=-1;
      return true;
    }
  }
  return true;
}

bool CSMesStatistic::statistic(const ModuleFile &module_in,const SourceFile &source_in,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  SourceFile source=source_in;
  ModuleFile module=module_in;
  if (!findSourceModule(module,source))
    return false;
  return statisticModule(module,source,coverage_level,method,nb_tested,nb_untested,_instrumentations);
}

bool CSMesStatistic::statisticExecution(const QStringList &ms,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations,const bool &abort_operation) const
{ 
  bool skip=false;
  if (execution_analysis)
  {
    if (cmp.isEmpty())
      skip=true;
    for (QStringList::const_iterator it=ms.begin();it!=ms.end();++it)
    {
      if (cmp.contains(*it))
      {
        skip=true;
        break;
      }
    }
  }

  if (skip)
  {
    nb_tested=-1;
    nb_untested=-1;
    return false;
  }
  CSMesInstrumentations __instrumentations ;
  copyInstrumentation(__instrumentations,_instrumentations);

  if (selectExecutionsComparaison(__instrumentations,ms,cmp,execution_analysis,method,abort_operation))
    return statistic(coverage_level,method,nb_tested,nb_untested,__instrumentations);
  else
  {
    nb_tested=-1;
    nb_untested=-1;
    return false;
  }
}

bool CSMesStatistic::statisticFunctionsExecution(const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  nb_tested.clear();
  nb_untested.clear();
  QStringList selected_measurements=selectedExecutions();
  bool is_test_count_mode_selected=isTestCountModeSelected();

  CSMesInstrumentations __instrumentations ;
  copyInstrumentation(__instrumentations,_instrumentations);
  selectExecutionsComparaison(__instrumentations,ms,QStringList(),is_test_count_mode_selected,method,false);
  const SourceFiles sources=Sources(ALL);

  for (SourceFiles::const_iterator itsrc=sources.begin();itsrc!=sources.end();++itsrc)
  {

    QVector<FunctionInfo> FunctionsInfo = FunctionInfoSource(QString(),*itsrc);
    for (QVector<FunctionInfo>::const_iterator itfct=FunctionsInfo.begin();itfct!=FunctionsInfo.end();++itfct)
    {
      int tested=-1;
      int untested=-1;
      if (statisticFunctionPre(
            QString(),
            *itsrc,
            (*itfct).startLinePre() ,
            (*itfct).startColumnPre() ,
            (*itfct).endLinePre() ,
            (*itfct).endColumnPre() ,
            coverage_level,
            method,
            tested,
            untested,
            __instrumentations))
      {
        CSMesFunctionInfo::functionskey_t f;
        f.method=(*itfct);
        f.source=(*itsrc);

        nb_tested[f]   = tested;
        nb_untested[f] = untested;
      }
    }
  }

  return true;
}

bool CSMesStatistic::statisticSourcesExecution(const SourceFiles &sources,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<QString,int> &nb_tested,QHash<QString,int> &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  bool res=true;
  nb_tested.clear();
  nb_untested.clear();
  QStringList selected_measurements=selectedExecutions();
  bool is_test_count_mode_selected=isTestCountModeSelected();

  CSMesInstrumentations __instrumentations ;
  copyInstrumentation(__instrumentations,_instrumentations);
  selectExecutionsComparaison(__instrumentations,ms,QStringList(),is_test_count_mode_selected,method,false);
  for(SourceFiles::const_iterator it=sources.begin();res && it!=sources.end();++it)
  {
    SourceFile source=*it;
    ModuleFile module("");
    if (!findSourceModule(module,source))
      res=false;
    else
    {
      nb_tested[*it]=-1;
      nb_untested[*it]=-1;
      res = res && statisticModule(module,source,coverage_level,method,nb_tested[*it],nb_untested[*it],__instrumentations);
    }
  }

  return true;
}

bool CSMesStatistic::statisticModuleExecution(ModuleFile module,SourceFile source,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const
{
  if (!findSourceModule(module,source))
    return false;
  bool is_test_count_mode_selected=isTestCountModeSelected();
  QStringList selected_measurements=selectedExecutions();
  QStringList selected_comparaison=selectedComparaison();

  CSMesInstrumentations __instrumentations ;
  copyInstrumentation(__instrumentations,_instrumentations);
  selectExecutionsComparaison(__instrumentations,ms,QStringList(),is_test_count_mode_selected,method,false);
  bool res=statisticModule(module,source,coverage_level,method,nb_tested,nb_untested,__instrumentations);

  return res;
}



QString CSMesStatistic::printStat(int nb_tested,int nb_untested) 
{
  if (nb_tested+nb_untested>0)
  {
    double stat=static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested)*100.0;
    return printStat(stat)+" ("+QString::number(nb_tested)+"/"+QString::number(nb_tested+nb_untested)+")";
  }
  else
    return "--";
}

QString CSMesStatistic::printStat(double v) 
{
  if (v<-0.1)
    return "--";
  if (v<10)
    return "  "+QString::number(v,'f',3)+"%";
  if (v<100)
    return " "+QString::number(v,'f',3)+"%";
  return QString::number(v,'f',3)+"%";
}



bool CSMesStatistic::statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const bool &abort_operation) const 
{
   return statisticExecution(mes,cmp,execution_analysis,coverage_level,method,nb_tested,nb_untested,instrumentations,abort_operation);
}


