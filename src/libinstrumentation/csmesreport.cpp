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

#include "libinstrumentationpdef.h"
#include "csmesreport.h"
#include "xmlwriter.h"
#include "htmlwriter.h"
#include "csmesinstrumentation.h"
#include "csmes.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>
#include <QTime>
#include <QBuffer>
#include <QXmlStreamWriter>
#include <QSet>
#include "Service.h"

CSMesReport::CSMesReport() : CSMesStatistic()
{
}




bool CSMesReport::tableLessThanName(const QVector<CSMesReport::table_data_t> &e1,const QVector<CSMesReport::table_data_t> &e2)
{
  ASSERT(e1.first().type()==e2.first().type());
  if (e1.first().type()==TABLE_FORMAT_FUNCTION)
    return e1.first().func().method.getScopedName() < e2.first().func().method.getScopedName() ;
  else
    return e1.first().data().toString().toLower() < e2.first().data().toString().toLower() ;
}

bool CSMesReport::tableLessThanCoverage(const QVector<CSMesReport::table_data_t> &e1,const QVector<CSMesReport::table_data_t> &e2)
{
  bool ok;
  double e1Float = e1.last().data().toDouble(&ok) ;
  if (!ok)
    return e1.last().data().toString() < e2.last().data().toString() ;
  double e2Float = e2.last().data().toDouble(&ok) ;
  if (!ok)
    return e1.last().data().toString() < e2.last().data().toString() ;
  if (e1Float==e2Float)
    return tableLessThanName(e1,e2);
  else
    return e1Float > e2Float;
}

void CSMesReport::formatHtmlFunctionKey(Writer &stream,const CSMesFunctionInfo::functionskey_t &f) const
{
  stream.begin(Writer::FunctionInfo);
  stream.begin(Writer::HtmlMethodScopedName);
  stream <<  f.method.getScopedName();
  stream.end(Writer::HtmlMethodScopedName);
  stream.writeLine();
  stream.begin(Writer::FunctionPrototype);
  stream << f.method.getPrototype();
  stream.end(Writer::FunctionPrototype);
  stream.writeLine();
  stream.begin(Writer::HtmlMethodRelativeSourceFileName);
  stream << relativeSourceName(f.source);
  stream.end(Writer::HtmlMethodRelativeSourceFileName);
  stream.end(Writer::FunctionInfo);
}

QString CSMesReport::exportStatisticFunction(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight,bool sort_coverage) const
{
  QString result ;
  table.clear();
  int i,j;
  const ExecutionNames executions=selectedExecutions();
  if (executions.isEmpty())
    return QObject::tr("No executions selected");

  const QList<CSMesFunctionInfo::functionskey_t> functions= Functions();
  int nb_functions=functions.count();
  int max_levels= (coverage_level>max_number_of_levels)?max_number_of_levels:coverage_level;
  float level_step= static_cast<float>(coverage_level)/static_cast<float>(max_number_of_levels);
  if (coverage_level==1)
    max_levels=1;
  else if (all_levels)
    ;
  else if (first_level)
    max_levels=2;
  else
    max_levels=1;

  table.resize(nb_functions+1);
  for (i=0;i<table.size();i++)
  {
    table[i].resize(max_levels+1);
  }

  table[0][0].setData(QObject::tr("Method"), TABLE_FORMAT_TITLE);
  for (j=0;j<nb_functions;j++)
    table[j+1][0].setData(functions[j], TABLE_FORMAT_FUNCTION );

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    table[0][i+1].setData(level, TABLE_FORMAT_LEVEL);
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    // module statistic
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_list;
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_untested_list;
    if ( statisticFunctionsExecution(executions,level,method,nb_tested_list,nb_untested_list) )
    {
      for (j=0;j<nb_functions;j++)
      {
        const CSMesFunctionInfo::functionskey_t &f=functions.at(j);
        if (nb_tested_list.contains(f) && nb_untested_list.contains(f) )
          table[j+1][i+1].setData( level,StatValue(nb_tested_list[f],nb_untested_list[f]),  TABLE_FORMAT_STATISTIC_LEVEL );
        else
          table[j+1][i+1].setData ( level, "--", TABLE_FORMAT_STATISTIC_LEVEL );
      }
    }
  }

  // Filter
  for (int i=table.size()-1 ; i>=0 ;i--)
  {
    bool ok;
    double vFloat = table[i].last().data().toDouble(&ok) ;
    if (ok)
    {
      if ( (vFloat>=exclude_coverage_low) && (vFloat<=exclude_coverage_hight) )
      {
        table.remove(i);
      }
    }
  }

  // Sort
  CSMesReport::table_t::Iterator  begin_table=table.begin();
  ++begin_table;
  if (sort_coverage)
    qSort(begin_table, table.end(), tableLessThanCoverage);
  else
    qSort(begin_table, table.end(), tableLessThanName);

  return result;
}

QString CSMesReport::exportStatisticGlobalCoverage(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level, int max_number_of_levels) const
{
  QString result ;
  table.clear();
  int i;
  const ExecutionNames executions=selectedExecutions();
  if (executions.isEmpty())
    return QObject::tr("No executions selected");

  int max_levels= (coverage_level>max_number_of_levels)?max_number_of_levels:coverage_level;
  float level_step= static_cast<float>(coverage_level)/static_cast<float>(max_number_of_levels);
  if (coverage_level==1)
    max_levels=1;
  else if (all_levels)
    ;
  else if (first_level)
    max_levels=2;
  else
    max_levels=1;
  table.resize(max_levels+1);

  for (i=0;i<table.size();i++)
    table[i].resize(1+1);

  table[0][0].setData (QObject::tr("Level"), TABLE_FORMAT_TITLE );
  table[0][1].setData (QObject::tr("Coverage"), TABLE_FORMAT_TITLE );

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    table[i+1][0].setData(level, TABLE_FORMAT_LEVEL );
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    int nb_tested,nb_untested;
    if ( statisticExecution(executions,ExecutionNames(),false,level,method,nb_tested,nb_untested,false) )
    {
      table[i+1][1].setData ( level, StatValue(nb_tested,nb_untested), TABLE_FORMAT_STATISTIC_LEVEL );
    }
  }
  return result;
}

