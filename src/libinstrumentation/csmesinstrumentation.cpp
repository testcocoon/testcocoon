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

#include "csmesinstrumentation.h"
#include "Service.h"
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

CSMesInstrumentation::CSMesInstrumentation()
{
  clear();
  resetModificationFlag();
}

const ModuleFiles & CSMesInstrumentation::Modules() const
{
  return _modules;
}

ModuleFiles CSMesInstrumentation::Modules_int() const
{
  ModuleFiles l=instrumentations.modules.keys();
  qSort(l);
  return l;
}

bool CSMesInstrumentation::moduleExists(const ModuleFile &m) const
{
  return instrumentations.modules.contains(m);
}

const SourceFiles & CSMesInstrumentation::Sources(source_filter_t source_filter) const
{
  switch (source_filter)
  {
    case ALL:
      return _sources_empty_modules;
    default:
      ASSERT(false);
    case NON_EMPTY:
      return _sources_non_empty_modules;
  }
}

SourceFiles CSMesInstrumentation::Sources_int(source_filter_t f) const
{
  ModuleFiles  modules=Modules_int();
  SourceFiles  sources;
  for (ModuleFiles::const_iterator it_mod= modules.begin(); it_mod != modules.end(); ++it_mod )
  {
    const ModuleFile &mod=*it_mod;
    SourceFiles  srcs= instrumentations.modules[mod].sources.keys();
    for (SourceFiles::const_iterator it_src= srcs.begin(); it_src != srcs.end(); ++it_src )
    {
      const SourceFile &src=*it_src;
      if (!sources.contains(src))
      {
        if (f==ALL)
          sources.append(src);
        else
        {
          const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;
          bool empty=inst.isNOP();
          if (!empty)
            sources.append(src);
        }
      }
    }
  }

  qSort(sources);
  return sources;
}

const SourceFiles &CSMesInstrumentation::Headers() const
{
  return _headers;
}

SourceFiles CSMesInstrumentation::Headers_int() const
{
  SourceFiles headers=Sources_int(ALL);
  ModuleFiles modules=Modules_int();
  for (ModuleFiles::const_iterator it_mod= modules.begin(); it_mod != modules.end(); ++it_mod )
  {
    if (headers.contains(*it_mod))
      headers.removeAll(*it_mod);
  }
  qSort(headers);
  return headers;
}

SourceFiles CSMesInstrumentation::Sources(const ModuleFile &m) const
{
  if (instrumentations.modules.contains(m))
    return instrumentations.modules[m].sources.keys();
  else
    return SourceFiles();
}

QString CSMesInstrumentation::relativeSourceName(const SourceFile &s) const
{
  SourceFile src=s;
  ModuleFile mod("");
  if (!findSourceModule(mod,src))
    return src;
  return instrumentations.modules[mod].source_relative_name[src];
}

void CSMesInstrumentation::clear()
{
  instrumentations.modules.clear();
  _sources_empty_modules.clear();
  _sources_non_empty_modules.clear();
  _modules.clear();
  _headers.clear();
  QMutexLocker locker(&_findSourceModuleCache_mutex);
  _findSourceModuleCache.clear();
}

void CSMesInstrumentation::update()
{
  _sources_empty_modules=Sources_int(ALL);
  _sources_non_empty_modules=Sources_int(NON_EMPTY);
  _headers=Headers_int();
  _modules=Modules_int();
}

int CSMesInstrumentation::nbInstrumentations(const QString &module) const
{
  int nb_mes = -1;
  if (!instrumentations.modules.contains(module))
    return -1;
  const CSMesInstrumentations::Module &module_inst=instrumentations.modules[module];

  CSMesInstrumentations::Sources::const_iterator srcit;
  for ( srcit = module_inst.sources.begin(); srcit != module_inst.sources.end(); ++srcit )
  {
    int sz=(*srcit).instrumentations.size();
    for (int i=0;i<sz;i++)
    {
      if ((*srcit).instrumentations[i].getMaxIndex()+1>nb_mes)
        nb_mes=(*srcit).instrumentations[i].getMaxIndex()+1;
    }
  }
  return nb_mes;
}

const QString &CSMesInstrumentation::findSourceForModule(QString &module,int instrumentation_index) const
{
  static QString null=QString::null;
  const CSMesInstrumentations::Module &mod=instrumentations.modules[module];
  CSMesInstrumentations::Sources::const_iterator it_src;
  for (it_src= mod.sources.begin(); it_src != mod.sources.end(); ++it_src )
  {
    for (CSMesInstrumentations::Instrumentations::const_iterator it_inst=it_src.value().instrumentations.begin();
        it_inst!=it_src.value().instrumentations.end();
        ++it_inst)
    {
      if (it_inst->getMinIndex()>=instrumentation_index || it_inst->getMaxIndex()<=instrumentation_index)
        return it_src.key();
    }
  }
  return null;
}

