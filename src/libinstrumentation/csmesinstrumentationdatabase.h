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

#ifndef CSMES_INSTRUMENTATION_DATABASE_H
#define CSMES_INSTRUMENTATION_DATABASE_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QHash>
#include "instrumentation.h"
#include "FunctionInfo.h"
#include "modulefiles.h"
#include "sourcefiles.h"

class CSMesInstrumentations 
{
  protected:

  class Instrumentations : public QVector<Instrumentation> 
  {
    public:
      Instrumentations(): QVector<Instrumentation>()
    {
    }

      bool isNOP() const
      {
        for (CSMesInstrumentations::Instrumentations::const_iterator it_status=this->begin();it_status!=this->end();++it_status)
        {
          if (it_status->getType()!=Instrumentation::NOP)
            return false;
        }
        return true;
      }
  };

  class Functions : public QVector<FunctionInfo> 
  {
    public:
      Functions() :  QVector<FunctionInfo>()
    {
    }
  };

  class Source
  {
    public:
      Instrumentations instrumentations;
      Functions        functions;
  } ;

  class Sources : public QHash<SourceFile,Source> 
  {
    public:
      Sources() : QHash<SourceFile,Source>()
    {
    }

      bool hasNOP() const 
      {
        for (CSMesInstrumentations::Sources::const_iterator it_src=begin(); it_src!=end();++it_src)
        {
           if (it_src.value().instrumentations.isNOP())
              return true;
        }
        return false;
      }
  };

  class Module 
  {
    public:
      Sources sources;
      QHash<QString,QString> source_relative_name;
      unsigned long signature;
      int nb_measurements_items;
      ModuleFile module_relative_name;
  } ;

  class Modules : public QHash<ModuleFile,Module> 
  {
    public:
      Modules() :  QHash<ModuleFile,Module>()
    {
    }
  };

  Modules modules;

  friend class CSMesInstrumentation;
  friend class CSMesRTF;
  friend class CSMesAnalysis;
  friend class CSMesExecution;
  friend class CSMesFunctionInfo;
  friend class CSMesReport;
  friend class CSMesEmma;
  friend class CSMesIO;
  friend class CSMesStatistic;
};

#endif
