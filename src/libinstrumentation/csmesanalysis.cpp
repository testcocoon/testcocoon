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

#include "csmesanalysis.h"
#include <QList>
#include <QFile>
#include <QtDebug>
#include <QTextStream>
#include <QTime>
#include <QBuffer>
#include <QMutexLocker>
#include "Service.h"
#include "htmlwriter.h"

CSMesAnalysis::CSMesAnalysis() : CSMesEmma()
{
  comparaison_mode=COMPARAISON_MODE_NONE;
  csmes_reference_p=NULL;
  _statistic_execution_cache.setMaxCost(10000);
  _modified_functions.clear();
  _not_modified_functions.clear();
  _module_modifications.clear();
  _module_differences.clear();
}

CSMesAnalysis::~CSMesAnalysis()
{
  _module_modifications.clear();
  _module_differences.clear();
  _modified_functions.clear();
  _not_modified_functions.clear();
  if (csmes_reference_p)
    delete csmes_reference_p;
}

void CSMesAnalysis::clear()
{
  clearStatisticExecutionCache();
  CSMesEmma::clear();
}


QList<int> CSMesAnalysis::commentedLinesPre(ModuleFile mod,SourceFile src) const
{
  QList<int> ret;
  if (!findSourceModule(mod,src))
    return ret;

  QList<int> comment_index=commentedIndexes(mod,src);
  const CSMesInstrumentations::Instrumentations &inst= instrumentations.modules[mod].sources[src].instrumentations;
  for (QList<int>::const_iterator it= comment_index.begin(); it != comment_index.end(); ++it )
  {
    const Instrumentation *instrumentation_p=&inst[*it];
    for (int line=instrumentation_p->startLinePre();line<=instrumentation_p->endLinePre();line++)
    {
      if (!ret.contains(line))
        ret.append(line);
    }
  }
  qSort(ret);
  return ret;
}

QList<int> CSMesAnalysis::commentedLinesOrg(ModuleFile mod,SourceFile src) const
{
  QList<int> ret;
  if (!findSourceModule(mod,src))
    return ret;

  QList<int> comment_index=commentedIndexes(mod,src);
  const CSMesInstrumentations::Instrumentations &inst= instrumentations.modules[mod].sources[src].instrumentations;
  for (QList<int>::const_iterator it= comment_index.begin(); it != comment_index.end(); ++it )
  {
    const Instrumentation *instrumentation_p=&inst[*it];
    for (int line=instrumentation_p->startLinePre();line<=instrumentation_p->endLinePre();line++)
    {
      if (!ret.contains(line))
        ret.append(line);
    }
  }
  qSort(ret);
  return ret;
}

