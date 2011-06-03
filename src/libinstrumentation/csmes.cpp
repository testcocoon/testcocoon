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

#include "csmes.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QBuffer>
#include <QList>
#include "Service.h"
#include "htmlwriter.h"
#include "accesslocker.h"



CSMes::CSMes() : CSMesAnalysis()
{
  csmes_locker=new AccessLocker();
}

CSMes::~CSMes()
{
  delete csmes_locker;
}

QList<CSMesFunctionInfo::functionskey_t> CSMes::FunctionsReference() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::FunctionsReference();
}

QVector<FunctionInfo> CSMes::FunctionInfoSourceReference( const ModuleFile &module, const SourceFile &source) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::FunctionInfoSourceReference(module,source);
}

const ModuleFiles& CSMes::ModulesReference() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::ModulesReference();
}

QString CSMes::relativeSourceNameReference(const QString &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::relativeSourceNameReference(src);
}

SourceFiles CSMes::SourcesReference(const SourceFile &s) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::SourcesReference(s);
}

const SourceFiles& CSMes::HeadersReference() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::HeadersReference();
}

const SourceFiles& CSMes::SourcesReference(source_filter_t f) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::SourcesReference(f);
}

QString  CSMes::explanation (const ModuleFile &module,const SourceFile &source,const QList<int> &indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit) const
{
  AccessLockerRead lock(csmes_locker);
  return  CSMesAnalysis::explanation (module,source,indexs,source_type,coverage_level,method,executed_by_limit) ;
}

const ModuleFiles& CSMes::Modules() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::Modules();
}

SourceFiles CSMes::Sources(const SourceFile &s) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::Sources(s);
}

const SourceFiles& CSMes::Headers() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::Headers();
}

const SourceFiles& CSMes::Sources(source_filter_t f) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::Sources(f);
}

bool CSMes::moduleExists(const ModuleFile &m) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::moduleExists(m);
}

bool CSMes::modificationFlag() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::modificationFlag();
}

QList<DiffItem> CSMes::differencesWithReference(const ModuleFile &module,const SourceFile &source,source_type_t type) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::differencesWithReference(module,source,type) ;
}

bool CSMes::getManuallyValidated(const ModuleFile &mod,const SourceFile &src,int index) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::getManuallyValidated(mod,src,index) ;
}

void CSMes::setManuallyValidated(const ModuleFile &mod,const SourceFile &src,int index,bool b)
{
  AccessLockerWrite lock(csmes_locker);
  CSMesAnalysis::setManuallyValidated(mod,src,index,b);
}

QString CSMes::relativeSourceName(const QString &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesInstrumentation::relativeSourceName(src);
}

QList<int> CSMes::instrumentedLinesPre(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,const ModuleFile &mod,const SourceFile &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::instrumentedLinesPre(level,method,filter,mod,src) ;
}

QList<int> CSMes::instrumentedLinesOrg(int level,Instrumentation::coverage_method_t method,Instrumentation::filter_t filter,const ModuleFile &mod,const SourceFile &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::instrumentedLinesOrg(level,method,filter,mod,src) ;
}

QList<int> CSMes::commentedLinesPre(const ModuleFile &mod,const SourceFile &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::commentedLinesPre(mod,src) ;
}

QList<int> CSMes::commentedLinesOrg(const ModuleFile &mod,const SourceFile &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::commentedLinesOrg(mod,src) ;
}


bool CSMes::statisticFunctionsExecution(const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested,QHash<CSMesFunctionInfo::functionskey_t,int> &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statisticFunctionsExecution(ms,coverage_level,method,nb_tested,nb_untested) ;
}

bool CSMes::statisticSourcesExecution(const SourceFiles &sources,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,QHash<QString,int> &nb_tested,QHash<QString,int> &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statisticSourcesExecution(sources,ms,coverage_level,method,nb_tested,nb_untested) ;
}


bool CSMes::statistic(const ModuleFile &mod,const SourceFile &src,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statistic(mod,src,coverage_level,method,nb_tested,nb_untested) ;
}

