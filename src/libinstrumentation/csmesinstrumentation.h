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

#ifndef CSMES_INSTRUMENTATION_H
#define CSMES_INSTRUMENTATION_H
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>
#include <QHash>
#include "instrumentation.h"
#include "FunctionInfo.h"
#include "csmesinstrumentationdatabase.h"
class BenchFindSourceModule;
class TestFindSourceModule;

class CSMesInstrumentation 
{
  public:
    enum source_filter_t
    {
      ALL,
      NON_EMPTY,
    };
  protected:
    CSMesInstrumentation();
  protected:
    const ModuleFiles& Modules() const ;
    SourceFiles Sources(const ModuleFile &s) const ;
    const SourceFiles& Headers() const ;
    const SourceFiles& Sources(source_filter_t) const ;
    bool moduleExists(const ModuleFile &m) const;
    bool modificationFlag() const { return modification_flag; }
    bool getManuallyValidated(ModuleFile mod,SourceFile src,int index) const;
    void setManuallyValidated(ModuleFile mod,SourceFile src,int index,bool b);
    QString relativeSourceName(const SourceFile &src) const;
  protected:
    CSMesInstrumentations instrumentations;
    void copyInstrumentation(CSMesInstrumentations &dst, const CSMesInstrumentations &src) const ;
    void generateEquivalences(CSMesInstrumentations &) const;
  private:
    SourceFiles Sources(bool) const ;
    const QString &findSourceForModule(QString &module,int instrumentation_index) const;
    ModuleFiles Modules_int() const ;
    SourceFiles Headers_int() const ;
    SourceFiles Sources_int(source_filter_t) const ;
    bool modification_flag;
    SourceFiles _sources_empty_modules;
    ModuleFiles _modules;
    SourceFiles _headers;
    SourceFiles _sources_non_empty_modules;
    mutable QHash<QString,QString> _findSourceModuleCache;
  protected:
    void resetModificationFlag() { modification_flag=false; }
    void setModificationFlag() { modification_flag=true; }
    int nbInstrumentations(const QString &module) const;
    void clear();
    void update();
    bool findSourceModule(ModuleFile &module,SourceFile &source) const;
    bool getManuallyValidated_intern(const ModuleFile &mod,const SourceFile &src,int index) const;
    void setManuallyValidated_intern(const ModuleFile &mod,const SourceFile &src,int index,bool b);

    friend class BenchFindSourceModule;
    friend class TestFindSourceModule;
};

#endif