QList<int> CSMesAnalysis::instrumentedLinesOrg(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,ModuleFile mod,SourceFile src) const
{
  QList<int> ret;
  if (!findSourceModule(mod,src))
    return ret;
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;
  CSMesInstrumentations::Instrumentations::const_iterator it;
  int index=0;
  for (it= inst.begin(); it != inst.end(); ++it )
  {
    bool include=false;
    Instrumentation::status_t st=it->status(level,method);
    switch (filter)
    {
      case Instrumentation::FILTER_NONE:
        include=true;
        break;
      case Instrumentation::FILTER_MANUALLY_VALIDATED:
        if ( st==Instrumentation::STATUS_MANUALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_VALIDATED:
        if ( st==Instrumentation::STATUS_VALIDATED ) 
          include=true;
        else if ( st==Instrumentation::STATUS_PARTIALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_UNVALIDATED:
        if ( st==Instrumentation::STATUS_UNVALIDATED ) 
          include=true;
        else if ( st==Instrumentation::STATUS_PARTIALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_COMMENT:
        {
          QString s=getComment(mod,src,index);
          if (s!="" && s!=QString::null)
            include=true;
        }
        break;
    }
    if (include)
    {
      for (int line=it->startLineOrg();line<=it->endLineOrg();line++)
      {
        if (!ret.contains(line+1))
          ret.append(line+1);
      }
    }
    index++;
  }
  qSort(ret);
  return ret;
}

QList<int> CSMesAnalysis::instrumentedLinesPre(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,ModuleFile mod,SourceFile src) const
{
  QList<int> ret;
  if (!findSourceModule(mod,src))
    return ret;
  const CSMesInstrumentations::Instrumentations &inst=instrumentations.modules[mod].sources[src].instrumentations;
  CSMesInstrumentations::Instrumentations::const_iterator it;
  int index=0;
  for (it= inst.begin(); it != inst.end(); ++it )
  {
    bool include=false;
    Instrumentation::status_t st=it->status(level,method);
    switch (filter)
    {
      case Instrumentation::FILTER_NONE:
        include=true;
        break;
      case Instrumentation::FILTER_MANUALLY_VALIDATED:
        if ( st==Instrumentation::STATUS_MANUALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_VALIDATED:
        if ( st==Instrumentation::STATUS_VALIDATED ) 
          include=true;
        else if ( st==Instrumentation::STATUS_PARTIALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_UNVALIDATED:
        if ( st==Instrumentation::STATUS_UNVALIDATED ) 
          include=true;
        else if ( st==Instrumentation::STATUS_PARTIALLY_VALIDATED ) 
          include=true;
        break;
      case Instrumentation::FILTER_COMMENT:
        {
          QString s=getComment(mod,src,index);
          if (s!="" && s!=QString::null)
            include=true;
        }
        break;
    }
    if (include)
    {
      for (int line=it->startLinePre();line<=it->endLinePre();line++)
      {
        if (!ret.contains(line+1))
          ret.append(line+1);
      }
    }
    index++;
  }
  qSort(ret);
  return ret;
}

bool CSMesAnalysis::loadCSMes(const QString &file)
{
  bool ret = CSMesRTF::loadCSMes(file);
  {
    QMutexLocker locker(&_module_modifications_mutex);
    _module_modifications.clear();
  }
  {
    QMutexLocker locker(&_module_differences_mutex);
    _module_differences.clear();
  }
  {
    QMutexLocker locker(&_modified_functions_mutex);
    _modified_functions.clear();
  }
  {
    QMutexLocker locker(&_not_modified_functions_mutex);
    _not_modified_functions.clear();
  }
  clearStatisticExecutionCache();

  return ret;
}


bool CSMesAnalysis::compareCSMes(const QString &file)
{
  clearStatisticExecutionCache();
  comparaison_mode=COMPARAISON_MODE_NONE;
  if (csmes_reference_p)
  {
    {
      QMutexLocker locker(&_module_modifications_mutex);
      _module_modifications.clear();
    }
    {
      QMutexLocker locker(&_module_differences_mutex);
      _module_differences.clear();
    }
    {
      QMutexLocker locker(&_modified_functions_mutex);
      _modified_functions.clear();
    }
    {
      QMutexLocker locker(&_not_modified_functions_mutex);
      _not_modified_functions.clear();
    }
    delete csmes_reference_p;
    csmes_reference_p = NULL;
  }
  if (file.isEmpty())
    return true;
  csmes_reference_p = new CSMesAnalysis;
  if (csmes_reference_p->loadCSMes(file))
    return true;
  delete csmes_reference_p;
  csmes_reference_p = NULL;
  return false;
}

CSMesAnalysis::modifications_t CSMesAnalysis::compareCSMesFunction(const CSMesFunctionInfo::functionskey_t &func,const CSMesFunctionInfo::functionskey_t &func_ref) const
{
  if (!csmes_reference_p)
    return NOTHING_TO_COMPARE;

  ModuleFile mod;
  SourceFile src=func.source;
  ModuleFile mod_ref;
  SourceFile src_ref=func_ref.source;
  if (src.isEmpty())
    return NOT_EXISTING;
  if (src_ref.isEmpty())
    return NEW;
  if (!findSourceModule(mod,src))
  {
    ASSERT(false);
    return NOT_EXISTING;
  }
  if (!csmes_reference_p->findSourceModule(mod_ref,src_ref))
  {
    ASSERT(false);
    return NEW;
  }

  const FunctionInfo *func_ref_p=&func_ref.method;
  const FunctionInfo *func_p=&func.method;

  if (csmes_reference_p->checksum(mod_ref,src_ref)==checksum(mod,src))
    return SAME;

  QString plain_text;
  toPLAIN(mod,src,CSMES,plain_text);
  QString plain_text_ref;
  toPLAINReference(mod_ref,src_ref,CSMES,plain_text_ref);
  plain_text_ref=extractText(plain_text_ref,func_ref_p->startLinePre(),func_ref_p->startColumnPre(),func_ref_p->endLinePre(),func_ref_p->endColumnPre());
  plain_text=extractText(plain_text,func_p->startLinePre(),func_p->startColumnPre(),func_p->endLinePre(),func_p->endColumnPre());
  differences_t diff=differences(plain_text,plain_text_ref);
  switch (diff)
  {
    default:
      ASSERT(false);
    case DIFF_SAME:
      return SAME;
      break;
    case DIFF_DIFFERENCE_IN_BLANCS:
      return SAME_EXCEPT_WHITE_SPACES;
      break;
    case DIFF_DIFFERENCE_IN_COMMENTS:
      return SAME_EXCEPT_COMMENT;
      break;
    case DIFF_DIFFERENT:
      return DIFFERENT;
      break;
  }
  return SAME;
}

static inline bool compareFileName(const QString &a,const QString &b)
{
   if (a == b)
      return true;
   bool windows_files=a.contains('\\') || b.contains('\\');
   QString sep;
   if (windows_files)
      sep='\\';
   else
      sep='/';

   QString pattern=sep+sep;
   QString aa= a;
   QString bb= b;
   aa= aa.replace(pattern,sep);
   bb= bb.replace(pattern,sep);
   pattern=sep+'.'+sep;
   aa= aa.replace(pattern,sep);
   bb= bb.replace(pattern,sep);
   pattern='.'+sep;
   while (aa.left(2)==pattern)
      aa=aa.mid(2);
   while (bb.left(2)==pattern)
      bb=bb.mid(2);
   return bb==aa;
}

void CSMesAnalysis::findSourceModuleReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref,ModuleFile &mod_rel,SourceFile &src_rel) const 
{
  mod=module;
  mod_ref=module;
  src=source;
  src_ref=source;
  src_rel.clear();
  mod_rel.clear();
  bool module_found=false;
  bool module_ref_found=false;
  if (findSourceModule(mod,src))
  {
    mod_rel=instrumentations.modules[mod].module_relative_name;
    src_rel=SourceFile(instrumentations.modules[mod].source_relative_name[src.toQString()]);
    module_found=true;
  }
  else
  {
    mod.clear();
    src.clear();
  }
  if (!csmes_reference_p)
    return ;

  if (csmes_reference_p->findSourceModule(mod_ref,src_ref))
  {
    mod_rel=csmes_reference_p->instrumentations.modules[mod_ref].module_relative_name;
    src_rel=csmes_reference_p->instrumentations.modules[mod_ref].source_relative_name[src_ref];
    module_ref_found=true;
  }
  else
  {
    mod_ref.clear();
    src_ref.clear();
  }


  if (!module_ref_found && !module_found)
  { // TODO
    mod.clear();
    mod_ref.clear();
    src=source;
    src_ref=source;
    src_rel.clear();
    mod_rel.clear();
    if (findSourceModule(mod,src))
    {
      mod_rel=instrumentations.modules[mod].module_relative_name;
      src_rel=instrumentations.modules[mod].source_relative_name[src];
      module_found=true;
    }
    else
    {
      mod.clear();
      src.clear();
    }

    if (csmes_reference_p->findSourceModule(mod_ref,src_ref))
    {
      mod_rel=csmes_reference_p->instrumentations.modules[mod_ref].module_relative_name;
      src_rel=csmes_reference_p->instrumentations.modules[mod_ref].source_relative_name[src_ref];
      module_ref_found=true;
    }
    else
    {
      mod_ref.clear();
      src_ref.clear();
    }
    return;
  }
  if (!module_ref_found && !module_found)
    return;

  if (!module_ref_found)
  {
    for (CSMesInstrumentations::Modules::const_iterator itMod=csmes_reference_p->instrumentations.modules.begin();
        itMod!=csmes_reference_p->instrumentations.modules.end();
        ++itMod)
    {
      if (compareFileName(mod_rel , (*itMod).module_relative_name) )
      {
        mod_ref= itMod.key();
        for (QHash<QString,QString>::const_iterator itSrc=(*itMod).source_relative_name.begin();
            itSrc!= (*itMod).source_relative_name.end();
            ++itSrc)
        {
          if (compareFileName(src_rel , itSrc.value()) )
          {
            src_ref= itSrc.key();
            return;
          }
        }
      }
    }
  }
  if (!module_found)
  {
    for (CSMesInstrumentations::Modules::const_iterator itMod=instrumentations.modules.begin();
        itMod!=instrumentations.modules.end();
        ++itMod)
    {
      if (compareFileName(mod_rel , (*itMod).module_relative_name) )
      {
        mod= itMod.key();
        for (QHash<QString,QString>::const_iterator itSrc=(*itMod).source_relative_name.begin();
            itSrc!= (*itMod).source_relative_name.end();
            ++itSrc)
        {
          if (compareFileName(src_rel , itSrc.value()) )
          {
            src= itSrc.key();
            return;
          }
        }
      }
    }
  }
}

void CSMesAnalysis::equivalentModulesReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref) const 
{
  ModuleFile mod_rel;
  SourceFile src_rel;

  findSourceModuleReference(module,source,mod,src,mod_ref,src_ref,mod_rel,src_rel) ;
}

CSMesAnalysis::modifications_t CSMesAnalysis::compareCSMesSource(const ModuleFile &module,const SourceFile &source) const 
{
  if (csmes_reference_p)
  {
    ModuleFile mod;
    SourceFile src;
    ModuleFile mod_rel;
    SourceFile src_rel;
    ModuleFile mod_ref;
    SourceFile src_ref;
    findSourceModuleReference(module,source,mod,src,mod_ref,src_ref,mod_rel,src_rel) ;
    if ((src_ref.isEmpty()) && (src.isEmpty()))
      return NOTHING_TO_COMPARE;

    if (src_ref.isEmpty())
      return NEW;
    if (src.isEmpty())
      return NOT_EXISTING;
  

    if (csmes_reference_p->checksum(mod_ref,src_ref)!=checksum(mod,src))
    {
      QMutexLocker locker(&_module_modifications_mutex);
      if (_module_modifications.contains(module))
      {
        if (_module_modifications.value(module).contains(source))
          return _module_modifications.value(module).value(source);
      }
      QString plain_text;
      toPLAIN(mod,src,ORIGINAL,plain_text);
      QString plain_text_ref;
      toPLAINReference(mod_ref,src_ref,ORIGINAL,plain_text_ref);
      differences_t diff=differences(plain_text,plain_text_ref);
      modifications_t modif;
      switch (diff)
      {
        default:
          ASSERT(false);
        case DIFF_SAME:
          modif = SAME;
          break;
        case DIFF_DIFFERENCE_IN_BLANCS:
          modif = SAME_EXCEPT_WHITE_SPACES;
          break;
        case DIFF_DIFFERENCE_IN_COMMENTS:
          modif = SAME_EXCEPT_COMMENT;
          break;
        case DIFF_DIFFERENT:
          modif = DIFFERENT;
          break;
      }
      _module_modifications[module][source]=modif;
      return modif;
    }

    return SAME;
  }
  else
    return NOTHING_TO_COMPARE;
}

QString CSMesAnalysis::modificationsToString(modifications_t t)
{
  switch (t)
  {
    case SAME:
      return QObject::tr("Same");
    case SAME_EXCEPT_WHITE_SPACES:
      return QObject::tr("Different indentation");
    case SAME_EXCEPT_COMMENT:
      return QObject::tr("Different C/C++ comments");
    case DIFFERENT:
      return QObject::tr("Different");
    case NOT_EXISTING:
      return QObject::tr("Not existing");
    case NEW:
      return QObject::tr("New");
    case MODIFICATIONS_UNDEFINED:
      break;
    case NOTHING_TO_COMPARE:
      return "-";
  }
  return "???";
}

const QString &CSMesAnalysis::getFilenameReference() const 
{
  static QString null;
  if (csmes_reference_p)
    return csmes_reference_p->getFilename();
  else
    return null;
}

const ModuleFiles& CSMesAnalysis::ModulesReference() const 
{
  static ModuleFiles null;
  if (csmes_reference_p)
    return csmes_reference_p->Modules();
  else
    return null;
}

SourceFiles CSMesAnalysis::SourcesReference(const SourceFile &s) const 
{
  static SourceFiles null;
  if (csmes_reference_p)
    return csmes_reference_p->Sources(s);
  else
    return null;
}

const SourceFiles& CSMesAnalysis::HeadersReference() const 
{
  static SourceFiles null;
  if (csmes_reference_p)
    return csmes_reference_p->Headers();
  else
    return null;
}

const SourceFiles& CSMesAnalysis::SourcesReference(source_filter_t f) const 
{
  static SourceFiles null;
  if (csmes_reference_p)
    return csmes_reference_p->Sources(f);
  else
    return null;
}


QList<CSMesFunctionInfo::functionskey_t> CSMesAnalysis::FunctionsReference() const
{
   if (csmes_reference_p)
      return csmes_reference_p->Functions();
   else
      return QList<CSMesFunctionInfo::functionskey_t>();
}

QVector<FunctionInfo> CSMesAnalysis::FunctionInfoSourceReference( const ModuleFile &module, const SourceFile &source) const
{
   if (csmes_reference_p)
      return csmes_reference_p->FunctionInfoSource( module, source) ;
   else
      return QVector<FunctionInfo>();
}

void CSMesAnalysis::toPLAINReference(const ModuleFile &module,const SourceFile &source,source_type_t type,QString &out) const
{
   if (csmes_reference_p)
      return csmes_reference_p->toPLAIN(module,source,type,out);
   else
      return out.clear();
}

QList<DiffItem> CSMesAnalysis::differencesWithReference(const ModuleFile &module,const SourceFile &source,source_type_t type) const 
{
  QMutexLocker locker(&_module_differences_mutex);
  if (_module_differences.contains(module))
  {
    if (_module_differences.value(module).contains(source))
    {
      if (_module_differences.value(module).value(source).contains(type))
        return _module_differences.value(module).value(source).value(type);
    }
  }
  ModuleFile mod,mod_ref;
  SourceFile src,src_ref;
  equivalentModulesReference(module,source,mod,src,mod_ref,src_ref);
  QList<DiffItem> diff;
  QString reference,text;
  toPLAINReference(mod_ref,src_ref,type,reference);
  toPLAIN(mod,src,type,text);
  calcDiff(diff,reference,text,DiffWordIgnoreSpacesAndComments);
  _module_differences[module][source][type]=diff;
  return diff;
}

CSMesAnalysis::modifications_t CSMesAnalysis::combineModifications(CSMesAnalysis::modifications_t a,CSMesAnalysis::modifications_t b)
{
  switch (a)
  {
    case SAME:
      switch (b)
      {
        case SAME:
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME_EXCEPT_COMMENT:
        case DIFFERENT:
          return b;
        case NOT_EXISTING:
        case NEW:
          return DIFFERENT;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
        case MODIFICATIONS_UNDEFINED:
          return a;
      }
    case SAME_EXCEPT_WHITE_SPACES:
      switch (b)
      {
        case SAME:
          return SAME_EXCEPT_WHITE_SPACES;
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME_EXCEPT_COMMENT:
        case DIFFERENT:
          return b;
        case NOT_EXISTING:
          return DIFFERENT;
        case NEW:
          return DIFFERENT;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
        case MODIFICATIONS_UNDEFINED:
          return a;
      }
    case SAME_EXCEPT_COMMENT:
      switch (b)
      {
        case SAME:
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME_EXCEPT_COMMENT:
          return SAME_EXCEPT_COMMENT;
        case NOT_EXISTING:
        case NEW:
        case DIFFERENT:
          return DIFFERENT;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
        case MODIFICATIONS_UNDEFINED:
          return a;
      }
    case DIFFERENT:
      switch (b)
      {
        case SAME:
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME_EXCEPT_COMMENT:
        case MODIFICATIONS_UNDEFINED:
        case DIFFERENT:
        case NOT_EXISTING:
        case NEW:
          return DIFFERENT;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
      }
    case NOT_EXISTING:
      switch (b)
      {
        case NOT_EXISTING:
          return NOT_EXISTING;
        case NEW:
        case SAME:
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME_EXCEPT_COMMENT:
        case DIFFERENT:
          return DIFFERENT;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
        case MODIFICATIONS_UNDEFINED:
          return a;
      }
    case NEW:
      switch (b)
      {
        case DIFFERENT:
        case NOT_EXISTING:
          return DIFFERENT;
        case SAME_EXCEPT_COMMENT:
        case SAME_EXCEPT_WHITE_SPACES:
        case SAME:
        case MODIFICATIONS_UNDEFINED:
        case NEW:
          return NEW;
        case NOTHING_TO_COMPARE:
          return NOTHING_TO_COMPARE;
      }
    case NOTHING_TO_COMPARE:
      return NOTHING_TO_COMPARE;
    case MODIFICATIONS_UNDEFINED:
      return b;
  }
  return NOTHING_TO_COMPARE;
}

QString CSMesAnalysis::relativeSourceNameReference(const QString &src) const
{
   if (csmes_reference_p)
      return csmes_reference_p->relativeSourceName(src);
   else
      return QString();
}

void CSMesAnalysis::processComparaisonMode(CSMesInstrumentations &_instrumentations,comparaison_mode_t m) const
{
  if (m==COMPARAISON_MODE_NONE)
    return;

  QMutexLocker locker1(&_modified_functions_mutex);
  QMutexLocker locker2(&_not_modified_functions_mutex);
  if (_modified_functions.isEmpty() && _not_modified_functions.isEmpty())
    modifiedAndNotModifiedFunctions(_modified_functions,_not_modified_functions);
  switch (m)
  {
    default:
    case COMPARAISON_MODE_NONE:
      ASSERT(false);
    case COMPARAISON_MODE_MODIFIED_FUNCTIONS:
      hideInstrumentationsOfFunctions(_instrumentations,_not_modified_functions);
      break;
    case COMPARAISON_MODE_NOT_MODIFIED_FUNCTIONS:
      hideInstrumentationsOfFunctions(_instrumentations,_modified_functions);
      break;
  }
}

void CSMesAnalysis::hideInstrumentationsOfFunctions(CSMesInstrumentations &_instrumentations,const QList<functionskey_t> &functions) const
{
  for (QList<functionskey_t>::const_iterator itfunc=functions.begin();itfunc!=functions.end();++itfunc)
  {
    SourceFile source=(*itfunc).source;
    ModuleFile module;
    if (findSourceModule(module,source))
    {
      QList<int> instrumentation_list;
      if (instrumentationListFunctionPre(module,source,
            (*itfunc).method.startLinePre(),(*itfunc).method.startColumnPre(),(*itfunc).method.endLinePre(),(*itfunc).method.endColumnPre(),
            instrumentation_list))
      {
        ASSERT(_instrumentations.modules.contains(module));
        ASSERT(_instrumentations.modules[module].sources.contains(source));
        CSMesInstrumentations::Instrumentations &inst=_instrumentations.modules[module].sources[source].instrumentations;

        for (QList<int>::const_iterator iti=instrumentation_list.begin();iti!=instrumentation_list.end();++iti)
        {
          Instrumentation &ins=inst[*iti];
          ins.setExecution(Instrumentation::EXECUTION_STATE_NONE);
          for (Instrumentation *equiv=ins.getEquivalent();equiv!=&ins;equiv=equiv->getEquivalent())
            equiv->copyExecutionState(ins);
        }
      }
    }
  }
}

void CSMesAnalysis::modifiedAndNotModifiedFunctions(QList<functionskey_t> &modifiedFunctions,QList<functionskey_t> &notModifiedFunctions) const
{
  const SourceFiles sources_all= Sources(NON_EMPTY);
  SourceFiles sources;
  modifiedFunctions.clear();
  notModifiedFunctions.clear();


  for (SourceFiles::const_iterator itsrc=sources_all.begin();itsrc!=sources_all.end();++itsrc)
  {
    if (sources.contains(*itsrc))
      continue;

    sources.append(*itsrc);
    ModuleFile mod, mod_ref;
    SourceFile src, src_ref;
    equivalentModulesReference(QString(),*itsrc,mod,src,mod_ref,src_ref);
    QVector<FunctionInfo> FunctionsInfo = FunctionInfoSource(QString(),src);
    QVector<FunctionInfo> FunctionsInfoReference = FunctionInfoSourceReference(QString(),src_ref);
    for (QVector<FunctionInfo>::const_iterator itfct=FunctionsInfo.begin();itfct!=FunctionsInfo.end();++itfct)
    {
      bool found=false;
      functionskey_t finfo;
      finfo.source=src;
      finfo.method=*itfct;
      for (QVector<FunctionInfo>::const_iterator itfctref=FunctionsInfoReference.begin();itfctref!=FunctionsInfoReference.end();++itfctref)
      {
        if (   (*itfct).getScopedName()==(*itfctref).getScopedName()
            && (*itfct).getPrototype()==(*itfctref).getPrototype())
        {
          functionskey_t finforef;
          finforef.source=src_ref;
          finforef.method=*itfctref;
          switch (compareCSMesFunction(finfo,finforef))
          {
            case SAME:
            case SAME_EXCEPT_WHITE_SPACES:
            case SAME_EXCEPT_COMMENT:
              found=true;
              notModifiedFunctions.append(finfo);
              break;
            case DIFFERENT:
              found=true;
              modifiedFunctions.append(finfo);
              break;
            case NOT_EXISTING:
              ASSERT(false);
              break;
            case NEW:
              ASSERT(false);
              break;
            case NOTHING_TO_COMPARE:
              ASSERT(false);
              break;
            case MODIFICATIONS_UNDEFINED:
              ASSERT(false);
              break;
          }
          break;
        }
      }
      if (!found)
        modifiedFunctions.append(finfo);
    }
  }
}

bool CSMesAnalysis::selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode, comparaison_mode_t m,const bool &abort_operation) const
{
  bool ret = CSMesExecution::selectExecutionsComparaison(_instrumentations,ms,comparaison,test_coverage_mode, methode,abort_operation) ;
  processComparaisonMode(_instrumentations,m);
  return ret;
}

bool CSMesAnalysis::selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method, comparaison_mode_t m,const bool &abort_operation) 
{
  comparaison_mode=m;
  bool ret = CSMesExecution::selectExecutionsComparaison(ms,comparaison,test_coverage_mode, method,abort_operation) ;
  processComparaisonMode(instrumentations,m);
  return ret;
}

bool CSMesAnalysis::statisticExecution(const QStringList &ms,const QStringList &cmp,bool test_count_mode,int coverage_level,Instrumentation::coverage_method_t method,comparaison_mode_t comparaison, int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations,const bool &abort_operation) const
{
  CSMesInstrumentations __instrumentations ;
  copyInstrumentation(__instrumentations,_instrumentations);

  if (CSMesAnalysis::selectExecutionsComparaison(__instrumentations,ms,cmp,test_count_mode,method,comparaison,abort_operation))
    return statistic(coverage_level,method,nb_tested,nb_untested,__instrumentations);
  else
  {
    nb_tested=-1;
    nb_untested=-1;
    return false;
  }
}

inline uint qHash(const CSMesAnalysis::StatisticExecutionCacheKey &f) 
{
  uint h=0;
  QStringList::const_iterator it;
  for (it=f._mes.begin();it!=f._mes.end();++it)
    h = h ^ qHash(*it);
  for (it=f._cmp.begin();it!=f._cmp.end();++it)
    h = h ^ qHash(*it);
  h = h ^ qHash(f._execution_analysis);
  h = h ^ qHash(f._comparaison);
  h = h ^ qHash(f._method);
  h = h ^ qHash(f._coverage_level);
  return h;
}

bool CSMesAnalysis::statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,comparaison_mode_t comparaison, int &nb_tested,int &nb_untested,const bool &abort_operation) const 
{
  StatisticExecutionCacheKey cache_key(mes,cmp,execution_analysis,coverage_level,method,comparaison);
  QMutexLocker locker(&_statistic_execution_cache_mutex);
  StatisticExecutionCacheValue *statistics_p=_statistic_execution_cache.object(cache_key);
  if (statistics_p==NULL)
  { // cache miss
    _statistic_cache_miss++;
    if ( !statisticExecution(mes,cmp,execution_analysis,coverage_level,method,comparaison,nb_tested,nb_untested,instrumentations,abort_operation))
      return false;
    if (!abort_operation)
    {
      statistics_p=new StatisticExecutionCacheValue(nb_tested,nb_untested);
      if (!_statistic_execution_cache.insert(cache_key,statistics_p,1))
      { // item too big
        //delete source_text_p;
      }
    }
    return true;
  }
  else
  { // cache hit
    _statistic_cache_hit++;
    nb_tested=statistics_p->nb_tested();
    nb_untested=statistics_p->nb_untested();
    return true;
  }
}


