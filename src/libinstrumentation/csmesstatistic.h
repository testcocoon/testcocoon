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

#ifndef CSMES_STATISTIC_H
#define CSMES_STATISTIC_H
#include "csmesrtf.h"
#include <QVector>
#include <QVariant>
#include  <QTextStream>

class CSMesStatistic : public CSMesRTF
{
  protected:
    CSMesStatistic();
  protected:
    bool statisticFunctionsExecution(const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_untested) const
    {
      return statisticFunctionsExecution(ms,coverage_level,method,nb_tested,nb_untested,instrumentations) ;
    }
    bool statistic(const QString &mod,const QString &src,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
    {
      return statistic(mod,src,coverage_level,method,nb_tested,nb_untested,instrumentations);
    }
    bool statistic(int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
    {
      return statistic(coverage_level,method,nb_tested,nb_untested,instrumentations);
    }
    bool statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const bool &abort_operation) const ;
    static QString printStat(double v) ;
    static QString printStat(int nb_tested,int nb_untested) ;
    bool statisticModuleExecution(ModuleFile module,SourceFile source,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
    {
      return statisticModuleExecution(module,source,ms,coverage_level,method,nb_tested,nb_untested,instrumentations) ;
    }
    bool statisticFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
    {
      return statisticFunctionPre(module,source,start_line,start_column,end_line,end_column,coverage_level,method,nb_tested,nb_untested,instrumentations) ;
    }
  protected:
    bool statistic(const ModuleFile &mod,const SourceFile &src,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statistic(int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations,const bool &abort_operation) const ;
    bool statisticModuleExecution(ModuleFile module,SourceFile source,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statisticFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statisticModule(const ModuleFile &module,const SourceFile &source,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statisticSourcesExecution(const SourceFiles &sources,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<QString,int> &nb_tested,QHash<QString,int> &nb_untested,const CSMesInstrumentations &_instrumentations) const;
    bool statisticSourcesExecution(const SourceFiles &sources,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<QString,int> &nb_tested,QHash<QString,int> &nb_untested) const
    {
      return statisticSourcesExecution(sources,ms,coverage_level,method,nb_tested,nb_untested,instrumentations) ;
    }
    bool statisticFunctionsExecution(const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_untested,const CSMesInstrumentations &_instrumentations) const;
} ;
#endif