QString CSMesReport::exportStatisticSources(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight,bool sort_coverage) const
{
  QString result ;
  table.clear();
  int i,j;
  const ExecutionNames executions=selectedExecutions();
  if (executions.isEmpty())
    return QObject::tr("No executions selected");

  const SourceFiles sources= Sources(NON_EMPTY);
  int nb_sources=sources.count();
  int max_levels= (coverage_level>max_number_of_levels)?max_number_of_levels:coverage_level;
  float level_step= static_cast<float>(coverage_level)/static_cast<float>(max_number_of_levels);
  if (coverage_level==1)
    max_levels=1;
  else if (all_levels)
    ;
  else if (first_level)
    max_levels=2;
  else
    max_levels=1;
  table.resize(nb_sources+1);
  for (i=0;i<table.size();i++)
  {
    table[i].resize(max_levels+1);
  }

  table[0][0].setData(QObject::tr("Source"), TABLE_FORMAT_TITLE);
  for (j=0;j<nb_sources;j++)
  {
    table[j+1][0].setData(sources[j], TABLE_FORMAT_SOURCE);
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    table[0][i+1].setData(level, TABLE_FORMAT_LEVEL);
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    // module statistic
    QHash<ExecutionName,int> nb_tested_list;
    QHash<ExecutionName,int> nb_untested_list;
    if ( statisticSourcesExecution(sources,executions,level,method,nb_tested_list,nb_untested_list) )
    {
      for (j=0;j<nb_sources;j++)
      {
        const QString &f=sources.at(j);
        if (nb_tested_list.contains(f) && nb_untested_list.contains(f) )
          table[j+1][i+1].setData ( level, StatValue(nb_tested_list[f],nb_untested_list[f]),TABLE_FORMAT_STATISTIC_LEVEL);
        else
          table[j+1][i+1].setData ( level, "--", TABLE_FORMAT_STATISTIC_LEVEL );
      }
    }
  }

  // Filter
  for (int i=table.size()-1 ; i>=0 ;i--)
  {
    bool ok;
    double vFloat = table[i].last().data().toDouble(&ok) ;
    if (ok)
    {
      if ( (vFloat>=exclude_coverage_low) && (vFloat<=exclude_coverage_hight) )
      {
        table.remove(i);
      }
    }
  }

  // Sort
  CSMesReport::table_t::Iterator  begin_table=table.begin();
  ++begin_table;
  if (sort_coverage)
    qSort(begin_table, table.end(), tableLessThanCoverage);
  else
    qSort(begin_table, table.end(), tableLessThanName);

  /*
  for (j=1;j<table.size();j++)
  {
    QString v=table[j][0].data.toString();
    table[j][0]="<TT>"
      + Qt::escape(relativeSourceName(v))
      + "<BR>"
      + "<FONT size=\"1\">"
      + Qt::escape(v)
      + "</FONT>"
      + "</TT>";
  }
  */

  return result;
}

QString CSMesReport::exportStatisticExecutions(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight,bool sort_coverage) const
{
  QString result ;
  table.clear();
  int i,j;
  const ExecutionNames executions=selectedExecutions();
  if (executions.isEmpty())
    return QObject::tr("No executions selected");

  int nb_executions=executions.count();
  int max_levels= (coverage_level>max_number_of_levels)?max_number_of_levels:coverage_level;
  float level_step= static_cast<float>(coverage_level)/static_cast<float>(max_number_of_levels);
  if (coverage_level==1)
    max_levels=1;
  else if (all_levels)
    ;
  else if (first_level)
    max_levels=2;
  else
    max_levels=1;
  table.resize(nb_executions+1);
  for (i=0;i<table.size();i++)
  {
    table[i].resize(max_levels+1);
  }

  table[0][0].setData(QObject::tr("Execution"), TABLE_FORMAT_TITLE);
  for (j=0;j<nb_executions;j++)
  {
    table[j+1][0].setData(executions[j], TABLE_FORMAT_EXECUTION);
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    table[0][i+1].setData(level, TABLE_FORMAT_LEVEL);
  }

  for (i=0;i<max_levels;i++)
  {
    int level;
    if (i==max_levels-1)
      level=(coverage_level);
    else
      level=static_cast<int>(i*level_step+1);

    // module statistic
    int nb_tested;
    int nb_untested;
    for (j=0;j<nb_executions;j++)
    {
      const ExecutionName &e=executions.at(j);
      ExecutionNames ee;
      ee+=e;
      if ( statisticExecution(ee,ExecutionNames(),false,level,method,nb_tested,nb_untested,false) )
      {
        table[j+1][i+1].setData ( level, StatValue(nb_tested,nb_untested), TABLE_FORMAT_STATISTIC_LEVEL);
      }
    }
  }

  // Filter
  for (int i=table.size()-1 ; i>=0 ;i--)
  {
    bool ok;
    double vFloat = table[i].last().data().toDouble(&ok) ;
    if (ok)
    {
      if ( (vFloat>=exclude_coverage_low) && (vFloat<=exclude_coverage_hight) )
      {
        table.remove(i);
      }
    }
  }

  // Sort
  CSMesReport::table_t::Iterator  begin_table=table.begin();
  ++begin_table;
  if (sort_coverage)
    qSort(begin_table, table.end(), tableLessThanCoverage);
  else
    qSort(begin_table, table.end(), tableLessThanName);

#if 0
  for (j=1;j<table.size();j++)
  {
    QString v=table[j][0].toString();
    table[j][0]="<TT>"
      + Qt::escape(v)
      + "</TT>";
  }
#endif

  return result;
}