void CSMesAnalysis::getStatisticCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const
{
   cache_hit=_statistic_cache_hit;
   cache_miss=_statistic_cache_miss;
   cost=_statistic_execution_cache.totalCost();
   max_cost=_statistic_execution_cache.maxCost();
}

void CSMesAnalysis::setStatisticCacheMaxCost(long value)
{
  QMutexLocker locker(&_statistic_execution_cache_mutex);
  _statistic_execution_cache.setMaxCost(value);
}

void CSMesAnalysis::clearStatisticExecutionCache() 
{
  QMutexLocker locker(&_statistic_execution_cache_mutex);
  _statistic_execution_cache.clear();
  _statistic_cache_hit=0;
  _statistic_cache_miss=0;
}

void CSMesAnalysis::setManuallyValidated(const QString &mod,const QString &src,int index,bool b)
{
  CSMesEmma::setManuallyValidated(mod,src,index,b);
  clearStatisticExecutionCache();
}

QString  CSMesAnalysis::explanation(const ModuleFile &module,const SourceFile &source,const QList<int> &lines_indexs,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  QList<int> indexs;
  const QVector<Instrumentation> &inst=instrumentationList(module,source);
  if (source_type==ORIGINAL)
  {
    if (inst.isEmpty())
      return QString();

    int sz=inst.size();
    QList<int>::const_iterator it;
    for (it=lines_indexs.begin();it!=lines_indexs.end();++it)
    {
      int line=*it;
      for (int i=0;i<sz;i++)
      {
        int s_line=inst.at(i).startLineOrg();
        int e_line=inst.at(i).endLineOrg();
        if ( line>=s_line &&  line<=e_line  )
        {
          if (inst.at(i).getType()==Instrumentation::NOP)
            continue; //ignore
          if (method==Instrumentation::COVERAGE_BRANCH && inst.at(i).getType()!=Instrumentation::COMPUTATION)
            continue; //ignore
          if (!indexs.contains(i))
            indexs.append(i);
        }
      }
    }
  }
  else
    indexs=lines_indexs;

  int rowid=0;
  if (inst.isEmpty())
    return QString();
  QBuffer buffer;
  HTMLWriter stream(&buffer);
  buffer.open(QIODevice::WriteOnly);

  QList<int>::const_iterator it;
  stream.beginHeader();
  stream.setDisplayFooter(false);
  stream.endHeader();
  stream.beginBody();
  stream.begin(Writer::CoverageBrowserExplanationTable);
  for (it=indexs.begin();it!=indexs.end();++it)
  {
    int index=*it;
    if (inst.at(index).getType()==Instrumentation::NOP)
      continue ; //ignore
    if (method==Instrumentation::COVERAGE_BRANCH && inst.at(index).getType()!=Instrumentation::COMPUTATION)
      continue ; //ignore

    Writer::section_type_t sec=Writer::HtmlItemOdd;
    if ((rowid%2)==0)
      sec=Writer::HtmlItemEven;

    {
      WriterSection section(stream,sec);
      explanationFragmentFromIndex(stream,module,source,index,source_type,coverage_level,method,executed_by_limit,true);
    }
    rowid++;
  }
  stream.end(Writer::CoverageBrowserExplanationTable);
  stream.endBody();
  buffer.close();
  return buffer.data();
}
