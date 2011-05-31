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

#ifndef CSMES_ANALYSIS_H
#define CSMES_ANALYSIS_H
#include "diffalgo.h"
#include "csmesemma.h"
#include <QHash>

class CSMesAnalysis : public CSMesEmma
{
  public:
    enum modifications_t
    {
      SAME=0,
      SAME_EXCEPT_WHITE_SPACES=1,
      SAME_EXCEPT_COMMENT=2,
      DIFFERENT=3,
      NOT_EXISTING=4,
      NEW=5,
      NOTHING_TO_COMPARE=6,
      MODIFICATIONS_UNDEFINED=256
    } ;
    enum comparaison_mode_t
    {
      COMPARAISON_MODE_NONE=0,
      COMPARAISON_MODE_MODIFIED_FUNCTIONS=1,
      COMPARAISON_MODE_NOT_MODIFIED_FUNCTIONS=2
    };
    LIBINSTRUMENTATION_API static QString modificationsToString(modifications_t);
    LIBINSTRUMENTATION_API static modifications_t combineModifications(modifications_t,modifications_t);
    LIBINSTRUMENTATION_API void getStatisticCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const;
    LIBINSTRUMENTATION_API void setStatisticCacheMaxCost(long value);

  protected:
    CSMesAnalysis();
    virtual ~CSMesAnalysis();
    void clear();
    bool compareCSMes(const QString &file);
    void setManuallyValidated(const QString &mod,const QString &src,int index,bool b);
    QString relativeSourceNameReference(const QString &src) const;
    void equivalentModulesReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref) const ;
    modifications_t compareCSMesSource(const ModuleFile &module,const SourceFile &source) const ;
    bool statisticExecution(const QStringList &ms,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,comparaison_mode_t, int &nb_tested,int &nb_untested,const CSMesInstrumentations &_instrumentations,const bool &abort_operation) const;
    bool statisticExecution(const QStringList &ms,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,comparaison_mode_t comparaison, int &nb_tested,int &nb_untested,const bool &abort_operation) const;
    const QString &getFilenameReference() const ;
    const ModuleFiles& ModulesReference() const ;
    SourceFiles SourcesReference(const SourceFile &s) const ;
    bool  hasReference() const { return csmes_reference_p!=NULL; }
    void toPLAINReference(const QString &module,const QString &source,source_type_t type,QString &out) const;
    const SourceFiles& HeadersReference() const ;
    const SourceFiles& SourcesReference(source_filter_t filter) const ;
    QList<functionskey_t> FunctionsReference() const ;
    QList<DiffItem> differencesWithReference(const QString &module,const QString &source,source_type_t type) const ;

    QVector<FunctionInfo> FunctionInfoSourceReference( const QString &module, const QString &source) const;
    modifications_t compareCSMesFunction(const CSMesFunctionInfo::functionskey_t &func,const CSMesFunctionInfo::functionskey_t &func_ref) const;
    bool loadCSMes(const QString &file);
    bool selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode, comparaison_mode_t m,const bool &abort_operation) const;
    bool selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method, comparaison_mode_t m,const bool &abort_operation) ;
    comparaison_mode_t comparaisonMode() const { return comparaison_mode ; }
    QString  explanation (const QString &module,const QString &source,const QList<int> &indexs,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const ;
  protected:
    QList<int> instrumentedLinesPre(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,ModuleFile mod,SourceFile src) const;
    QList<int> instrumentedLinesOrg(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,ModuleFile mod,SourceFile src) const;
    QList<int> commentedLinesOrg(ModuleFile mod,SourceFile src) const;
    QList<int> commentedLinesPre(ModuleFile mod,SourceFile src) const;

  private:
    CSMesAnalysis *csmes_reference_p;
    mutable QHash<QString,QHash<QString,modifications_t> > module_modifications;
    mutable QHash<QString,QHash<QString,QHash<source_type_t,QList<DiffItem> > > > module_differences;
    void findSourceModuleReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref,ModuleFile &mod_rel,SourceFile &src_rel) const ;
    void modifiedAndNotModifiedFunctions(QList<functionskey_t> &modifiedFunctions,QList<functionskey_t> &notModifiedFunctions) const;
    void hideInstrumentationsOfFunctions(CSMesInstrumentations &_instrumentations,const QList<functionskey_t> &functions) const;
    comparaison_mode_t comparaison_mode;
    mutable QList<functionskey_t> modified_functions;
    mutable QList<functionskey_t> not_modified_functions;
    void processComparaisonMode(CSMesInstrumentations &_instrumentations,comparaison_mode_t m) const;
    void clearStatisticExecutionCache() ;
  private:
    class StatisticExecutionCacheKey
    {
      private:
        comparaison_mode_t _comparaison;
        QStringList _mes;
        QStringList _cmp;
        bool _execution_analysis;
        int _coverage_level;
        Instrumentation::coverage_method_t _method;
        friend inline uint qHash(const CSMesAnalysis::StatisticExecutionCacheKey &f) ;
      public:
        StatisticExecutionCacheKey(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,comparaison_mode_t comparaison)
        {
          _mes=mes;
          _cmp=cmp;
          _execution_analysis=execution_analysis;
          _coverage_level=coverage_level;
          _method=method;
          _comparaison=comparaison;
        }
        bool operator== ( const StatisticExecutionCacheKey & x ) const
        {
          return 
            _mes==x._mes &&
            _cmp==x._cmp &&
            _execution_analysis==x._execution_analysis &&
            _coverage_level==x._coverage_level &&
            _method==x._method &&
            _comparaison==x._comparaison ;
        }
    };
    class StatisticExecutionCacheValue
    {
      private:
        int _nb_tested;
        int _nb_untested;
      public:
        StatisticExecutionCacheValue(int tested, int untested) { _nb_tested=tested;_nb_untested=untested; }
        int nb_tested() const { return _nb_tested; }
        int nb_untested() const { return _nb_untested; }
    };
    friend inline uint qHash(const CSMesAnalysis::StatisticExecutionCacheKey &f) ;
    mutable QCache<StatisticExecutionCacheKey,StatisticExecutionCacheValue> _statistic_execution_cache;
   mutable int _statistic_cache_hit;
   mutable int _statistic_cache_miss;
    SourceFiles SourcesReference(bool) const ;
} ;
#endif