bool CSMes::statistic(int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statistic(coverage_level,method,nb_tested,nb_untested) ;
}

bool CSMes::statisticExecution(const QStringList &mes,const QStringList &cmp,bool execution_analysis,int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t comparaison,int &nb_tested,int &nb_untested,const bool &abort_operation) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::statisticExecution(mes,cmp,execution_analysis,coverage_level,method,comparaison,nb_tested,nb_untested,abort_operation) ;
}

bool CSMes::statisticModuleExecution(ModuleFile module,SourceFile source,const QStringList &ms,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statisticModuleExecution(module,source,ms,coverage_level,method,nb_tested,nb_untested) ;
}

bool CSMes::statisticFunctionPre(ModuleFile module,SourceFile source,long start_line,long start_column,long end_line,long end_column,int coverage_level,Instrumentation::coverage_method_t method,int &nb_tested,int &nb_untested) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesStatistic::statisticFunctionPre(module,source,start_line,start_column,end_line,end_column,coverage_level,method,nb_tested,nb_untested) ;
}

QStringList CSMes::executionList() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::executionList() ;
}

bool CSMes::setExecutionComment(const QString &name, const QString &comment)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesExecution::setExecutionComment(name, comment) ;
  return ret;
}

bool CSMes::getExecutionComment(const QString &name, QString &comment) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::getExecutionComment(name, comment) ;
}

bool CSMes::deleteExecution(const QString &v)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesExecution::deleteExecution(v) ;
  return ret;
}

bool CSMes::renameExecution(const QString &old_name,const QString &new_name)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesExecution::renameExecution(old_name,new_name) ;
  return ret;
}


void CSMes::restoreExecution(const QString &name,const Executions::modules_executions_private_t &exec)
{
  AccessLockerWrite lock(csmes_locker);
  CSMesExecution::restoreExecution(name,exec);
}

bool CSMes::backupExecution(const QString &name,Executions::modules_executions_private_t &exec) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::backupExecution(name,exec);
}

bool CSMes::mergeExecutions(const QStringList &sources,const QString &dest)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesExecution::mergeExecutions(sources,dest);
  return ret;
}

bool CSMes::executionExists(const QString &m) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::executionExists(m) ;
}

bool CSMes::executionPathExists(const QString &m) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::executionPathExists(m) ;
}

void CSMes::setExecutionStatus(const QString &name,Executions::execution_status_t e)
{
  AccessLockerWrite lock(csmes_locker);
  CSMesExecution::setExecutionStatus(name,e);
}

Executions::execution_status_t CSMes::getExecutionStatus(const QString &name) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::getExecutionStatus(name) ;
}

QString CSMes::getExecutionStatusStr(const QString &name) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::getExecutionStatusStr(name) ;
}

void CSMes::setExecutionStatusStr(const QString &name,const QString &execution_status)
{
  AccessLockerWrite lock(csmes_locker);
  CSMesExecution::setExecutionStatusStr(name,execution_status) ;
}

bool CSMes::selectExecutionsComparaison(CSMesInstrumentations &_instrumentations,const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t methode, comparaison_mode_t m) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::selectExecutionsComparaison(_instrumentations,ms,comparaison,test_coverage_mode, methode,m,false) ;
}

bool CSMes::selectExecutionsComparaison(const QStringList &ms,const QStringList &comparaison,bool test_coverage_mode, Instrumentation::coverage_method_t method, comparaison_mode_t m)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::selectExecutionsComparaison(ms,comparaison,test_coverage_mode, method,m,false) ;
  return ret;
}

QStringList CSMes::executedBy(const ModuleFile& module,const SourceFile & source,int index,bool selected_executions_only) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesExecution::executedBy(module,source,index,selected_executions_only) ;
}


QString CSMes::exportHtmlStatisticFunction(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlStatisticFunction(stream,coverage_level,method,all_levels, first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight, watermark_low_medium, watermark_medium_high,sort_coverage,bargraph) ;
}