QString CSMesReport::exportStatisticFunction(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method) const
{
  QString result ;
  table.clear();
  int i,j;
  int nb_tested,nb_untested;
  const ExecutionNames executions=selectedExecutions();
  int nb_executions=executions.count();
  if (executions.isEmpty())
    return QObject::tr("No executions selected");

  const QList<CSMesFunctionInfo::functionskey_t> functions= Functions();
  int nb_functions=functions.count();
  table.resize(nb_functions+3);
  for (i=0;i<table.size();i++)
    table[i].resize(nb_executions+4);

  table[0][0].setData(QObject::tr("Method"), TABLE_FORMAT_TITLE);
  table[1][0].setData(QObject::tr("Status"), TABLE_FORMAT_TITLE);
  table[0][1].setData(QObject::tr("Prototype"), TABLE_FORMAT_TITLE);
  table[0][2].setData(QObject::tr("Source"), TABLE_FORMAT_TITLE);
  for (j=0;j<nb_functions;j++)
  {
    table[j+2][0].setData(functions[j].method.getScopedName(), TABLE_FORMAT_SCOPED_NAME);
    table[j+2][1].setData(functions[j].method.getPrototype(), TABLE_FORMAT_PROTOTYPE);
    table[j+2][2].setData(relativeSourceName(functions[j].source), TABLE_FORMAT_SOURCE_RELATIVE);
  }
  table[nb_functions+2][0].setData(QObject::tr("Total"), TABLE_FORMAT_TITLE);

  for (i=0;i<nb_executions;i++)
  {
    table[0][i+3].setData(executions[i], TABLE_FORMAT_EXECUTION);
  }
  table[0][nb_executions+3].setData(QObject::tr("Total - level %1").arg(coverage_level), TABLE_FORMAT_TITLE);

  for (i=0;i<nb_executions;i++)
  {
    printStatus(QObject::tr("Exporting Statistics..."),i/(3+nb_executions));
    const QStringList execStr = executionsStatusStr();
    // module statistic
    ExecutionNames execs;
    execs+=executions.at(i);
    table [1][i+3].setData( execStr[getExecutionStatus(executions.at(i))], TABLE_FORMAT_EXECUTION_STATUS);
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_list;
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_untested_list;
    if ( statisticFunctionsExecution(execs,coverage_level,method,nb_tested_list,nb_untested_list,instrumentations) )
    {
      for (j=0;j<nb_functions;j++)
      {
        const CSMesFunctionInfo::functionskey_t &f=functions.at(j);
        if (nb_tested_list.contains(f) && nb_untested_list.contains(f) )
          table[j+2][i+3].setData( coverage_level, StatValue(nb_tested_list[f],nb_untested_list[f]),TABLE_FORMAT_STATISTIC_LEVEL);
        else
          table[j+2][i+3].setData( coverage_level, ("--"),TABLE_FORMAT_STATISTIC_LEVEL);
      }
    }
    nb_tested_list.clear();
    nb_untested_list.clear();

    // statistics for each execution
    if (statisticExecution(execs,ExecutionNames(),false,coverage_level,method,nb_tested,nb_untested,instrumentations,false))
      table[nb_functions+2][i+3].setData ( coverage_level,StatValue(nb_tested,nb_untested), TABLE_FORMAT_STATISTIC_LEVEL);
    else
      table[nb_functions+2][i+3].setData ( coverage_level, ("--"), TABLE_FORMAT_STATISTIC_LEVEL);

    // statistics for each module
    printStatus(QObject::tr("Exporting Statistics..."),(nb_executions)/(3+nb_executions));
    if ( statisticFunctionsExecution(executions,coverage_level,method,nb_tested_list,nb_untested_list,instrumentations) )
    {
      for (j=0;j<nb_functions;j++)
      {
        const CSMesFunctionInfo::functionskey_t &f=functions.at(j);
        if (nb_tested_list.contains(f) && nb_untested_list.contains(f) )
          table[j+2][nb_executions+3].setData( coverage_level, StatValue(nb_tested_list[f],nb_untested_list[f]), TABLE_FORMAT_STATISTIC_LEVEL);
        else
          table[j+2][nb_executions+3].setData( coverage_level, ("--"), TABLE_FORMAT_STATISTIC_LEVEL);
      }
    }
    printStatus(QObject::tr("Exporting Statistics..."),(nb_executions+2)/(3+nb_executions));

    // global statistic
    if (statisticExecution(executions,ExecutionNames(),false,coverage_level,method,nb_tested,nb_untested,instrumentations,false))
      table[nb_functions+2][nb_executions+3].setData(coverage_level, StatValue(nb_tested,nb_untested),TABLE_FORMAT_STATISTIC_LEVEL);
    else
      table[nb_functions+2][nb_executions+3].setData(coverage_level, ("--"),TABLE_FORMAT_STATISTIC_LEVEL);
  }
  return result;
}


