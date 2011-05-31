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
#include "csmesemma.h"
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

CSMesEmma::CSMesEmma() : CSMesCsv()
{
}


QString CSMesEmma::emmaStatistic(int nb_tested,int nb_untested)
{
   if (nb_tested+nb_untested==0)
      return "100% (0/0)";
   else
      return QString::number((nb_tested*100)/(nb_tested+nb_untested))+"% ("+QString::number(nb_tested)+"/"+QString::number(nb_tested+nb_untested)+")";
}


void CSMesEmma::emmaStatisticClasses( const QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested_function_list,int &nb_tested,int &nb_untested)
{
  QSet<QString> _nb_tested;
  QSet<QString> _nb_all;
  for (QHash<CSMesFunctionInfo::functionskey_t,int>::const_iterator itNbTested=nb_tested_function_list.begin();itNbTested!=nb_tested_function_list.end();++itNbTested)
  {
    QString c=classFromFunction(itNbTested.key().method.getScopedName());
    if (itNbTested.value()>0)
      _nb_tested+=c;
    _nb_all+=c;
  }
  nb_tested=_nb_tested.count();
  nb_untested=_nb_all.count()-_nb_tested.count();
}

void CSMesEmma::emmaStatisticMethods( const QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested_function_list,int &nb_tested,int &nb_untested)
{
  nb_tested=0;
  nb_untested=0;
  for (QHash<CSMesFunctionInfo::functionskey_t,int>::const_iterator itNbTested=nb_tested_function_list.begin();itNbTested!=nb_tested_function_list.end();++itNbTested)
  {
    if (itNbTested.value()==0)
      nb_untested++;
    else
      nb_tested++;
  }
}

QString CSMesEmma::classFromFunction(const QString &func)
{
  QString c;
  int pos=func.lastIndexOf("::");
  if (pos>=0)
    c=func.mid(0,pos);
  return c;
}

void CSMesEmma::exportEmmaStatisticsField(QXmlStreamWriter &stream,
                                     const QString &name,
                                     int nb_tested,int nb_untested)
{
  if (nb_tested>=0 && nb_untested>=0)
  {
    stream.writeEmptyElement("coverage");
    stream.writeAttribute("type",name+", %");
    stream.writeAttribute("value",emmaStatistic(nb_tested,nb_untested));
  }
}

void CSMesEmma::exportEmmaStatistics(QXmlStreamWriter &stream,
                                     int nb_tested_line,int nb_untested_line,
                                     int nb_tested_method,int nb_untested_method,
                                     int nb_tested_class,int nb_untested_class,
                                     int nb_tested_block,int nb_untested_block )
{
  exportEmmaStatisticsField(stream,"line",nb_tested_line,nb_untested_line);
  exportEmmaStatisticsField(stream,"method",nb_tested_method,nb_untested_method);
  exportEmmaStatisticsField(stream,"class",nb_tested_class,nb_untested_class);
  exportEmmaStatisticsField(stream,"block",nb_tested_block,nb_untested_block);
}