bool CSMes::exportHtmlReport(
    DocumentType_t document_type,
    const QString &file, const QString &title, const QString &icon, const QString &css_file,
    DisplayLevelType_t global_coverage_display_mode,
    DisplayLevelType_t method_coverage_display_mode,
    DisplaySortingType_t method_coverage_sorting_mode,
    DisplayLevelType_t source_coverage_display_mode,
    DisplaySortingType_t source_coverage_sorting_mode,
    DisplayLevelType_t execution_coverage_display_mode,
    DisplaySortingType_t execution_coverage_sorting_mode,
    int executions_level, bool toc, bool code_fragments_manually_validated, bool code_fragments_unexecuted, bool code_fragments_executed, bool executions_filter_active, double executions_filter_min, double executions_filter_max,  bool sources_filter_active, double sources_filter_min, double sources_filter_max, bool methods_filter_active, double methods_filter_min, double methods_filter_max, double executions_watermark_medium_level, double executions_watermark_low_level, double global_coverage_watermark_medium_level, double global_coverage_watermark_low_level, int global_coverage_max_intermediate_levels, int methods_max_intermediate_levels, int executions_max_intermediate_levels, int sources_max_intermediate_levels, int  global_coverage_level , Instrumentation::coverage_method_t coverage_method, int methods_level, double methods_watermark_low_level, double methods_watermark_medium_level, double sources_watermark_low_level, double sources_watermark_medium_level, bool test_count_mode,
    bool bargraph
    )const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlReport(
      document_type,
      file,
      title,
      icon,
      css_file,
      global_coverage_display_mode,
      method_coverage_display_mode,
      method_coverage_sorting_mode,
      source_coverage_display_mode,
      source_coverage_sorting_mode,
      execution_coverage_display_mode,
      execution_coverage_sorting_mode,
      executions_level,
      toc,
      code_fragments_manually_validated,
      code_fragments_unexecuted,
      code_fragments_executed,
      executions_filter_active,
      executions_filter_min,
      executions_filter_max,
      sources_filter_active,
      sources_filter_min,
      sources_filter_max,
      methods_filter_active,
      methods_filter_min,
      methods_filter_max,
      executions_watermark_medium_level,
      executions_watermark_low_level,
      global_coverage_watermark_medium_level,
      global_coverage_watermark_low_level,
      global_coverage_max_intermediate_levels,
      methods_max_intermediate_levels,
      executions_max_intermediate_levels,
      sources_max_intermediate_levels,
      global_coverage_level ,
      coverage_method,
      methods_level,
      methods_watermark_low_level,
      methods_watermark_medium_level,
      sources_watermark_low_level,
      sources_watermark_medium_level,
      test_count_mode,
      bargraph
        );
}

QString CSMes::exportHtmlStatisticExecutions(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlStatisticExecutions(stream,coverage_level,method,all_levels, first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight, watermark_low_medium, watermark_medium_high,sort_coverage,bargraph);
}

QString CSMes::exportHtmlStatisticSources(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlStatisticSources(stream,coverage_level,method,all_levels, first_level,max_number_of_levels,exclude_coverage_low,exclude_coverage_hight, watermark_low_medium, watermark_medium_high,sort_coverage,bargraph);
}

QString CSMes::exportEMMAStatistic(const QString &filename,int coverage_level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesEmma::exportEMMAStatistic(filename,coverage_level,method);
}

QString CSMes::exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesCsv::exportCSVStatisticFunction(filename,separator,floatSep,coverage_level,method);
}

QString CSMes::exportTextReport(const QString &filename,const QString & format_executed,const QString & format_unexecuted,int coverage_level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesTextReport::exportTextReport(filename,format_executed,format_unexecuted,coverage_level,method);
}

QString CSMes::exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesCsv::exportCSVStatisticModule(filename,separator,floatSep,coverage_level,method);
}

QString CSMes::exportHtmlStatisticGlobalCoverage(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float watermark_low_medium, float watermark_medium_high,bool bargraph) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlStatisticGlobalCoverage(stream,coverage_level,method,all_levels, first_level,max_number_of_levels,watermark_low_medium, watermark_medium_high,bargraph);
}

QString CSMes::exportHtmlExecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlExecutedCode(stream,level,method);
}