bool CSMesInstrumentation::findSourceModule(ModuleFile &module,SourceFile &source) const
{
  if (source.isEmpty())
  {
    if (module.isEmpty())
      return false;
    source=module;
  }
  else if (module.isEmpty())
  {
	QMutexLocker locker(&_findSourceModuleCache_mutex);
    module=_findSourceModuleCache.value(source);
    if (module.isEmpty())
    {
      CSMesInstrumentations::Modules::const_iterator it;
      for (it= instrumentations.modules.begin(); it != instrumentations.modules.end(); ++it )
      {
        if (it.value().sources.contains(source))
        {
          module=it.key();
          if (!it.value().sources.hasNOP())
            break;
        }
      }
      _findSourceModuleCache[source]=module;
    }
  }

  if (!instrumentations.modules.contains(module))
  {
    return false;
  }
  if (!instrumentations.modules[module].sources.contains(source))
  {
    return false;
  }
  return true;
}


void CSMesInstrumentation::setManuallyValidated(ModuleFile mod,SourceFile src,int index,bool b)
{
  findSourceModule(mod,src);
  CSMesInstrumentations::Modules::Iterator it;
  for (it= instrumentations.modules.begin(); it != instrumentations.modules.end(); ++it )
  {
    if (it.value().sources.contains(src))
      setManuallyValidated_intern(it.key(),src,index,b);
  }
}

void CSMesInstrumentation::setManuallyValidated_intern(const ModuleFile &mod,const SourceFile &src,int index,bool b)
{
  setModificationFlag();
  instrumentations.modules[mod].sources[src].instrumentations[index].setManuallyValidated(b);
}

bool CSMesInstrumentation::getManuallyValidated(ModuleFile mod,SourceFile src,int index) const
{
  findSourceModule(mod,src);
  CSMesInstrumentations::Modules::const_iterator it;
  for (it= instrumentations.modules.begin(); it != instrumentations.modules.end(); ++it )
  {
    if (it.value().sources.contains(src))
      return getManuallyValidated_intern(it.key(),src,index);
  }
  return false;
}

bool CSMesInstrumentation::getManuallyValidated_intern(const ModuleFile &mod,const SourceFile &src,int index) const
{
  return instrumentations.modules[mod].sources[src].instrumentations[index].getManuallyValidated();
}

void CSMesInstrumentation::copyInstrumentation(CSMesInstrumentations &dst, const CSMesInstrumentations &src) const
{
  dst=src;
  generateEquivalences(dst);
}

void CSMesInstrumentation::generateEquivalences(CSMesInstrumentations &_instrumentations) const
{
  CSMesInstrumentations::Modules::Iterator modit;
  // Generate list of modules for each source file
  QHash<QString,QStringList> modules_per_sources;
  for ( modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
  {
     QString cur_module=modit.key();
     CSMesInstrumentations::Sources::Iterator srcit;
     for ( srcit = modit.value().sources.begin(); srcit != modit.value().sources.end(); ++srcit )
     {
        QString cur_source=srcit.key();
        if (!srcit.value().instrumentations.isNOP())
          modules_per_sources[cur_source]+=cur_module;
     }
  }

  // Clear equivalences
  for ( modit = _instrumentations.modules.begin(); modit != _instrumentations.modules.end(); ++modit )
  {
    CSMesInstrumentations::Sources::Iterator srcit;
    for ( srcit = modit.value().sources.begin(); srcit != modit.value().sources.end(); ++srcit )
    {
      CSMesInstrumentations::Instrumentations *inst_p=&(srcit.value().instrumentations);
      int sz=inst_p->size();
      int instrument_id;

      for (instrument_id=0;instrument_id<sz;instrument_id++)
      {
        Instrumentation &ins=(*inst_p)[instrument_id];
        ins.resetEquivalent();
      }
    }
  }


  // generate equivalences
  for (QHash<QString,QStringList>::const_iterator it_source=  modules_per_sources.begin();
     it_source!=modules_per_sources.end();
     ++it_source)
  {
     const QString     &source  = it_source.key();
     const QStringList &modules = it_source.value();
     if (modules.count()==1)
        continue; // no equivalences
     QStringList::const_iterator it_module=modules.begin();
     const QString &module = *it_module;
     CSMesInstrumentations::Instrumentations *inst_p= &(_instrumentations.modules[module].sources[source].instrumentations);
     int sz=inst_p->size();

     for (++it_module;
        it_module!=modules.end();
        ++it_module)
     {
        const QString &module = *it_module;
        CSMesInstrumentations::Instrumentations *other_inst_p= &(_instrumentations.modules[module].sources[source].instrumentations);
        int instrument_id;

        for (instrument_id=0;instrument_id<sz;instrument_id++)
        {
           Instrumentation &ins=(*inst_p)[instrument_id];
           CSMesInstrumentations::Instrumentations::iterator found=qFind(other_inst_p->begin(),other_inst_p->end(),ins);
           if (found!=other_inst_p->end())
           {
              ASSERT(*found==ins);
              Instrumentation &other_ins=*found;
              ins.addEquivalent(&other_ins);
           }
        }

     }
  }


}