QString CSMesReport::exportHtml(Writer::section_type_t section,const CSMesReport::table_t &table,Writer &stream,float watermark_low_medium, float watermark_medium_high,bool bargraph) 
{
  int imax=table.size();
  if (imax<=0)
    return QObject::tr("empty");

  int jmax=table.at(0).size();
  if (jmax<=0)
    return QObject::tr("empty");

  stream.begin(section);
  for (int i=0;i<imax;i++)
  {
    if (i == 0)
      stream.begin(Writer::HtmlItemTitle);
    else 
    {
      stream.begin(Writer::Item);
      if (i % 2 == 0)
        stream.begin(Writer::HtmlItemOdd);
      else
        stream.begin(Writer::HtmlItemEven);
    }
    for (int j=0;j<jmax;j++)
    {
      Writer::section_type_t sec=Writer::ItemTitle;
      switch (table.at(i).at(j).type())
      {
        case CSMesReport::TABLE_FORMAT_TITLE:
          sec = Writer::ItemTitle;
          break;
        case CSMesReport::TABLE_FORMAT_TEXT:
          sec = Writer::HtmlTableFormat;
          break;
        case CSMesReport::TABLE_FORMAT_FUNCTION:
          sec = Writer::ItemFunction;
          break;
        case CSMesReport::TABLE_FORMAT_STATISTIC_LEVEL:
          sec = Writer::ItemStatistic;
          break;
        case CSMesReport::TABLE_FORMAT_SOURCE:
          sec = Writer::ItemSource;
          break;
        case CSMesReport::TABLE_FORMAT_EXECUTION:
          sec = Writer::ExecutionName;
          break;
        case CSMesReport::TABLE_FORMAT_LEVEL:
          sec = Writer::ItemTitleLevel;
          break;
        case CSMesReport::TABLE_FORMAT_SCOPED_NAME:
          sec = Writer::HtmlTableFormatScopedName;
          break;
        case CSMesReport::TABLE_FORMAT_SOURCE_RELATIVE:
          sec = Writer::HtmlTableFormatSourceRelative;
          break;
        case CSMesReport::TABLE_FORMAT_PROTOTYPE:
          sec = Writer::HtmlTableFormatPrototype;
          break;
        case CSMesReport::TABLE_FORMAT_EXECUTION_STATUS:
          sec = Writer::HtmlTableFormatExecutionStatus;
          break;
      }
      {
        WriterSection table_section(stream,sec);
        if (table.at(i).at(j).type()==CSMesReport::TABLE_FORMAT_STATISTIC_LEVEL)
        {
          stream.begin(Writer::ItemStatisticLevel);
          stream<<table.at(i).at(j).level();
          stream.end(Writer::ItemStatisticLevel);
          stream.begin(Writer::ItemStatisticValue);
        }
        switch (table.at(i).at(j).type())
        {
          case CSMesReport::TABLE_FORMAT_FUNCTION:
            {
              const CSMesFunctionInfo::functionskey_t &info=table.at(i).at(j).func();
              stream.begin(Writer::ItemFunctionScopedName);
              stream << info.method.getScopedName();
              stream.end(Writer::ItemFunctionScopedName);

              stream.begin(Writer::ItemFunctionPrototype);
              stream << info.method.getPrototype();
              stream.end(Writer::ItemFunctionPrototype);

              stream.begin(Writer::ItemFunctionSource);
              stream << info.source;
              stream.end(Writer::ItemFunctionSource);
            }

            break;
          case CSMesReport::TABLE_FORMAT_LEVEL:
            {
              const QVariant &value=table.at(i).at(j).data();
              stream.begin(Writer::HtmlLevel);
              stream<<QObject::tr("level") + " ";
              stream.end(Writer::HtmlLevel);
              stream <<  value.toString() ;
            }
            break;
          case CSMesReport::TABLE_FORMAT_STATISTIC_LEVEL:
          case CSMesReport::TABLE_FORMAT_TITLE:
          case CSMesReport::TABLE_FORMAT_TEXT:
          case CSMesReport::TABLE_FORMAT_SOURCE:
          case CSMesReport::TABLE_FORMAT_EXECUTION:
          case CSMesReport::TABLE_FORMAT_SCOPED_NAME:
          case CSMesReport::TABLE_FORMAT_SOURCE_RELATIVE:
          case CSMesReport::TABLE_FORMAT_PROTOTYPE:
          case CSMesReport::TABLE_FORMAT_EXECUTION_STATUS:
            {
              const QVariant &value=table.at(i).at(j).data();
              bool isFloat;
              double value_d = value.toDouble(&isFloat);
              QColor c=COL_WHITE;
              if (value_d<watermark_low_medium)
                c=COL_RED;
              else if (value_d<watermark_medium_high)
                c=COL_ORANGE;
              else
                c=COL_GREEN;


              if (value==QVariant())
                ;
              else if (isFloat)
              {
                WriterSection numeric(stream,Writer::HtmlStatisticValue);
                if (bargraph)
                  stream << Writer::BARGRAPH << c << value_d << COL_BLACK <<  Writer::NUMERIC ;
                stream << Writer::PERCENT << value_d << Writer::NUMERIC ;
              }
              else
              {
                stream << value.toString();
              }
            }
            break;
        }
        if (table.at(i).at(j).type()==CSMesReport::TABLE_FORMAT_STATISTIC_LEVEL)
        {
          stream.end(Writer::ItemStatisticValue);
        }
      }
    }
    if (i  == 0)
      stream.end(Writer::HtmlItemTitle);
    else 
    {
      if (i % 2 == 0)
        stream.end(Writer::HtmlItemOdd);
      else
        stream.end(Writer::HtmlItemEven);
      stream.end(Writer::Item);
    }
  }
  stream.end(section);

  return QString();
}

QString CSMesReport::exportHtmlStatisticFunction(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  table_t table;
  QString result = exportStatisticFunction(table,coverage_level,method,all_levels,first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight,  sort_coverage) ;

  if (!result.isEmpty())
    return result;

  return exportHtml(Writer::MethodStatistics,table,stream,watermark_low_medium, watermark_medium_high,bargraph);
}

QString CSMesReport::exportHtmlStatisticGlobalCoverage(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float watermark_low_medium, float watermark_medium_high,bool bargraph) const
{
  table_t table;
  QString result = exportStatisticGlobalCoverage(table,coverage_level,method,all_levels,first_level,max_number_of_levels) ;

  if (!result.isEmpty())
    return result;

  return exportHtml(Writer::GlobalStatistic,table,stream,watermark_low_medium, watermark_medium_high,bargraph);
}


QString CSMesReport::exportHtmlStatisticSources(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  table_t table;
  QString result = exportStatisticSources(table,coverage_level,method,all_levels,first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight,  sort_coverage) ;

  if (!result.isEmpty())
    return result;

  return exportHtml(Writer::SourcesStatistics,table,stream,watermark_low_medium, watermark_medium_high,bargraph);
}


QString CSMesReport::exportHtmlStatisticExecutions(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels, float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  table_t table;
  QString result = exportStatisticExecutions(table,coverage_level,method,all_levels,first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight,  sort_coverage) ;

  if (!result.isEmpty())
    return result;

  return exportHtml(Writer::ExecutionStatistics,table,stream,watermark_low_medium, watermark_medium_high,bargraph);
}