QString CSMes::exportHtmlUnexecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlUnexecutedCode(stream,level,method);
}

QString CSMes::exportHtmlManuallyValidated(Writer &stream,int level,Instrumentation::coverage_method_t method) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesReport::exportHtmlManuallyValidated(stream,level,method);
}

QList<CSMes::functionskey_t> CSMes::Functions() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesFunctionInfo::Functions();
}

QVector<FunctionInfo> CSMes::FunctionInfoSource( const ModuleFile& module, const SourceFile& source) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesFunctionInfo::FunctionInfoSource( module, source) ;
}

bool CSMes::instrumentationListFunctionPre(const ModuleFile &module,const SourceFile &source,long start_line,long start_column,long end_line,long end_column,QList<int> &instrumentation_list) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesFunctionInfo::instrumentationListFunctionPre(module,source,start_line,start_column,end_line,end_column,instrumentation_list) ;
}

void CSMes::toRTFSyntax(const ModuleFile & module,const SourceFile & source,int level,Instrumentation::coverage_method_t a,source_type_t b,bool tooltip,int executed_by_limit,QTextDocument &out) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::toRTFSyntax(module,source,level,a,b,tooltip,executed_by_limit,out);
}

void CSMes::toRTFSyntax(const ModuleFile & module,const SourceFile & source,Instrumentation::coverage_method_t a,source_type_t b,QString &out) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::toRTFSyntax(module,source,a,b,out) ;
}

QString CSMes::getFilenameReference() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::getFilenameReference();
}

QString CSMes::getFilename() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::getFilename();
}

const QVector<Instrumentation> CSMes::instrumentationList(const ModuleFile &mod,const SourceFile &src) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::instrumentationList(mod,src) ;
}

QString CSMes::extractPlainTextPreprocessed(const ModuleFile & module,const SourceFile & source,int line1,int col1,int line2,int col2) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::extractPlainTextPreprocessed(module,source,line1,col1,line2,col2) ;
}

void  CSMes::statusLinesPre(const ModuleFile &mod,const SourceFile &src,int level,Instrumentation::coverage_method_t t,int number_of_lines,QVector<Instrumentation::status_t> &lines) const
{
  AccessLockerRead lock(csmes_locker);
  return  CSMesRTF::statusLinesPre(mod,src,level,t ,number_of_lines,lines);
}

void  CSMes::statusLinesOrg(const ModuleFile &mod,const SourceFile &src,int level,Instrumentation::coverage_method_t t,int number_of_lines,QVector<Instrumentation::status_t> &lines) const
{
  AccessLockerRead lock(csmes_locker);
  return  CSMesRTF::statusLinesOrg(mod,src,level,t ,number_of_lines,lines);
}

void CSMes::getExplanationFromIndex (Writer &stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t t,int executed_by_limit) const
{
  AccessLockerRead lock(csmes_locker);
  CSMesRTF::explanationFromIndex (stream,module,source,index,source_type,coverage_level,t,executed_by_limit) ;
}

void  CSMes::getExplanationFragmentFromIndex (Writer &stream,const ModuleFile &module,const SourceFile &source,int index,source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t v,int executed_by_limit, bool table_fragment) const
{
  AccessLockerRead lock(csmes_locker);
  CSMesRTF::explanationFragmentFromIndex (stream,module,source,index,source_type,coverage_level,v,executed_by_limit, table_fragment) ;
}

void CSMes::getExplanationFromLinePre (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t t,int executed_by_limit) const
{
  AccessLockerRead lock(csmes_locker);
  CSMesRTF::explanationFromLinePre (stream,module,source,line,coverage_level,t,executed_by_limit);
}

void CSMes::getExplanationFromLineOrg (Writer &stream,const ModuleFile &module,const SourceFile &source,int line,int coverage_level,Instrumentation::coverage_method_t t,int executed_by_limit) const
{
  AccessLockerRead lock(csmes_locker);
  CSMesRTF::explanationFromLineOrg (stream,module,source,line,coverage_level,t,executed_by_limit);
}

