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
#include "csmescsv.h"
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

CSMesCsv::CSMesCsv() : CSMesTextReport()
{
}


QString CSMesCsv::CSVEscape(const QString &v)
{
  QString r=QString();
  int lg=v.length();
  r.reserve(lg+2);
  r+='"';
  for (int i=0;i<lg;i++)
  {
    r+=v[i];
    if (v[i]=='"')
      r+='"';
  }
  r+='"';
  return r;
}

QString CSMesCsv::CSVstat(int nb_tested,int nb_untested,QChar floatSep)
{
  QVariant stat=StatValue(nb_tested,nb_untested);
  bool ok;
  double stat_value=stat.toDouble(&ok);
  if (!ok)
    return CSVEscape("--");
  else
    return QString::number(stat_value).replace('.',floatSep);
}

QString CSMesCsv::exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const
{
  int nb_tested,nb_untested;
  const QStringList executions=selectedExecutions();
  int nb_executions=executions.count();
  QStringList::const_iterator it;
  if (executions.isEmpty())
    return QObject::tr("No executions selected");
  QFile f(filename);
  if (!f.open(QIODevice::WriteOnly))
    return QObject::tr("Could not open file");

  QTextStream stream(&f);
  QTime tm;
  tm.start();
  SourceFiles::const_iterator it_src;
  const SourceFiles sources= Sources(NON_EMPTY);
  float max_elem=(nb_executions+1)*(1+sources.count());
  float count=0;

  // Title line
  stream << CSVEscape(QObject::tr("File Name")) ;
  stream << separator << CSVEscape(QObject::tr("Status")) ;
  int c;
  for (it_src=sources.begin(),c=0;it_src!=sources.end();++it_src,c++)
  {
    stream << separator << CSVEscape(relativeSourceName(*it_src));
  }
  stream << separator << CSVEscape(QObject::tr("Total - level %1").arg(QString::number(coverage_level)));
  stream << '\n' ;

  const QStringList execStr = executionsStatusStr();
  // module statistic
  printStatus(QObject::tr("Exporting Statistics..."),count/max_elem); count++;
  for (it=executions.begin();it!=executions.end();++it)
  {
    QStringList execs;
    execs+=*it;
    QHash<QString,int> nb_tested_list;
    QHash<QString,int> nb_untested_list;
    if ( statisticSourcesExecution(sources,execs,coverage_level,method,nb_tested_list,nb_untested_list,instrumentations) )
    {
      stream << CSVEscape(*it);
      stream << separator << CSVEscape(execStr[getExecutionStatus(*it)]);
      // statistics for each module/execution
      for (it_src=sources.begin(),c=0;it_src!=sources.end();++it_src,c++)
      {
        if (nb_tested_list.contains(*it_src) && nb_untested_list.contains(*it_src) )
          stream << separator << CSVstat(nb_tested_list[*it_src],nb_untested_list[*it_src],floatSep);
        else
          stream << separator << CSVEscape("--");
      }

      // statistics for each execution
      QStringList execs;
      execs+=*it;
      if (statisticExecution(execs,QStringList(),false,coverage_level,method,nb_tested,nb_untested,instrumentations,false))
        stream << separator << CSVstat(nb_tested,nb_untested,floatSep);
      else
        stream << separator << CSVEscape("--");
    }

    printStatus(QObject::tr("Exporting Statistics..."),count/max_elem); count++;
    stream << '\n' ;
  }

  stream << CSVEscape(QObject::tr("Total"));
  stream << separator ;
  // statistics for each module
  QHash<QString,int> nb_tested_list;
  QHash<QString,int> nb_untested_list;
  if ( statisticSourcesExecution(sources,executions,coverage_level,method,nb_tested_list,nb_untested_list,instrumentations) )
  {
    for (it_src=sources.begin(),c=0;it_src!=sources.end();++it_src,c++)
    {
      if (nb_tested_list.contains(*it_src) && nb_untested_list.contains(*it_src) )
        stream << separator << CSVstat(nb_tested_list[*it_src],nb_untested_list[*it_src],floatSep);
      else
        stream << separator << CSVEscape("--");
    }
  }
  printStatus(QObject::tr("Exporting Statistics..."),count/max_elem); count++;

  // global statistic
  if (statisticExecution(executions,QStringList(),false,coverage_level,method,nb_tested,nb_untested,instrumentations,false))
    stream << separator << CSVstat(nb_tested,nb_untested,floatSep);
  else
    stream << separator << CSVEscape("--");
  stream << '\n' ;
  stream << '\n' ;

  printStatus(QObject::tr("Statistics Exported")+" "+QString::number(tm.elapsed())+"ms",1);


  return QString();
}

QString CSMesCsv::exportCSVtable(const table_t &table,const QString &filename,QChar separator,QChar floatSep) const
{
  int imax=table.size();
  if (imax<=0)
    return QObject::tr("Table is empty");

  int jmax=table.at(0).size();
  if (jmax<=0)
    return QObject::tr("Table is empty");

  QFile f(filename);
  if (!f.open(QIODevice::WriteOnly))
    return QObject::tr("Could not open file '%1'").arg(filename);
  QTextStream stream(&f);
  for (int i=0;i<imax;i++)
  {
    if (i!=0)
      stream << '\n';
    for (int j=0;j<jmax;j++)
    {
      const table_data_t &value=table.at(i).at(j);
      if (value.type()==TABLE_FORMAT_FUNCTION)
      {
        stream << CSVEscape(value.func().method.getScopedName()) ;
      }
      else
      {
        bool isFloat;
        value.data().toDouble(&isFloat);
        if (j!=0)
          stream << separator;
        if (value.data()==QVariant())
          ;
        else if (isFloat)
        {
          if (value.type()==TABLE_FORMAT_LEVEL)
            stream << QObject::tr("level") + " ";
          stream << value.data().toString().replace('.',floatSep);
        }
        else
          stream << CSVEscape(value.data().toString()) ;
      }
    }
  }

  return QString();
}


QString CSMesCsv::exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const
{
  QTime tm;
  tm.start();

  table_t table;
  QString result = exportStatisticFunction(table,coverage_level,method) ;

  if (!result.isEmpty())
    return result;

  printStatus(QObject::tr("Saving file..."),1.0);
  result = exportCSVtable(table,filename, separator,floatSep);

  printStatus(QObject::tr("Statistics Exported")+" "+QString::number(tm.elapsed())+"ms",-1);

  return result;
}