QString CSMesReport::exportHtmlCodeFragments(Writer &stream,int level,Instrumentation::coverage_method_t method,code_fragment_type_t code_fragment_type,int executed_by_limit) const
{
  SourceFiles sources=Sources(NON_EMPTY);
  bool empty_output=true;
  {
    WriterSection code_fragments(stream,Writer::CodeFragment);
    for (SourceFiles::const_iterator itsrc=sources.begin();itsrc!=sources.end();++itsrc)
    {
      bool found=false;
      SourceFile source=*itsrc;
      ModuleFile module("");
      if (findSourceModule(module,source))
      {
        int nb_found=0;
        const QVector<Instrumentation> inst=instrumentationList(module,source);
        for (int index=0;index<inst.size();index++)
        {
          bool print=false;
          switch ( inst.at(index).status(level,method) )
          {
            case Instrumentation::STATUS_PARTIALLY_VALIDATED:
            case Instrumentation::STATUS_UNVALIDATED:
              print= code_fragment_type == FRAGMENTS_UNEXECUTED;
              break;
            case Instrumentation::STATUS_MANUALLY_VALIDATED:
              print= code_fragment_type == FRAGMENTS_MANUALLY_VALIDATED;
              break;
            case Instrumentation::STATUS_VALIDATED:
              print= code_fragment_type == FRAGMENTS_EXECUTED;
              break;
            case Instrumentation::STATUS_UNKNOWN:
            case Instrumentation::STATUS_NONE:
            case Instrumentation::STATUS_HIDDEN:
              print=false;
              break;
          }
          if (print)
          {
            empty_output=false;
            if (!found)
            {
              stream.begin(Writer::CodeFragmentItem);
              {
                WriterSection sourcesec(stream,Writer::CodeFragmentItemSource);
                stream << source ;
              }
              stream.begin(Writer::CodeFragmentItemExplanation);
            }
            found=true;
            Writer::section_type_t  sec;

            if (nb_found++ % 2 == 0)
              sec=Writer::HtmlCodeFragmentItemExplanationItemOdd;
            else
              sec=Writer::HtmlCodeFragmentItemExplanationItemEven;
            {
              WriterSection explanation(stream,sec);
              {
                WriterSection explanation1(stream,Writer::CodeFragmentItemExplanationItem);
                explanationFragmentFromIndex(stream,module,source,index,CSMes::ORIGINAL,level,method,executed_by_limit,true) ;
              }
            }
          }
        }
        if (found)
        {
          stream.end(Writer::CodeFragmentItemExplanation);
          stream.end(Writer::CodeFragmentItem);
        }
      }
    }
  }
  if (empty_output)
  {
    WriterSection code_fragments(stream,Writer::HtmlCodeFragmentsEmpty);
    stream << QObject::tr("Empty") ;
  }
  return QString();
}

QString CSMesReport::exportHtmlExecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  return exportHtmlCodeFragments(stream,level,method,FRAGMENTS_EXECUTED,executed_by_limit);
}

QString CSMesReport::exportHtmlUnexecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  return exportHtmlCodeFragments(stream,level,method,FRAGMENTS_UNEXECUTED,executed_by_limit);
}

QString CSMesReport::exportHtmlManuallyValidated(Writer &stream,int level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  return exportHtmlCodeFragments(stream,level,method,FRAGMENTS_MANUALLY_VALIDATED,executed_by_limit);
}


void CSMesReport::generateHeaders(
    Writer &stream,
    const QString &title,
    const QString &icon,
    const QString &css_file,
    bool statistic_global_coverage,
    bool statistic_method,
    bool statistic_sources,
    bool statistic_executions,
    bool code_fragments,
    bool toc ,
    bool code_fragments_manually_validated,
    bool code_fragments_unexecuted,
    bool code_fragments_executed
    )
{
  stream.beginHeader();
  stream.setTitle(title);
  stream.setIconFile(icon);
  stream.setCssFile(css_file);
  stream.endHeader();
  stream.beginBody();

  if (toc)
  {
    WriterSection toc_level1(stream,Writer::HtmlTocLevel1);
    if (statistic_global_coverage
        || statistic_sources
        || statistic_method
        || statistic_executions)
    {
      stream.begin(Writer::HtmlTocLevel1Item);
      stream << QObject::tr("Statistic") ; 
      stream.begin(Writer::HtmlTocLevel2);
      if (statistic_global_coverage)
      {
        WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
        WriterSection toc_link(stream,Writer::HtmlTocLinkGlobalCoverage);
        stream << QObject::tr("Global Coverage") ; 
      }
      if (statistic_sources)
      {
        WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
        WriterSection toc_link(stream,Writer::HtmlTocLinkSource);
        stream  << QObject::tr("Sources")  ; 
      }
      if (statistic_method)
      {
        WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
        WriterSection toc_link(stream,Writer::HtmlTocLinkMethod);
        stream << QObject::tr("Namespaces, Classes and Methods")  ; 
      }
      if (statistic_executions)
      {
        WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
        WriterSection toc_link(stream,Writer::HtmlTocLinkExecution);
        stream << QObject::tr("Executions") ; 
      }
      stream.end(Writer::HtmlTocLevel2);
      stream.end(Writer::HtmlTocLevel1Item);
    }
    if (code_fragments)
    {
      WriterSection toc_level1_entry(stream,Writer::HtmlTocLevel1Item);
      stream << QObject::tr("Code Fragments") ; 
      {
        WriterSection toc_level2(stream,Writer::HtmlTocLevel2);
        if (code_fragments_manually_validated)
        {
          WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
          WriterSection toc_link(stream,Writer::HtmlTocLinkManuallyValidated);
          stream << QObject::tr("Manually Validated Code Parts") ; 
        }
        if (code_fragments_unexecuted)
        {
          WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
          WriterSection toc_link(stream,Writer::HtmlTocLinkUnexecuted);
          stream << QObject::tr("Unexecuted Code Parts") ; 
        }
        if (code_fragments_executed)
        {
          WriterSection toc_level2_entry(stream,Writer::HtmlTocLevel2Entry);
          WriterSection toc_link(stream,Writer::HtmlTocLinkExecuted);
          stream << QObject::tr("Executed Code Parts") ; 
        }
      }
    }
  }
}