QString CSMesEmma::exportEMMAStatistic(const QString &filename,int coverage_level,Instrumentation::coverage_method_t method) const
{
  const QStringList executions=selectedExecutions();
  int nb_executions=executions.count();
  if (nb_executions==0)
    return QObject::tr("No executions selected");

  QFile f;
  f.setFileName(filename);
  if (f.open(QFile::WriteOnly))
  {
    bool is_test_count_mode_selected=isTestCountModeSelected();

    CSMesInstrumentations _instrumentations ;
    copyInstrumentation(_instrumentations,instrumentations);
    selectExecutionsComparaison(_instrumentations,executions,QStringList(),is_test_count_mode_selected,method,false);
    CSMesInstrumentations _instrumentations_block ;
    copyInstrumentation(_instrumentations_block,instrumentations);
    selectExecutionsComparaison(_instrumentations_block,executions,QStringList(),is_test_count_mode_selected,Instrumentation::COVERAGE_BRANCH,false);
    QStringList::const_iterator it_fct;
    const QList<CSMesFunctionInfo::functionskey_t> functions= Functions();
    int nb_functions=functions.count();
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_function_list;
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_untested_function_list;
    QSet<QString> classes;
    if ( statisticFunctionsExecution(executions,coverage_level,method,nb_tested_function_list,nb_untested_function_list,_instrumentations) )
    {
      for ( QList<CSMesFunctionInfo::functionskey_t>::const_iterator itFunction=functions.begin();itFunction<functions.end();++itFunction++)
      {
        const CSMesFunctionInfo::functionskey_t &f=*itFunction;
        if (nb_tested_function_list.contains(f) && nb_untested_function_list.contains(f) )
        {
          const QString &func=f.method.getScopedName();
          classes+=classFromFunction(func);
        }
      }
    }
    int nb_class=classes.count();

    QHash<QString,int> nb_tested_source_list;
    QHash<QString,int> nb_untested_source_list;
    const SourceFiles sources= Sources(NON_EMPTY);
    int nb_sources=sources.count();
    statisticSourcesExecution(sources,executions,coverage_level,method,nb_tested_source_list,nb_untested_source_list,_instrumentations) ;

    int nb_tested,nb_untested;
    statistic(coverage_level,method,nb_tested,nb_untested,_instrumentations) ;

    int nb_tested_block,nb_untested_block;
    statistic(coverage_level,Instrumentation::COVERAGE_BRANCH,nb_tested_block,nb_untested_block,_instrumentations_block) ;

    QHash<QString,int> nb_tested_source_block_list;
    QHash<QString,int> nb_untested_source_block_list;
    statisticSourcesExecution(sources,executions,coverage_level,Instrumentation::COVERAGE_BRANCH,nb_tested_source_block_list,nb_untested_source_block_list,_instrumentations_block) ;

    QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_function_block_list;
    QHash<CSMesFunctionInfo::functionskey_t,int> nb_untested_function_block_list;
    statisticFunctionsExecution(executions,coverage_level,Instrumentation::COVERAGE_BRANCH,nb_tested_function_block_list,nb_untested_function_block_list,_instrumentations_block) ;

    int nb_instrumentation=0;
    for (CSMesInstrumentations::Modules::const_iterator mod_it=_instrumentations.modules.begin();mod_it!=_instrumentations.modules.end();++mod_it)
      nb_instrumentation += mod_it->nb_measurements_items;


    QXmlStreamWriter stream(&f);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("report");

    stream.writeStartElement("stats");

    stream.writeEmptyElement("packages");
    stream.writeAttribute("value",QString::number(1));

    stream.writeEmptyElement("methods");
    stream.writeAttribute("value",QString::number(nb_functions));

    stream.writeEmptyElement("srcfiles");
    stream.writeAttribute("value",QString::number(nb_sources));

    stream.writeEmptyElement("srclines");
    stream.writeAttribute("value",QString::number(nb_tested+nb_untested));

    stream.writeEmptyElement("classes");
    stream.writeAttribute("value",QString::number(nb_class));
    stream.writeEndElement();

    stream.writeStartElement("data");
    stream.writeStartElement("all");
    stream.writeAttribute("name","all classes");

    {
      int nb_tested_line=nb_tested;
      int nb_untested_line=nb_untested;
      int nb_tested_method;
      int nb_untested_method;
      int nb_tested_class;
      int nb_untested_class;
      int nb_tested_blocks=nb_tested_block;
      int nb_untested_blocks=nb_tested_block;
      emmaStatisticMethods(nb_tested_function_list,nb_tested_method,nb_untested_method);
      emmaStatisticClasses(nb_tested_function_list,nb_tested_class,nb_untested_class);

      exportEmmaStatistics(stream,nb_tested_line,nb_untested_line,nb_tested_method,nb_untested_method,nb_tested_class,nb_untested_class,nb_tested_blocks,nb_untested_blocks);
    }
    stream.writeStartElement("packages");
    stream.writeAttribute("name",getFilename());

    for (SourceFiles::const_iterator itSrc=sources.begin();itSrc!=sources.end();++itSrc)
    {
      stream.writeStartElement("srcfile");
      stream.writeAttribute("name",*itSrc);
      QList<CSMesFunctionInfo::functionskey_t> functions_source;
      QSet<QString> classes_source;
      for (QList<CSMesFunctionInfo::functionskey_t>::const_iterator itMeth=functions.begin();itMeth!=functions.end();++itMeth)
      {
        if ((*itMeth).source==*itSrc)
        {
          functions_source+=*itMeth;
          const QString &func=(*itMeth).method.getScopedName();
          classes_source+=classFromFunction(func);
        }
      }
      {
        int nb_tested_line=0;
        int nb_untested_line=0;
        if (nb_tested_source_list.contains(*itSrc) && nb_untested_source_list.contains(*itSrc) )
        {
          nb_tested_line=nb_tested_source_list[*itSrc];
          nb_untested_line=nb_untested_source_list[*itSrc];
        }
        QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_functions;
        for (QHash<CSMesFunctionInfo::functionskey_t,int>::const_iterator itFunc=nb_tested_function_list.begin();itFunc!=nb_tested_function_list.end();++itFunc)
        {
          if (itFunc.key().source==*itSrc)
            nb_tested_functions[itFunc.key()]=itFunc.value();
        }
        int nb_tested_method=0;
        int nb_untested_method=0;
        int nb_tested_class=0;
        int nb_untested_class=0;
        int nb_tested_blocks=0;
        int nb_untested_blocks=0;
        if (nb_tested_source_block_list.contains(*itSrc) && nb_untested_source_block_list.contains(*itSrc) )
        {
          nb_tested_blocks=nb_tested_source_block_list[*itSrc];
          nb_untested_blocks=nb_untested_source_block_list[*itSrc];
        }
        emmaStatisticMethods(nb_tested_functions,nb_tested_method,nb_untested_method);
        emmaStatisticClasses(nb_tested_functions,nb_tested_class,nb_untested_class);

        exportEmmaStatistics(stream,nb_tested_line,nb_untested_line,nb_tested_method,nb_untested_method,nb_tested_class,nb_untested_class,nb_tested_blocks,nb_untested_blocks);
      }
      for (QSet<QString>::const_iterator itClass=classes_source.begin();itClass!=classes_source.end();++itClass)
      {
        stream.writeStartElement("class");
        stream.writeAttribute("name",*itClass);
        {
          int nb_tested_line=0;
          int nb_untested_line=0;
          int nb_tested_blocks=0;
          int nb_untested_blocks=0;
          QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_functions;
          for (QHash<CSMesFunctionInfo::functionskey_t,int>::const_iterator itFunc=nb_tested_function_list.begin();itFunc!=nb_tested_function_list.end();++itFunc)
          {
            if ( (itFunc.key().source==*itSrc) && (classFromFunction(itFunc.key().method.getScopedName())==*itClass))
            {
              nb_tested_functions[itFunc.key()]=itFunc.value();
              nb_tested_line+=itFunc.value();
              nb_untested_line+=nb_untested_function_list[itFunc.key()];
              nb_tested_blocks+=nb_tested_function_block_list[itFunc.key()];
              nb_untested_blocks+=nb_untested_function_block_list[itFunc.key()];
            }
          }
          int nb_tested_method=0;
          int nb_untested_method=0;
          int nb_tested_class=0;
          int nb_untested_class=0;
          emmaStatisticMethods(nb_tested_functions,nb_tested_method,nb_untested_method);
          emmaStatisticClasses(nb_tested_functions,nb_tested_class,nb_untested_class);

          exportEmmaStatistics(stream,nb_tested_line,nb_untested_line,nb_tested_method,nb_untested_method,nb_tested_class,nb_untested_class,nb_tested_blocks,nb_untested_blocks);
        }
        for (QList<CSMesFunctionInfo::functionskey_t>::const_iterator itMeth=functions_source.begin();itMeth!=functions_source.end();++itMeth)
        {
          if ( classFromFunction(itMeth->method.getScopedName())==*itClass)
          {
            stream.writeStartElement("method");
            stream.writeAttribute("name",(*itMeth).method.getScopedName());
            {
              int nb_tested_line=0;
              int nb_untested_line=0;
              int nb_tested_blocks=0;
              int nb_untested_blocks=0;
              QHash<CSMesFunctionInfo::functionskey_t,int> nb_tested_functions;
              for (QHash<CSMesFunctionInfo::functionskey_t,int>::const_iterator itFunc=nb_tested_function_list.begin();itFunc!=nb_tested_function_list.end();++itFunc)
              {
                if ( itFunc.key()==*itMeth )
                {
                  nb_tested_functions[itFunc.key()]=itFunc.value();
                  nb_tested_line+=itFunc.value();
                  nb_untested_line+=nb_untested_function_list[itFunc.key()];
                  nb_tested_blocks+=nb_tested_function_block_list[itFunc.key()];
                  nb_untested_blocks+=nb_untested_function_block_list[itFunc.key()];
                }
              }
              int nb_tested_method=0;
              int nb_untested_method=0;
              emmaStatisticMethods(nb_tested_functions,nb_tested_method,nb_untested_method);

              exportEmmaStatistics(stream,nb_tested_line,nb_untested_line,nb_tested_method,nb_untested_method,-1,-1,nb_tested_blocks,nb_untested_blocks);
            }
            stream.writeEndElement();
          }
        }
        stream.writeEndElement();
      }
      stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();
    f.close();
    return QString();
  }
  else
    return QObject::tr("Error opening file '%1':%2").arg(filename).arg(f.errorString());
}

