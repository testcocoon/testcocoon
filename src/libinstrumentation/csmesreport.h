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

#ifndef CSMES_REPORT_H
#define CSMES_REPORT_H
#include "csmesstatistic.h"
#include <QVector>
#include <QVariant>
#include "writer.h"
class CSMes;

class CSMesReport : public CSMesStatistic
{
  protected:
    CSMesReport();
  public:
    enum DisplayLevelType_t
    {
      DISPLAY_NOTHING,
      ALL_LEVEL,
      SELECTED_LEVEL,
      SELECTED_LEVEL_AND_LEVEL_1
    };

    enum DisplaySortingType_t
    {
      SORT_NAME,
      SORT_COVERAGE,
      SORT_NONE
    };

    enum DocumentType_t
    {
      DOCUMENT_HTML,
      DOCUMENT_OPEN_OFFICE,
      DOCUMENT_XML,
    };
  protected:
bool exportHtmlReport(
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
    bool bargraph,
    int executed_by_limit
    )const ;

    QString exportHtmlStatisticFunction(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportHtmlStatisticExecutions(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportHtmlStatisticSources(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, float watermark_low_medium, float watermark_medium_high,bool sort_coverage,bool bargraph) const;
    QString exportHtmlStatisticGlobalCoverage(Writer &stream,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float watermark_low_medium, float watermark_medium_high,bool bargraph) const;
    QString exportHtmlUnexecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method,int) const ;
    QString exportHtmlExecutedCode(Writer &stream,int level,Instrumentation::coverage_method_t method,int) const ;
    QString exportHtmlManuallyValidated(Writer &stream,int level,Instrumentation::coverage_method_t method,int) const ;
  public:
    enum table_format_type_t
    {
      TABLE_FORMAT_TITLE,
      TABLE_FORMAT_TEXT,
      TABLE_FORMAT_FUNCTION,
      TABLE_FORMAT_STATISTIC_LEVEL,
      TABLE_FORMAT_SOURCE ,
      TABLE_FORMAT_EXECUTION ,
      TABLE_FORMAT_LEVEL ,
      TABLE_FORMAT_SCOPED_NAME ,
      TABLE_FORMAT_SOURCE_RELATIVE ,
      TABLE_FORMAT_PROTOTYPE ,
      TABLE_FORMAT_EXECUTION_STATUS ,
    };
    class table_data_t
    {
      private:
        table_format_type_t _type;
        QVariant _data;
        int _level;
        CSMesFunctionInfo::functionskey_t  _func;
      public:
        void setData(const QVariant &v,table_format_type_t t) { _data=v; _type=t; }
        void setData(int level,const QVariant &v,table_format_type_t t) { _level=level, _data=v; _type=t; }
        void setData(const CSMesFunctionInfo::functionskey_t  &v,table_format_type_t t) { _func=v; _type=t; }
        table_format_type_t type() const { return _type; }
        int level() const { return _level; }
        const QVariant & data() const { return _data; }
        const CSMesFunctionInfo::functionskey_t & func() const { return _func; }
    };
    typedef QVector< QVector<table_data_t> > table_t;
  private:
    void formatHtmlFunctionKey(Writer & stream,const CSMesFunctionInfo::functionskey_t &f) const;
    QString exportStatisticFunction(table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, bool sort_coverage) const;
    QString exportStatisticExecutions(table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, bool sort_coverage) const;
    QString exportStatisticSources(table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels,float exclude_coverage_low,float exclude_coverage_hight, bool sort_coverage) const;
    QString exportStatisticGlobalCoverage(CSMesReport::table_t &table,int coverage_level,Instrumentation::coverage_method_t method,bool all_levels, bool first_level,int max_number_of_levels) const;
    enum code_fragment_type_t { FRAGMENTS_MANUALLY_VALIDATED, FRAGMENTS_UNEXECUTED, FRAGMENTS_EXECUTED };
    QString exportHtmlCodeFragments(Writer &stream,int level,Instrumentation::coverage_method_t method,code_fragment_type_t,int ) const ;
    static bool tableLessThanName(const QVector<CSMesReport::table_data_t> &e1,const QVector<CSMesReport::table_data_t> &e2);
    static bool tableLessThanCoverage(const QVector<CSMesReport::table_data_t> &e1,const QVector<CSMesReport::table_data_t> &e2);
  protected:
    QString exportStatisticFunction(table_t &table,int coverage_level,Instrumentation::coverage_method_t method) const;
    static QVariant StatValue(int nb_tested,int nb_untested);

  private:
static QString exportHtml(Writer::section_type_t section,const CSMesReport::table_t &table,Writer &stream,float watermark_low_medium, float watermark_medium_high,bool bargraph) ;
static inline void generateHeaders(
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
    );

static inline void generateIntro(
    Writer & stream ,
    const CSMes & csmes ,
    Instrumentation::coverage_method_t coverage_method,
    bool test_count_mode
    );

static inline void generateStatisticGlobalCoverage(
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
    );

static inline void generateStatisticSources(
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
    );


static inline void generateStatisticMethods(
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
    );
static inline void generateStatisticExecutions(
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
    );

static inline void generateCodeFragments(
    Writer & stream ,
    const CSMes & csmes ,
    bool  code_fragments ,
    bool  code_fragments_manually_validated ,
    bool  code_fragments_unexecuted ,
    bool  code_fragments_executed ,
    Instrumentation::coverage_method_t coverage_method,
    int executed_by_limit
    );

static inline void generateFooters( Writer & stream );
} ;
#endif