void CSMesReport::generateIntro(
    Writer & stream ,
    const CSMes & csmes ,
    Instrumentation::coverage_method_t coverage_method,
    bool test_count_mode
    )
{
  stream.begin(Writer::HtmlInformation);
  stream << QObject::tr("Information") ;
  stream.end(Writer::HtmlInformation);
  {
    WriterSection information(stream,Writer::GlobalInformation);
    {
      WriterSection information(stream,Writer::GlobalInformationCoverageMethod);
      stream.begin(Writer::HtmlInformationCoverageMethodDescription);
      stream <<  QObject::tr("Coverage Method:") ;
      stream.end(Writer::HtmlInformationCoverageMethodDescription);
      stream.begin(Writer::HtmlInformationCoverageMethodValue);
      switch (coverage_method)
      {
        case Instrumentation::COVERAGE_CONDITION:
          stream << QObject::tr("defined during the generation (branch, decision or condition)")  ; 
          break;
        case Instrumentation::COVERAGE_BRANCH:
          stream << QObject::tr("branches only")  ; 
          break;
        default:
          stream << "???"; 
          break;
      }
      stream.end(Writer::HtmlInformationCoverageMethodValue);
    }
    {
      WriterSection information(stream,Writer::GlobalInformationCoverageMode);
      stream.begin(Writer::HtmlInformationCoverageModeDescription);
      stream <<  QObject::tr("Coverage Mode:") ;
      stream.end(Writer::HtmlInformationCoverageModeDescription);
      stream.begin(Writer::HtmlInformationCoverageModeValue);
      if (test_count_mode)
        stream << QObject::tr("Test Count")   ; 
      else
        stream  << QObject::tr("Execution Count")  ; 
      stream.end(Writer::HtmlInformationCoverageModeValue);
    }
  }

  {
    ExecutionNames execs=csmes.selectedExecutions();
    {
      WriterSection execution_list_title(stream,Writer::HtmlExecutionListTitle);
      stream  << QObject::tr("Execution List") << ":" ; 
    }
    WriterSection execution_list(stream,Writer::ExecutionList);
    {
      WriterSection execution_list_header(stream,Writer::HtmlExecutionListHeader);
      {
        WriterSection execution_list_header_item(stream,Writer::HtmlExecutionListHeaderItem);
        stream << QObject::tr("Name")  ; 
      }
      {
        WriterSection execution_list_header_item(stream,Writer::HtmlExecutionListHeaderItem);
        stream <<  QObject::tr("State")  ; 
      }
    }
    for (ExecutionNames::const_iterator it=execs.begin();it!=execs.end();++it)
    {
      WriterSection execution_list_item(stream,Writer::ExecutionListItem);
      {
        WriterSection execution_list_item(stream,Writer::ExecutionListName);
        stream << *it;
        QString comment;
        if (csmes.getExecutionComment(*it,comment) && !comment.isEmpty())
        {
          WriterSection execution_list_item(stream,Writer::HtmlExecutionListContainComment);
          stream.insertHtml(comment);
        }
      }
      switch (csmes.getExecutionStatus(*it))
      {
        case Executions::EXECUTION_STATUS_FAILED:
          {
            WriterSection execution_status(stream,Writer::ExecutionListStatusFailed);
            stream << csmes.getExecutionStatusStr(*it);
          }
          break;
        case Executions::EXECUTION_STATUS_PASSED:
          {
            WriterSection execution_status(stream,Writer::ExecutionListStatusPassed);
            stream << csmes.getExecutionStatusStr(*it);
          }
          break;
        case Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY:
          {
            WriterSection execution_status(stream,Writer::ExecutionListStatusToBeCheckManually);
            stream << csmes.getExecutionStatusStr(*it);
          }
          break;
        case Executions::EXECUTION_STATUS_UNKNOWN:
          {
            WriterSection execution_status(stream,Writer::ExecutionListStatusUnknown);
            stream << csmes.getExecutionStatusStr(*it);
          }
          break;
      }
    }
  }
}


void CSMesReport::generateStatisticGlobalCoverage(
    Writer & stream ,
    const CSMes & csmes ,
    bool  bargraph ,
    bool  statistic_global_coverage ,
    bool  global_coverage_output_all_level ,
    bool  global_coverage_output_selected_level_and_level_1 ,
    double  global_coverage_watermark_medium_level ,
    double  global_coverage_watermark_low_level ,
    int  global_coverage_max_intermediate_levels ,
    int global_coverage_level,
    Instrumentation::coverage_method_t coverage_method
    )
{
  if (!statistic_global_coverage)
    return;
  stream.begin(Writer::HtmlGlobalCoverage);
  stream << QObject::tr("Global Coverage") ;
  stream.end(Writer::HtmlGlobalCoverage);
  csmes.exportHtmlStatisticGlobalCoverage(stream,
      global_coverage_level,
      coverage_method,
      global_coverage_output_all_level,
      global_coverage_output_selected_level_and_level_1,
      global_coverage_max_intermediate_levels+2,
      global_coverage_watermark_low_level/100.0f,
      global_coverage_watermark_medium_level/100.0f,
      bargraph
      );
}

void CSMesReport::generateStatisticSources(
    Writer & stream ,
    const CSMes & csmes ,
    bool bargraph ,
    bool  statistic_sources ,
    bool  sources_output_all_level ,
    bool  sources_output_selected_level_and_level_1 ,
    bool  sources_sorting_coverage ,
    bool  sources_filter_active ,
    double  sources_filter_min ,
    double  sources_filter_max ,
    int  sources_max_intermediate_levels ,
    int  global_coverage_level ,
    Instrumentation::coverage_method_t coverage_method,
    double sources_watermark_low_level,
    double sources_watermark_medium_level
    )
{
  if (!statistic_sources)
    return;
  stream.begin(Writer::HtmlStatisticSourcesTitle);
  stream << QObject::tr("Sources") ; 
  stream.end(Writer::HtmlStatisticSourcesTitle);
  csmes.exportHtmlStatisticSources(stream,
      global_coverage_level,
      coverage_method,
      sources_output_all_level,
      sources_output_selected_level_and_level_1,
      sources_max_intermediate_levels+2,
      sources_filter_active?sources_filter_min/100.0f:-10000.0f,
      sources_filter_active?sources_filter_max/100.0f:-10000.0f,
      sources_watermark_low_level/100.0f,
      sources_watermark_medium_level/100.0f,
      sources_sorting_coverage,
      bargraph
      );
}

