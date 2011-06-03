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

#ifndef CSMES_H
#define CSMES_H
#include "csmesanalysis.h"
#include "libinstrumentationpdef.h"
#include <QList>
class AccessLocker;
    
class LIBINSTRUMENTATION_API CSMes : public CSMesAnalysis
{
  public:
    CSMes();
    virtual ~CSMes();

    /* CSMesInstrumentation */
    const ModuleFiles& Modules() const ;
    SourceFiles Sources(const SourceFile &s) const ;
    const SourceFiles& Headers() const ;
    const SourceFiles& Sources(source_filter_t f) const ;
    bool moduleExists(const ModuleFile &m) const;
    bool modificationFlag() const ;
    bool getManuallyValidated(const ModuleFile &mod,const SourceFile &src,int index) const;
    void setManuallyValidated(const ModuleFile &mod,const SourceFile &src,int index,bool b);
    QString relativeSourceName(const QString &src) const;

    /* CSMesAnalysis */
    QString relativeSourceNameReference(const QString &src) const;
    void equivalentModulesReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref) const ;
    QList<DiffItem> differencesWithReference(const ModuleFile &module,const SourceFile &source,source_type_t type) const ;
    modifications_t compareCSMesSource(const ModuleFile &module,const SourceFile &source) const ;
    QList<int> instrumentedLinesPre(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,const ModuleFile &mod,const SourceFile &src) const;
    QList<int> instrumentedLinesOrg(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,const ModuleFile &mod,const SourceFile &src) const;
    QList<int> commentedLinesOrg(const ModuleFile &mod,const SourceFile &src) const;
    QList<int> commentedLinesPre(const ModuleFile &mod,const SourceFile &src) const;
    QString  getFilenameReference() const ;
    const ModuleFiles& ModulesReference() const ;
    bool  hasReference() const ;
    SourceFiles SourcesReference(const SourceFile &s) const ;
    const SourceFiles& HeadersReference() const ;
    const SourceFiles& SourcesReference(source_filter_t) const ;
    QList<functionskey_t> FunctionsReference() const ;

    QVector<FunctionInfo> FunctionInfoSourceReference( const ModuleFile &module, const SourceFile &source) const;
    void toPLAINReference(const ModuleFile &module,const SourceFile &source,source_type_t type,QString &out) const;
    modifications_t compareCSMesFunction(const CSMesFunctionInfo::functionskey_t &func,const CSMesFunctionInfo::functionskey_t &func_ref) const;
    QString  explanation (const ModuleFile &module,const SourceFile &source,const QList<int> &indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const ;

    /* CSMesStatistic */
    bool statisticFunctionsExecution(const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_untested) const ;
    bool statistic(const ModuleFile &mod,const SourceFile &src,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const ;
    bool statistic(int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const ;
    bool statisticSourcesExecution(const SourceFiles &sources,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<QString,int> &nb_tested,QHash<QString,int> &nb_untested) const;
    
    static QString printStat(double v) { return CSMesStatistic::printStat(v) ; }
    static QString printStat(int nb_tested,int nb_untested) { return CSMesStatistic::printStat(nb_tested,nb_untested) ; }
    bool statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t m,int &nb_tested,int &nb_untested,const bool &abort_operation) const ;
    bool statisticModuleExecution(ModuleFile module,SourceFile source,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const ;
    bool statisticFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const ;

    /* CSMesExecution */
    void restoreExecution(const QString &name,const Executions::modules_executions_private_t &exec);
    bool backupExecution(const QString &execution_name,Executions::modules_executions_private_t &execution) const;
    QStringList executionList() const;
    bool setExecutionComment(const QString &name, const QString &comment) ;
    bool getExecutionComment(const QString &name, QString &comment) const ;
    bool deleteExecution(const QString &) ;
    bool renameExecution(const QString &old_name,const QString &new_name) ;
    bool mergeExecutions(const QStringList &sources,const QString &dest);
    bool executionExists(const QString &m) const ; 
    bool executionPathExists(const QString &m) const ;
    void setExecutionStatus(const QString &name,Executions::execution_status_t); 
    Executions::execution_status_t getExecutionStatus(const QString &name) const; 
    static QStringList executionsStatusStr() { return CSMesExecution::executionsStatusStr(); }
    QString getExecutionStatusStr(const QString &name) const; 
    void setExecutionStatusStr(const QString &name,const QString &execution_status) ; 
    bool selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode, comparaison_mode_t m) const;
    bool selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method, comparaison_mode_t m) ;
    QStringList executedBy(const ModuleFile& module,const SourceFile & source,int index,bool selected_executions_only) const;
    static bool executionNameValid(const QString &name,QString &explanation) { return CSMesExecution::executionNameValid(name,explanation); }
    static QColor executionStatusColor(Executions::execution_status_t v) { return CSMesExecution::executionStatusColor(v);}


    /* CSMesReport */
bool exportHtmlReport(
    DocumentType_t document_type,
    const QString &file,
    const QString &title,
    const QString &icon,
    const QString &css_file,
    DisplayLevelType_t global_coverage_display_mode,
    DisplayLevelType_t method_coverage_display_mode,
    DisplaySortingType_t method_coverage_sorting_mode,
    DisplayLevelType_t source_coverage_display_mode,
    DisplaySortingType_t source_coverage_sorting_mode,
    DisplayLevelType_t execution_statistic_display_type,
    DisplaySortingType_t execution_statistic_sort_type,
    int executions_level,
    bool toc,
    bool code_fragments_manually_validated,
    bool code_fragments_unexecuted,
    bool code_fragments_executed,
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
    bool bargraph
    )const ;
    QString exportHtmlStatisticFunction(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportHtmlStatisticExecutions(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportHtmlStatisticSources(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const;
    QString exportEMMAStatistic(const QString &filename,int coverage_level,Instrumentation::coverage_method_t method) const;
    QString exportTextReport(const QString &filename,const QString & format_executed,const QString & format_unexecuted,int coverage_level,Instrumentation::coverage_method_t method) const;
    QString exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const;
    QString exportHtmlStatisticGlobalCoverage(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float watermark_low_medium, float watermark_medium_high,bool bargraph) const;
    QString exportHtmlUnexecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method) const ;
    QString exportHtmlExecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method) const ;
    QString exportHtmlManuallyValidated(Writer &stream,int level,Instrumentation::coverage_method_t method) const ;

    /* CSMesFunctionInfo */
    QList<functionskey_t> Functions() const ;

    QVector<FunctionInfo> FunctionInfoSource( const ModuleFile &module, const SourceFile &source) const;
    bool instrumentationListFunctionPre(const ModuleFile &module,const SourceFile &source,long start_line,long start_column,long end_line,long end_column,QList<int> &instrumentation_list) const;

    /* CSMesRTF */
    void toRTFSyntax(const ModuleFile & module,const SourceFile & source,int level,Instrumentation::coverage_method_t ,source_type_t,bool tooltip,int executed_by_limit,QTextDocument &out) const;
    void toRTFSyntax(const ModuleFile &module,const SourceFile &source,Instrumentation::coverage_method_t ,source_type_t,QString &out) const;
    const QString &error() const ;
    const QVector<Instrumentation> instrumentationList(const ModuleFile &mod,const SourceFile &src) const;
    QString extractPlainTextPreprocessed(const ModuleFile &module,const SourceFile &source,int line1,int col1,int line2,int col2) const;
    void  statusLinesPre(const ModuleFile &mod,const SourceFile &src,int level,Instrumentation::coverage_method_t ,int number_of_lines,QVector<Instrumentation::status_t> &lines) const;
    void  statusLinesOrg(const ModuleFile &mod,const SourceFile &src,int level,Instrumentation::coverage_method_t ,int number_of_lines,QVector<Instrumentation::status_t> &lines) const;
    void getExplanationFromIndex (Writer &stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void getExplanationFragmentFromIndex (Writer &stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t, int executed_by_limit, bool table_fragment) const;
    void getExplanationFromLinePre (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void getExplanationFromLineOrg (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit) const;
    void toPLAIN(const ModuleFile &module,const SourceFile &source,source_type_t type,QString &out) const;
    void linesCoverageCountPre(const ModuleFile &mod,const SourceFile &src,QVector<int> &min,QVector<int> &max) const;
    void linesCoverageCountOrg(const ModuleFile &mod,const SourceFile &src,QVector<int> &min,QVector<int> &max) const;

    /* CSMesIO */
    bool compareCSMes(const QString &file);
    bool loadCSMes(const QString &file);
    bool loadCSExe(QIODevice &data,QString name,csexe_import_policy_t,Executions::execution_status_t default_execution_status,QStringList &new_executions,QString &info,QString &short_status,QString &errmgs,QHash<QString,Executions::modules_executions_private_t> *undo_backup_p,progress_function_t progress);
    bool mergeCSMes(const QString &s,CSMESFile::merge_policy_t policy);
    bool saveBlackBoxConfiguration(const QString &s) ;
    bool isBlackBoxConfiguration() const ;
    bool isCoverageBranchOnly() ;
    bool saveCSMes();
    bool flushCSMes();
    void closeCSMes();
    void clear();
    bool isSourceExisting(const ModuleFile &module,const SourceFile &source, CSMes::source_type_t t) const;
    QString  getFilename() const ;

    /* CSMesComment */
    bool setComment(const ModuleFile &module,const SourceFile &source,int instrument_index,const QString &comment);
    QString getComment(const ModuleFile &module,const SourceFile &source,int instrument_index) const;
  private:
    SourceFiles Sources(bool) const ;
    SourceFiles SourcesReference(bool) const ;
    AccessLocker *csmes_locker;
} ;
#endif
