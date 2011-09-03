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

#ifndef CMREPORT_ARGS_H
#define CMREPORT_ARGS_H
#include <qstring.h>
#include "cmreportpdef.h"
#include "csmes.h"
#include "executionselection.h"
#include "globalcoverage.h"

class CMReport
{
  private:
    CMReport();
  public:
    static CMReport &instance() ;
    bool analyse_args(int argc,const char*const * argv);
    bool getopt_process(int argc,const char* const* argv);
    void clear() ;

    const QString &arg_csmes_filename() const;

  private:

    QString csmes_filename;
    bool debug;
    CSMes csmes;
    ExecutionSelection selections;
    GlobalCoverage global_coverage;
    CSMes::DisplayLevelType_t global_coverage_display_mode;
    CSMes::DisplayLevelType_t method_coverage_display_mode;
    CSMes::DisplaySortingType_t method_coverage_sorting_mode;
    CSMes::DisplayLevelType_t source_coverage_display_mode;
    CSMes::DisplaySortingType_t source_coverage_sorting_mode;
    CSMes::DisplayLevelType_t execution_coverage_display_mode;
    CSMes::DisplaySortingType_t execution_coverage_sorting_mode;
    bool bargraph;
    int executed_by_limit;
    bool code_fragments_manually_validated;
    bool code_fragments_unexecuted;
    bool code_fragments_executed;
    bool executions_filter_active;
    bool methods_filter_active;
    bool sources_filter_active;
    bool toc;
    double executions_filter_max;
    double executions_filter_min;
    double executions_watermark_low_level;
    double executions_watermark_medium_level;
    double global_coverage_watermark_low_level;
    double global_coverage_watermark_medium_level;
    double methods_filter_max;
    double methods_filter_min;
    double methods_watermark_low_level;
    bool export_txt_dump_file;
    double methods_watermark_medium_level;
    double sources_filter_max;
    double sources_filter_min;
    double sources_watermark_low_level;
    double sources_watermark_medium_level;
    int executions_level;
    int executions_max_intermediate_levels;
    int global_coverage_max_intermediate_levels;
    int methods_level;
    int methods_max_intermediate_levels;
    int sources_max_intermediate_levels;
    QString css_file;
    QString csv_module;
    QString csv_funtion;
    QString emma;
    QString output_file;
    QString txt_dump_file;
    CSMes::DocumentType_t document_type;
    QString icon;
    QString format_executed;
    QString format_unexecuted;
    QString title;
    QChar   csv_field_separator;
    QChar   csv_float_separator;


    static bool getoption_select_execution(const char*exp);
    static bool getoption_branch_only(const char*);
    static bool getoption_set_test_count_mode(const char*);
    static bool getoption_debug(const char*);
    static bool getoption_set_level(const char*exp);
    static bool getoption_set_level_method(const char*exp);
    static bool getoption_set_level_execution(const char*exp);
    static bool getoption_set_executed_by_limit(const char*exp);
    static bool getoption_set_sublevels_execution(const char*exp);
    static bool getoption_set_sublevels_method(const char*exp);
    static bool getoption_set_sublevels_global(const char*exp);
    static bool getoption_set_sublevels_source(const char*exp);
    static bool getoption_set_source_filter_max(const char*exp);
    static bool getoption_set_source_filter_min(const char*exp);
    static bool getoption_set_execution_filter_max(const char*exp);
    static bool getoption_set_execution_filter_min(const char*exp);
    static bool getoption_set_method_filter_max(const char*exp);
    static bool getoption_set_method_filter_min(const char*exp);
    static bool getoption_set_method_watermark_medium_high(const char*exp);
    static bool getoption_set_method_watermark_low_medium(const char*exp);
    static bool getoption_set_execution_watermark_medium_high(const char*exp);
    static bool getoption_set_execution_watermark_low_medium(const char*exp);
    static bool getoption_set_global_watermark_medium_high(const char*exp);
    static bool getoption_set_global_watermark_low_medium(const char*exp);
    static bool getoption_set_source_watermark_medium_high(const char*exp);
    static bool getoption_set_source_watermark_low_medium(const char*exp);
    static bool getoption_deselect_execution(const char*exp);
    static bool getoption_set_executed(const char *);
    static bool getoption_set_unexecuted(const char *);
    static bool getoption_set_manually_validated(const char *);
    static bool getoption_set_bargraph(const char *);
    static bool getoption_set_toc(const char *);
    static bool getoption_set_icon(const char *f);
    static bool getoption_set_title(const char *f);
    static bool getoption_set_global_display_mode(const char *f);
    static bool getoption_set_execution_display_mode(const char *f);
    static bool getoption_set_method_display_mode(const char *f);
    static bool getoption_set_source_display_mode(const char *f);
    static bool getoption_set_execution_sorting_mode(const char *f);
    static bool getoption_set_source_sorting_mode(const char *f);
    static bool getoption_set_css(const char *f);
    static bool getoption_format_unexecuted(const char *f);
    static bool getoption_format_executed(const char *f);
    static bool getoption_set_csv_float_separator(const char *f);
    static bool getoption_set_csv_field_separator(const char *f);
    static bool getoption_set_csv_module(const char *f);
    static bool getoption_set_emma(const char *f);
    static bool getoption_set_csv_function(const char *f);
    static bool getoption_set_odt_file(const char *f);
    static bool getoption_set_text_dump_file(const char *f);
    static bool getoption_set_xml_file(const char *f);
    static bool getoption_set_html_file(const char *f);
    static bool getoption_csmes_filename(const char *file);
    static bool getoption_set_method_sorting_mode(const char *f);

    void dumpSettings() const;
};
#endif