void CSMesReport::generateStatisticMethods(
    Writer & stream ,
    const CSMes & csmes ,
    bool  bargraph ,
    bool  statistic_method ,
    bool  methods_output_all_level ,
    bool  methods_output_selected_level_and_level_1 ,
    bool  methods_sorting_coverage ,
    bool  methods_filter_active ,
    double  methods_filter_min ,
    double  methods_filter_max ,
    int  methods_max_intermediate_levels ,
    Instrumentation::coverage_method_t coverage_method,
    int methods_level,
    double methods_watermark_low_level,
    double methods_watermark_medium_level
    )
{
  if (!statistic_method)
    return;
  stream.begin(Writer::HtmlStatisticMethodTitle);
  stream << QObject::tr("Namespaces, Classes and Methods") ; 
  stream.end(Writer::HtmlStatisticMethodTitle);
  csmes.exportHtmlStatisticFunction(stream,
      methods_level,
      coverage_method,
      methods_output_all_level,
      methods_output_selected_level_and_level_1,
      methods_max_intermediate_levels+2,
      methods_filter_active?methods_filter_min/100.0f:-10000.0f,
      methods_filter_active?methods_filter_max/100.0f:-10000.0f,
      methods_watermark_low_level/100.0f,
      methods_watermark_medium_level/100.0f,
      methods_sorting_coverage,
      bargraph
      );
}

void CSMesReport::generateStatisticExecutions(
    Writer & stream ,
    const CSMes & csmes ,
    bool  bargraph ,
    bool  statistic_executions ,
    int  executions_level ,
    bool  executions_output_all_level ,
    bool  executions_output_selected_level_and_level_1 ,
    bool  executions_sorting_coverage ,
    bool  executions_filter_active ,
    double  executions_filter_min ,
    double  executions_filter_max ,
    double  executions_watermark_medium_level ,
    double  executions_watermark_low_level ,
    int  executions_max_intermediate_levels ,
    Instrumentation::coverage_method_t coverage_method
    )
{
  if (!statistic_executions)
    return;
  stream.begin(Writer::HtmlStatisticExecutionTitle);
  stream << "Executions" ; 
  stream.end(Writer::HtmlStatisticExecutionTitle);
  csmes.exportHtmlStatisticExecutions(stream,
      executions_level,
      coverage_method,
      executions_output_all_level,
      executions_output_selected_level_and_level_1,
      executions_max_intermediate_levels+2,
      executions_filter_active?executions_filter_min/100.0f:-10000.0f,
      executions_filter_active?executions_filter_max/100.0f:-10000.0f,
      executions_watermark_low_level/100.0f,
      executions_watermark_medium_level/100.0f,
      executions_sorting_coverage,
      bargraph
      );
}

void CSMesReport::generateCodeFragments(
    Writer & stream ,
    const CSMes & csmes ,
    bool  code_fragments ,
    bool  code_fragments_manually_validated ,
    bool  code_fragments_unexecuted ,
    bool  code_fragments_executed ,
    Instrumentation::coverage_method_t coverage_method,
    int executed_by_limit
    )
{
  if (!code_fragments)
    return;
  if (code_fragments_manually_validated)
  {
    stream.begin(Writer::FragmentsManuallyValidated);
    stream.begin(Writer::HtmlFragmentsManuallyValidatedTitle);
    stream << QObject::tr("Manually Validated Code Parts") ; 
    stream.end(Writer::HtmlFragmentsManuallyValidatedTitle);
    csmes.exportHtmlManuallyValidated(stream,1,coverage_method,executed_by_limit);
    stream.end(Writer::FragmentsManuallyValidated);

  }
  if (code_fragments_unexecuted)
  {
    stream.begin(Writer::FragmentsUnexecuted);
    stream.begin(Writer::HtmlFragmentsUnexecutedTitle);
    stream << QObject::tr("Unexecuted Code Parts") ; 
    stream.end(Writer::HtmlFragmentsUnexecutedTitle);
    csmes.exportHtmlUnexecutedCode(stream,1,coverage_method,executed_by_limit);
    stream.end(Writer::FragmentsUnexecuted);
  }
  if (code_fragments_executed)
  {
    stream.begin(Writer::FragmentsExecuted);
    stream.begin(Writer::HtmlFragmentsExecutedTitle);
    stream << QObject::tr("Executed Code Parts") ; 
    stream.end(Writer::HtmlFragmentsExecutedTitle);
    csmes.exportHtmlExecutedCode(stream,1,coverage_method,executed_by_limit);
    stream.end(Writer::FragmentsExecuted);
  }
}


void CSMesReport::generateFooters( Writer & stream)
{
  stream.endBody();
}