void CSMes::toPLAINReference(const ModuleFile &module,const SourceFile &source,source_type_t type,QString &out) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::toPLAINReference(module,source,type,out);
}

void CSMes::toPLAIN(const ModuleFile & module,const SourceFile & source,source_type_t type,QString &out) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::toPLAIN(module,source,type,out);
}

void CSMes::linesCoverageCountPre(const ModuleFile &mod,const SourceFile &src,QVector<int> &min,QVector<int> &max) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::linesCoverageCountPre(mod,src,min,max);
}

void CSMes::linesCoverageCountOrg(const ModuleFile &mod,const SourceFile &src,QVector<int> &min,QVector<int> &max) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesRTF::linesCoverageCountOrg(mod,src,min,max);
}


bool CSMes::compareCSMes(const QString &file)
{
  AccessLockerWrite lock(csmes_locker);
  return CSMesAnalysis::compareCSMes(file);
}

bool CSMes::loadCSMes(const QString &file)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::loadCSMes(file);
  return ret;
}

bool CSMes::loadCSExe(QIODevice &data,QString name,csexe_import_policy_t p,Executions::execution_status_t default_execution_status,QStringList &new_executions,QString &info,QString &short_status,QString &errmsg,QHash<QString,Executions::modules_executions_private_t> *undo_backup_p,progress_function_t progress)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::loadCSExe(data,name,p,default_execution_status,new_executions,info,short_status,errmsg,undo_backup_p,progress);
  return ret;
}

bool CSMes::mergeCSMes(const QString &s,CSMESFile::merge_policy_t policy)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::mergeCSMes(s,policy);
  return ret;
}

bool CSMes::flushCSMes()
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::flushCSMes();
  return ret;
}

bool CSMes::saveCSMes()
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesAnalysis::saveCSMes();
  return ret;
}


void CSMes::closeCSMes()
{
  AccessLockerWrite lock(csmes_locker);
  CSMesAnalysis::closeCSMes();
}

void CSMes::clear()
{
  AccessLockerWrite lock(csmes_locker);
  CSMesAnalysis::clear();
}

bool CSMes::setComment(const ModuleFile & module,const SourceFile & source,int instrument_index,const QString &comment)
{
  AccessLockerWrite lock(csmes_locker);
  bool ret = CSMesComment::setComment(module,source,instrument_index,comment);
  return ret;
}

QString CSMes::getComment(const ModuleFile & module,const SourceFile & source,int instrument_index) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesComment::getComment(module,source,instrument_index) ;
}

const QString &CSMes::error() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::error();
}


bool CSMes::saveBlackBoxConfiguration(const QString &s)
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::saveBlackBoxConfiguration(s);
}

bool CSMes::isCoverageBranchOnly()
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::isCoverageBranchOnly();
}

bool CSMes::isBlackBoxConfiguration() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::isBlackBoxConfiguration();
}

bool CSMes::isSourceExisting(const ModuleFile &module,const SourceFile &source, CSMes::source_type_t t) const
{
  AccessLockerRead lock(csmes_locker);
  return  CSMesAnalysis::isSourceExisting(module,source, t==CSMES?CSMESFile::PREPROCESSED:CSMESFile::ORIGINAL ) ;
}

CSMesAnalysis::modifications_t CSMes::compareCSMesFunction(const CSMesFunctionInfo::functionskey_t &func,const CSMesFunctionInfo::functionskey_t &func_ref) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::compareCSMesFunction(func,func_ref) ;
}

CSMes::modifications_t CSMes::compareCSMesSource(const ModuleFile &module,const SourceFile &source) const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::compareCSMesSource(module,source);
}

bool  CSMes::hasReference() const
{
  AccessLockerRead lock(csmes_locker);
  return CSMesAnalysis::hasReference();
}

void CSMes::equivalentModulesReference(const ModuleFile &module,const SourceFile &source,ModuleFile &mod,SourceFile &src,ModuleFile &mod_ref,SourceFile &src_ref) const
{
  AccessLockerRead lock(csmes_locker);
  CSMesAnalysis::equivalentModulesReference(module,source,mod,src,mod_ref,src_ref);
}