bool CSMesReport::exportHtmlReport(
    DocumentType_t documentType,
    const QString &file,
    const QString &title,
    const QString &icon,
    const QString &css_file,
    DisplayLevelType_t global_statistic_display_type,
    DisplayLevelType_t method_statistic_display_type,
    DisplaySortingType_t method_statistic_sort_type,
    DisplayLevelType_t source_statistic_display_type,
    DisplaySortingType_t source_statistic_sort_type,
    DisplayLevelType_t execution_statistic_display_type,
    DisplaySortingType_t execution_statistic_sort_type,
    int executions_level,
    bool toc,
    bool code_fragments_manually_validated,
    bool code_fragments_unexecuted,
    bool  code_fragments_executed ,
    bool executions_filter_active,
    double executions_filter_min,
    double executions_filter_max,
    bool sources_filter_active,
    double sources_filter_min,
    double sources_filter_max,
    bool methods_filter_active,
    double methods_filter_min,
    double methods_filter_max,
    double executions_watermark_medium_level,
    double executions_watermark_low_level,
    double global_coverage_watermark_medium_level,
    double global_coverage_watermark_low_level,
    int global_coverage_max_intermediate_levels,
    int methods_max_intermediate_levels,
    int executions_max_intermediate_levels,
    int sources_max_intermediate_levels,
    int  global_coverage_level ,
    Instrumentation::coverage_method_t coverage_method,
    int methods_level,
    double methods_watermark_low_level,
    double methods_watermark_medium_level,
    double sources_watermark_low_level,
    double sources_watermark_medium_level,
    bool test_count_mode,
    bool bargraph,
    int executed_by_limit
    ) const
{
  if (documentType == DOCUMENT_OPEN_OFFICE)
    bargraph=false;
  bool statistic_global_coverage=global_statistic_display_type!=DISPLAY_NOTHING;

  bool global_coverage_output_all_level=(global_statistic_display_type==ALL_LEVEL);
  bool global_coverage_output_selected_level_and_level_1=(global_statistic_display_type==SELECTED_LEVEL_AND_LEVEL_1);

  bool statistic_method=method_statistic_display_type!=DISPLAY_NOTHING;
  bool methods_output_all_level=(method_statistic_display_type==ALL_LEVEL);
  bool methods_output_selected_level_and_level_1=(method_statistic_display_type==SELECTED_LEVEL_AND_LEVEL_1);
  bool methods_sorting_coverage=(method_statistic_sort_type==SORT_COVERAGE);

  bool statistic_sources=source_statistic_display_type!=DISPLAY_NOTHING;
  bool sources_output_all_level=(source_statistic_display_type==ALL_LEVEL);
  bool sources_output_selected_level_and_level_1=(source_statistic_display_type==SELECTED_LEVEL_AND_LEVEL_1);
  bool sources_sorting_coverage=(source_statistic_sort_type==SORT_COVERAGE);

  bool statistic_executions=execution_statistic_display_type!=DISPLAY_NOTHING;
  bool executions_output_all_level=(execution_statistic_display_type==ALL_LEVEL);
  bool executions_output_selected_level_and_level_1=(execution_statistic_display_type==SELECTED_LEVEL_AND_LEVEL_1);
  bool executions_sorting_coverage=(execution_statistic_sort_type==SORT_COVERAGE);

  bool code_fragments=code_fragments_manually_validated || code_fragments_unexecuted || code_fragments_executed;

  const CSMes *csmes_p=dynamic_cast<const CSMes*>(this);
  if (csmes_p==NULL)
    return false;
  QBuffer output;
  output.open(QBuffer::WriteOnly);
  Writer *stream_p=NULL;
  switch (documentType)
  {
    case DOCUMENT_XML:
      stream_p=new XMLWriter(&output);
      break;
    case DOCUMENT_HTML:
    case DOCUMENT_OPEN_OFFICE:
      stream_p=new HTMLWriter(&output);
      break;
  }
  generateHeaders(
      *stream_p,
      title,
      icon,
      css_file,
      statistic_global_coverage,
      statistic_method,
      statistic_sources,
      statistic_executions,
      code_fragments,
      toc,
      code_fragments_manually_validated,
      code_fragments_unexecuted,
      code_fragments_executed
      );
  generateIntro(
      *stream_p,
      *csmes_p,
      coverage_method,
      test_count_mode
      );
  generateStatisticGlobalCoverage(
      *stream_p ,
      *csmes_p ,
      bargraph ,
      statistic_global_coverage ,
      global_coverage_output_all_level ,
      global_coverage_output_selected_level_and_level_1 ,
      global_coverage_watermark_medium_level ,
      global_coverage_watermark_low_level ,
      global_coverage_max_intermediate_levels ,
      global_coverage_level,
      coverage_method
      );
  generateStatisticSources(
      *stream_p,
      *csmes_p,
      bargraph ,
      statistic_sources ,
      sources_output_all_level ,
      sources_output_selected_level_and_level_1 ,
      sources_sorting_coverage ,
      sources_filter_active ,
      sources_filter_min ,
      sources_filter_max ,
      sources_max_intermediate_levels ,
      global_coverage_level ,
      coverage_method,
      sources_watermark_low_level,
      sources_watermark_medium_level
      );
  generateStatisticMethods(
      *stream_p,
      *csmes_p,
      bargraph ,
      statistic_method ,
      methods_output_all_level ,
      methods_output_selected_level_and_level_1 ,
      methods_sorting_coverage ,
      methods_filter_active ,
      methods_filter_min ,
      methods_filter_max ,
      methods_max_intermediate_levels ,
      coverage_method,
      methods_level,
      methods_watermark_low_level,
      methods_watermark_medium_level
      );
  generateStatisticExecutions(
      *stream_p,
      *csmes_p,
      bargraph ,
      statistic_executions ,
      executions_level ,
      executions_output_all_level ,
      executions_output_selected_level_and_level_1 ,
      executions_sorting_coverage ,
      executions_filter_active ,
      executions_filter_min ,
      executions_filter_max ,
      executions_watermark_medium_level ,
      executions_watermark_low_level ,
      executions_max_intermediate_levels ,
      coverage_method
      );
  generateCodeFragments(
      *stream_p,
      *csmes_p,
      code_fragments ,
      code_fragments_manually_validated ,
      code_fragments_unexecuted ,
      code_fragments_executed ,
      coverage_method,
      executed_by_limit
      );
  generateFooters( *stream_p);
  output.close();
  delete stream_p;

  switch (documentType)
  {
    case DOCUMENT_XML:
    case DOCUMENT_HTML:
      {
        QFile File;
        File.setFileName(file);
        if (!File.open(QIODevice::WriteOnly))
          return false;
        File.write(output.data());
        File.close();
      }
      break;
    case DOCUMENT_OPEN_OFFICE:
      {
        QTextDocument doc;
        QTextDocumentFragment frag = QTextDocumentFragment::fromHtml(output.data(),NULL);
        QTextCursor cur(&doc);
        cur.insertFragment(frag);
        QTextDocumentWriter odtfile(file,"ODF");
        if (!odtfile.write(&doc))
          return false;
      }
      break;
  }

  return true;
}

QVariant CSMesReport::StatValue(int nb_tested,int nb_untested)
{
  double stat;
  if (nb_tested<0 || nb_untested<0 || nb_tested+nb_untested==0)
    return "--";
  else
  {
    stat=static_cast<double>(nb_tested)/(static_cast<double>(nb_tested+nb_untested));
    return stat;
  }
}

