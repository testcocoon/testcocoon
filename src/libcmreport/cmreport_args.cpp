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

#include "cmreportpdef.h"
#include "cmreport_args.h"
#include "version.h"
#include <qstring.h>
#include <qmessagebox.h>
#include "getoption.h"
#include <iostream>
using namespace std;


CMReport::CMReport():global_coverage(csmes),selections(csmes)
{
  clear();
}

CMReport &CMReport::instance()
{
  static CMReport instance;
  return instance;
}

void CMReport::clear()
{
  csmes_filename=QString();
  debug=false;
  csmes.clear();
  global_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  method_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  method_coverage_sorting_mode=CSMes::SORT_NONE;
  source_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  source_coverage_sorting_mode=CSMes::SORT_NONE;
  execution_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  execution_coverage_sorting_mode=CSMes::SORT_NONE;
  bargraph=false;
  code_fragments_manually_validated=false;
  code_fragments_unexecuted=false;
  code_fragments_executed=false;
  executions_filter_active=false;
  methods_filter_active=false;
  sources_filter_active=false;
  toc=false;
  executions_filter_max=100.0;
  executions_filter_min=0.0;
  executions_watermark_low_level=30.0;
  executions_watermark_medium_level=70.0;
  global_coverage_watermark_low_level=30.0;
  global_coverage_watermark_medium_level=70.0;
  methods_filter_max=100.0;
  methods_filter_min=0.0;
  methods_watermark_low_level=30.0;
  export_txt_dump_file=false;
  methods_watermark_medium_level=70.0;
  sources_filter_max=100.0;
  sources_filter_min=0.0;
  sources_watermark_low_level=30.0;
  sources_watermark_medium_level=70.0;
  executions_level=1;
  executions_max_intermediate_levels=5;
  global_coverage_max_intermediate_levels=5;
  methods_level=1;
  methods_max_intermediate_levels=5;
  sources_max_intermediate_levels=5;
  css_file.clear();
  csv_module.clear();
  csv_funtion.clear();
  emma.clear();
  output_file.clear();
  txt_dump_file.clear();
  document_type=CSMes::DOCUMENT_HTML;
  icon.clear();
  format_executed.clear();
  format_unexecuted.clear();
  title.clear();
  csv_field_separator=',';
  csv_float_separator='.';
}

static CSMes::DisplayLevelType_t StringToDisplayLevel(const QString &s)
{
  if (s=="all")
    return CSMes::ALL_LEVEL;
  if (s=="selected")
    return CSMes::SELECTED_LEVEL;
  if (s=="selected_and_first")
    return CSMes::SELECTED_LEVEL_AND_LEVEL_1;
  return CSMes::DISPLAY_NOTHING;
}


static CSMes::DisplaySortingType_t StringToDisplaySorting(const QString &s)
{
  if (s=="name")
    return CSMes::SORT_NAME;
  if (s=="coverage")
    return CSMes::SORT_COVERAGE;
  return CSMes::SORT_NONE;
}

static QString DisplaySortingToString(CSMes::DisplaySortingType_t l)
{
  switch (l)
  {
    case CSMes::SORT_NAME:
      return QObject::tr("name");
    default:
    case CSMes::SORT_NONE:
      return QObject::tr("disabled");
    case CSMes::SORT_COVERAGE:
      return QObject::tr("coverage");
  }
}

static QString DisplayLevelToString(CSMes::DisplayLevelType_t l)
{
  switch (l)
  {
    default:
    case CSMes::DISPLAY_NOTHING:
      return QObject::tr("no");
    case CSMes::ALL_LEVEL:
      return QObject::tr("all levels");
    case CSMes::SELECTED_LEVEL:
      return QObject::tr("selected level");
    case CSMes::SELECTED_LEVEL_AND_LEVEL_1:
      return QObject::tr("selected level and level 1");
  }
}


bool CMReport::getoption_select_execution(const char*exp)
{
  CMReport::instance().selections.selectRegex(QString(exp));
  return true;
}

bool CMReport::getoption_branch_only(const char*)
{
  CMReport::instance().selections.setCoverageMethod(Instrumentation::COVERAGE_BRANCH);
  CMReport::instance().global_coverage.setCoverageMethod(Instrumentation::COVERAGE_BRANCH);
  return true;
}

bool CMReport::getoption_set_test_count_mode(const char*)
{
  CMReport::instance().selections.setTestCountMode(true);
  return true;
}

bool CMReport::getoption_debug(const char*)
{
  CMReport::instance().debug=true;
  return true;
}

bool CMReport::getoption_set_level(const char*exp)
{
  CMReport::instance().global_coverage.setCoverageLevel(atoi(exp));
  return true;
}

bool CMReport::getoption_set_level_method(const char*exp)
{
  CMReport::instance().methods_level=atoi(exp);
  return true;
}

bool CMReport::getoption_set_level_execution(const char*exp)
{
  CMReport::instance().executions_level=atoi(exp);
  return true;
}

bool CMReport::getoption_set_sublevels_execution(const char*exp)
{
  CMReport::instance().executions_max_intermediate_levels=atoi(exp);
  return true;
}

bool CMReport::getoption_set_sublevels_method(const char*exp)
{
  CMReport::instance().methods_max_intermediate_levels=atoi(exp);
  return true;
}

bool CMReport::getoption_set_sublevels_global(const char*exp)
{
  CMReport::instance().global_coverage_max_intermediate_levels=atoi(exp);
  return true;
}

bool CMReport::getoption_set_sublevels_source(const char*exp)
{
  CMReport::instance().sources_max_intermediate_levels=atoi(exp);
  return true;
}

bool CMReport::getoption_set_source_filter_max(const char*exp)
{
  CMReport::instance().sources_filter_active=true;
  CMReport::instance().sources_filter_max=atof(exp);
  return true;
}


bool CMReport::getoption_set_source_filter_min(const char*exp)
{
  CMReport::instance().sources_filter_active=true;
  CMReport::instance().sources_filter_min=atof(exp);
  return true;
}


bool CMReport::getoption_set_execution_filter_max(const char*exp)
{
  CMReport::instance().executions_filter_active=true;
  CMReport::instance().executions_filter_max=atof(exp);
  return true;
}


bool CMReport::getoption_set_execution_filter_min(const char*exp)
{
  CMReport::instance().executions_filter_active=true;
  CMReport::instance().executions_filter_min=atof(exp);
  return true;
}


bool CMReport::getoption_set_method_filter_max(const char*exp)
{
  CMReport::instance().methods_filter_active=true;
  CMReport::instance().methods_filter_max=atof(exp);
  return true;
}


bool CMReport::getoption_set_method_filter_min(const char*exp)
{
  CMReport::instance(). methods_filter_active=true;
  CMReport::instance().methods_filter_min=atof(exp);
  return true;
}


bool CMReport::getoption_set_method_watermark_medium_high(const char*exp)
{
  CMReport::instance().methods_watermark_medium_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_method_watermark_low_medium(const char*exp)
{
  CMReport::instance().methods_watermark_low_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_execution_watermark_medium_high(const char*exp)
{
  CMReport::instance().executions_watermark_medium_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_execution_watermark_low_medium(const char*exp)
{
  CMReport::instance().executions_watermark_low_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_global_watermark_medium_high(const char*exp)
{
  CMReport::instance().global_coverage_watermark_medium_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_global_watermark_low_medium(const char*exp)
{
  CMReport::instance().global_coverage_watermark_low_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_source_watermark_medium_high(const char*exp)
{
  CMReport::instance().sources_watermark_medium_level=atof(exp);
  return true;
}

bool CMReport::getoption_set_source_watermark_low_medium(const char*exp)
{
  CMReport::instance().sources_watermark_low_level=atof(exp);
  return true;
}

bool CMReport::getoption_deselect_execution(const char*exp)
{
  CMReport::instance().selections.deselectRegex(QString(exp));
  return true;
}

bool CMReport::getoption_set_executed(const char *)
{
  CMReport::instance().code_fragments_executed=true;
  return true;
}

bool CMReport::getoption_set_unexecuted(const char *)
{
  CMReport::instance().code_fragments_unexecuted=true;
  return true;
}

bool CMReport::getoption_set_manually_validated(const char *)
{
  CMReport::instance().code_fragments_manually_validated=true;
  return true;
}

bool CMReport::getoption_set_bargraph(const char *)
{
  CMReport::instance().bargraph=true;
  return true;
}

bool CMReport::getoption_set_toc(const char *)
{
  CMReport::instance().toc=true;
  return true;
}

bool CMReport::getoption_set_icon(const char *f)
{
  CMReport::instance().icon=QString(f);
  return true;
}

bool CMReport::getoption_set_title(const char *f)
{
  CMReport::instance().title=QString(f);
  return true;
}

bool CMReport::getoption_set_global_display_mode(const char *f)
{
  CMReport::instance().global_coverage_display_mode=StringToDisplayLevel(QString(f));
  return (CMReport::instance().global_coverage_display_mode!=CSMes::DISPLAY_NOTHING);
}

bool CMReport::getoption_set_execution_display_mode(const char *f)
{
  CMReport::instance().execution_coverage_display_mode=StringToDisplayLevel(QString(f));
  return (CMReport::instance().execution_coverage_display_mode!=CSMes::DISPLAY_NOTHING);
}

bool CMReport::getoption_set_method_display_mode(const char *f)
{
  CMReport::instance().method_coverage_display_mode=StringToDisplayLevel(QString(f));
  return (CMReport::instance().method_coverage_display_mode!=CSMes::DISPLAY_NOTHING);
}

bool CMReport::getoption_set_source_display_mode(const char *f)
{
  CMReport::instance().source_coverage_display_mode=StringToDisplayLevel(QString(f));
  return (CMReport::instance().source_coverage_display_mode!=CSMes::DISPLAY_NOTHING);
}

bool CMReport::getoption_set_execution_sorting_mode(const char *f)
{
  CMReport::instance().execution_coverage_sorting_mode=StringToDisplaySorting(QString(f));
  return (CMReport::instance().execution_coverage_sorting_mode!=CSMes::SORT_NONE);
}

bool CMReport::getoption_set_method_sorting_mode(const char *f)
{
  CMReport::instance().method_coverage_sorting_mode=StringToDisplaySorting(QString(f));
  return (CMReport::instance().method_coverage_sorting_mode!=CSMes::SORT_NONE);
}

bool CMReport::getoption_set_source_sorting_mode(const char *f)
{
  CMReport::instance().source_coverage_sorting_mode=StringToDisplaySorting(QString(f));
  return (CMReport::instance().source_coverage_sorting_mode!=CSMes::SORT_NONE);
}



bool CMReport::getoption_set_css(const char *f)
{
  CMReport::instance().css_file=QString(f);
  return true;
}

bool CMReport::getoption_format_unexecuted(const char *f)
{
  if (f)
    CMReport::instance().format_unexecuted=QString(f);
  else
    return false;
  return true;
}

bool CMReport::getoption_format_executed(const char *f)
{
  if (f)
    CMReport::instance().format_executed=QString(f);
  else
    return false;
  return true;
}

bool CMReport::getoption_set_csv_float_separator(const char *f)
{
  if (f)
    CMReport::instance().csv_float_separator=QChar(f[0]);
  else
    return false;
  return true;
}

bool CMReport::getoption_set_csv_field_separator(const char *f)
{
  if (f)
    CMReport::instance().csv_field_separator=QChar(f[0]);
  else
    return false;
  return true;
}

bool CMReport::getoption_set_csv_module(const char *f)
{
  CMReport::instance().csv_module=QString(f);
  return true;
}

bool CMReport::getoption_set_emma(const char *f)
{
  CMReport::instance().emma=QString(f);
  return true;
}

bool CMReport::getoption_set_csv_function(const char *f)
{
  CMReport::instance().csv_funtion=QString(f);
  return true;
}

#if OPEN_OFFICE
bool CMReport::getoption_set_odt_file(const char *f)
{
  CMReport::instance().document_type=CSMes::DOCUMENT_OPEN_OFFICE;
  CMReport::instance().output_file=QString(f);
  return true;
}
#endif

bool CMReport::getoption_set_text_dump_file(const char *f)
{
  CMReport::instance().export_txt_dump_file=true;
  CMReport::instance().txt_dump_file=QString(f);
  return true;
}

bool CMReport::getoption_set_xml_file(const char *f)
{
  CMReport::instance().document_type=CSMes::DOCUMENT_XML;
  CMReport::instance().output_file=QString(f);
  return true;
}

bool CMReport::getoption_set_html_file(const char *f)
{
  CMReport::instance().document_type=CSMes::DOCUMENT_HTML;
  CMReport::instance().output_file=QString(f);
  return true;
}

bool CMReport::getoption_csmes_filename(const char *file)
{
  CMReport::instance().csmes_filename=QString(file);
  bool ret = CMReport::instance().csmes.loadCSMes(CMReport::instance().csmes_filename);
  if (!ret)
  {
    cerr 
      << QObject::tr("Error: could not open csmes file").toStdString() 
      << " '" << file << "':" 
      << CMReport::instance().csmes.error().toStdString() 
      << endl;
  }
  return ret;
}

void CMReport::dumpSettings() const
{
  cerr << "Coverage level (--level): " << QString::number(global_coverage.getCoverageLevel()).toStdString() << endl;
  cerr << "Test count mode (--test-coverage): " << (selections.isTestCountMode()?"true":"false") << endl;
  cerr << "Coverage method (--coverage-branch): " << (selections.getCoverageMethod()==Instrumentation::COVERAGE_BRANCH?"branch only":"branch, condition and decision") << endl;
  if (output_file.isEmpty())
  {
    cerr << "No HTML/OpenOffice output file specified. Global coverage is calculated." << endl;
  }
  else
  {
    cerr << "HTML output file specified." << endl;

    cerr << "Global statistics (--global): " << DisplayLevelToString(global_coverage_display_mode).toStdString() << endl;
    if (global_coverage_display_mode!=CSMes::DISPLAY_NOTHING)
    {
      cerr << "Global statistics selected level (--level): " << QString::number(global_coverage.getCoverageLevel()).toStdString() << endl ;
      if (global_coverage_display_mode==CSMes::ALL_LEVEL)
        cerr << "Global statistics number of intermediate levels (--global-sublevels): " << QString::number(global_coverage_max_intermediate_levels).toStdString() << endl ;
      cerr << "Global statistics low/medium watermark (--global-watermark-low-medium): " << QString::number(global_coverage_watermark_low_level).toStdString() << endl;
      cerr << "Global statistics medium/high watermark (--global-watermark-medium-high): " << QString::number(global_coverage_watermark_medium_level).toStdString() << endl;
    }
    cerr << "Method statistics (--method): " << DisplayLevelToString(method_coverage_display_mode).toStdString() << endl;
    if (method_coverage_display_mode!=CSMes::DISPLAY_NOTHING)
    {
      cerr << "Method statistics selected level (--method-level): " << QString::number(methods_level).toStdString() << endl ;
      if (method_coverage_display_mode==CSMes::ALL_LEVEL)
        cerr << "Method statistics number of intermediate levels (--method-sublevels): " << QString::number(methods_max_intermediate_levels).toStdString() << endl ;
      cerr << "Method statistic filter (--method-filter-min/--method-filter-max): " << (methods_filter_active?"true":"false") << endl;
      if (methods_filter_active)
      {
        cerr << "Method statistic filter range: ["<< QString::number(methods_filter_max).toStdString() << " , " << 
          QString::number(methods_filter_min).toStdString() << "]" << endl;
      }
      cerr << "Method statistics low/medium watermark (--method-watermark-low-medium): " << QString::number(methods_watermark_low_level).toStdString() << endl;
      cerr << "Method statistics medium/high watermark (--method-watermark-medium-high): " << QString::number(methods_watermark_medium_level).toStdString() << endl;
      cerr << "Method statistics sorting (--method-sort): " << DisplaySortingToString(method_coverage_sorting_mode).toStdString() << endl;
    }
    cerr << "Source statistics (--source): " << DisplayLevelToString(source_coverage_display_mode).toStdString() << endl;
    if (source_coverage_display_mode!=CSMes::DISPLAY_NOTHING)
    {
      cerr << "Source statistics selected level (--level): " << QString::number(global_coverage.getCoverageLevel()).toStdString() << endl ;
      if (source_coverage_display_mode==CSMes::ALL_LEVEL)
        cerr << "Source statistics number of intermediate levels (--source-sublevels): " << QString::number(sources_max_intermediate_levels).toStdString() << endl ;
      cerr << "Source statistic filter (--source-filter-min/--source-filter-max): " << (sources_filter_active?"true":"false") << endl;
      if (sources_filter_active)
      {
        cerr << "Source statistic filter range: ["<< QString::number(sources_filter_max).toStdString() << " , " << 
          QString::number(sources_filter_min).toStdString() << "]" << endl;
      }
      cerr << "Source statistics low/medium watermark (--source-watermark-low-medium): " << QString::number(sources_watermark_low_level).toStdString() << endl;
      cerr << "Source statistics medium/high watermark (--source-watermark-medium-high): " << QString::number(sources_watermark_medium_level).toStdString() << endl;
      cerr << "Source statistics sorting (--source-sort): " << DisplaySortingToString(source_coverage_sorting_mode).toStdString() << endl;
    }
    cerr << "Execution statistics (--execution): " << DisplayLevelToString(execution_coverage_display_mode).toStdString() << endl;
    if (execution_coverage_display_mode!=CSMes::DISPLAY_NOTHING)
    {
      cerr << "Execution statistics selected level (--execution-level): " << QString::number(executions_level).toStdString() << endl ;
      if (execution_coverage_display_mode==CSMes::ALL_LEVEL)
        cerr << "Execution statistics number of intermediate levels (--execution-sublevels): " << QString::number(executions_max_intermediate_levels).toStdString() << endl ;
      cerr << "Execution statistic filter (--execution-filter-min/--execution-filter-max): " << (executions_filter_active?"true":"false") << endl;
      if (executions_filter_active)
      {
        cerr << "Execution statistic filter range: ["<< QString::number(executions_filter_max).toStdString() << " , " << 
          QString::number(executions_filter_min).toStdString() << "]" << endl;
      }
      cerr << "Execution statistics low/medium watermark (--execution-watermark-low-medium): " << QString::number(executions_watermark_low_level).toStdString() << endl;
      cerr << "Execution statistics medium/high watermark (--execution-watermark-medium-high): " << QString::number(executions_watermark_medium_level).toStdString() << endl;
      cerr << "Execution statistics sorting (--execution-sort): " << DisplaySortingToString(execution_coverage_sorting_mode).toStdString() << endl;
    }
    cerr << "Bargraph (--bargraph): " << (bargraph?"true":"false") << endl;
    cerr << "Code fragments of manually validated instrumentations (--manually-validated): " << (code_fragments_manually_validated?"true":"false") << endl;
    cerr << "Code fragments of unexecuted instrumentations (--unexecuted): " << (code_fragments_unexecuted?"true":"false") << endl;
    cerr << "Code fragments of executed instrumentations (--executed): " << (code_fragments_executed?"true":"false") << endl;
    cerr << "Table of contents (--toc): " << (toc?"true":"false") << endl;
    cerr << "CSS file (--css): " << css_file.toStdString() << endl;
    switch (document_type)
    {
      case CSMes::DOCUMENT_XML:
        cerr << "XML output file (--xml): " << output_file.toStdString() << endl;
        break;
      case CSMes::DOCUMENT_HTML:
        cerr << "HTML output file (--html): " << output_file.toStdString() << endl;
        break;
      case CSMes::DOCUMENT_OPEN_OFFICE:
        cerr << "HTML output file (--odt): " << output_file.toStdString() << endl;
        break;
    }

    cerr << "Icon (--icon): "<< icon.toStdString() << endl;
    cerr << "Title (--title): " << title.toStdString() << endl;
  }
}

bool CMReport::getopt_process(int argc,const char*const* argv)
{
  char error_msg[16000];
  clear();

  getoption_t command_line_arg[] = {
    { GETOPT_HELP, NULL, NULL, "cmreport" , NULL },
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MANDATORY, "-m" , "--csmes" , "CSMes file name" , getoption_csmes_filename},
    { GETOPT_OPTION|GETOPT_HASARG, "-l" , "--level" , "code coverage level" , getoption_set_level},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MULTIPLE|GETOPT_MANDATORY, "-s" , "--select" , "select executions using regular expression" , getoption_select_execution},
    { GETOPT_OPTION|GETOPT_HASARG|GETOPT_MULTIPLE, "-d" , "--deselect" , "deselect executions using regular expression" , getoption_deselect_execution},
    { GETOPT_OPTION|GETOPT_HASARG, "-h" , "--html" , "HTML report output file name" , getoption_set_html_file},
    { GETOPT_OPTION|GETOPT_HASARG, "-x" , "--xml" , "XML report output file name" , getoption_set_xml_file},
    { GETOPT_OPTION|GETOPT_HASARG, "-t" , "--text" , "text report output file name" , getoption_set_text_dump_file},
#if OPEN_OFFICE
    { GETOPT_OPTION|GETOPT_HASARG, "-o" , "--odt" , "OpenOffice report output file name" , getoption_set_odt_file},
#endif
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-level" , "code coverage level for executions (HTML/XML output)" , getoption_set_level_execution},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-level" , "code coverage level for methods (HTML/XML output)" , getoption_set_level_method},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--css" , "CSS style sheet (HTML output)" , getoption_set_css},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--icon" , "Icon (HTML output)" , getoption_set_icon},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--title" , "Title (HTML output)" , getoption_set_title},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--global" , "Global statistics, values=all, selected or selected_and_first (HTML/XML output)" , getoption_set_global_display_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method" , "Method statistics, values=all, selected or selected_and_first (HTML/XML output)" , getoption_set_method_display_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution" , "Execution statistics, values=all, selected or selected_and_first (HTML/XML output)" , getoption_set_execution_display_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source" , "Source file statistics, values=all, selected or selected_and_first (HTML/XML output)" , getoption_set_source_display_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-sort" , "Method sorting, values=name or coverage (HTML/XML output)" , getoption_set_method_sorting_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-sort" , "Execution sorting, values=name or coverage (HTML/XML output)" , getoption_set_execution_sorting_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-sort" , "Source file sorting, values=name or coverage (HTML/XML output)" , getoption_set_source_sorting_mode},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--global-watermark-low-medium" , "Global watermark setting (HTML/XML output)" , getoption_set_global_watermark_low_medium},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--global-watermark-medium-high" , "Global watermark setting (HTML/XML output)" , getoption_set_global_watermark_medium_high},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-watermark-low-medium" , "Source watermark setting (HTML/XML output)" , getoption_set_source_watermark_low_medium},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-watermark-medium-high" , "Source watermark setting (HTML/XML output)" , getoption_set_source_watermark_medium_high},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-watermark-low-medium" , "Method watermark setting (HTML/XML output)" , getoption_set_method_watermark_low_medium},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-watermark-medium-high" , "Method watermark setting (HTML/XML output)" , getoption_set_method_watermark_medium_high},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-watermark-low-medium" , "Execution watermark setting (HTML/XML output)" , getoption_set_execution_watermark_low_medium},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-watermark-medium-high" , "Execution watermark setting (HTML/XML output)" , getoption_set_execution_watermark_medium_high},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-sublevels" , "Maximum number of intermediate levels for sources (HTML/XML output)" , getoption_set_sublevels_source},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-sublevels" , "Maximum number of intermediate levels for executions (HTML/XML output)" , getoption_set_sublevels_execution},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--global-sublevels" , "Maximum number of intermediate levels for global statistics (HTML/XML output)" , getoption_set_sublevels_global},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-sublevels" , "Maximum number of intermediate levels for methods (HTML/XML output)" , getoption_set_sublevels_method},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-filter-min" , "Filtering out source files according the statistic (HTML/XML output)" , getoption_set_source_filter_min},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--source-filter-max" , "Filtering out source files according the statistic (HTML/XML output)" , getoption_set_source_filter_max},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-filter-min" , "Filtering out methods according the statistic (HTML/XML output)" , getoption_set_method_filter_min},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--method-filter-max" , "Filtering out methods according the statistic (HTML/XML output)" , getoption_set_method_filter_max},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-filter-min" , "Filtering out executions according the statistic (HTML/XML output)" , getoption_set_execution_filter_min},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--execution-filter-max" , "Filtering out executions according the statistic (HTML/XML output)" , getoption_set_execution_filter_max},
    { GETOPT_OPTION, "-b" , "--coverage-branch" , "code coverage on branch level only" , getoption_branch_only},
    { GETOPT_OPTION, "-t" , "--test-coverage" , "test count mode" , getoption_set_test_count_mode},
    { GETOPT_OPTION, "-D" , "--debug" , "debug flag" , getoption_debug},
    { GETOPT_OPTION, NULL , "--toc" , "Table of contents (HTML output)" , getoption_set_toc},
    { GETOPT_OPTION, NULL , "--manually-validated" , "Manually validated code fragments (HTML/XML output)" , getoption_set_manually_validated},
    { GETOPT_OPTION, NULL , "--unexecuted" , "Unexecuted code fragments (HTML/XML output)" , getoption_set_unexecuted},
    { GETOPT_OPTION, NULL , "--executed" , "Executed code fragments (HTML/XML output)" , getoption_set_executed},
    { GETOPT_OPTION, NULL , "--bargraph" , "Coverage data displayed in a bargraph (HTML output)" , getoption_set_bargraph},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--emma" , "Generate a EMMA-XML report file" , getoption_set_emma},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--csv-method" , "Generate a CSV report file for each method" , getoption_set_csv_function},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--csv-function" , "Generate a CSV report file for each file" , getoption_set_csv_module},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--csv-field-separator" , "Field separator for a CSV file" , getoption_set_csv_field_separator},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--csv-coma" , "Coma (, or .) used for floats in a CSV file" , getoption_set_csv_float_separator},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--format-executed" , "Line format for executed code fragments" , getoption_format_executed},
    { GETOPT_OPTION|GETOPT_HASARG, NULL , "--format-unexecuted" , "Line format for unexecuted code fragments" , getoption_format_unexecuted},
    { GETOPT_ARGUMENT, NULL , NULL , NULL , NULL},
    { GETOPT_LAST , NULL , NULL , NULL,  NULL }
  };


  if (! getoption_process(command_line_arg,argc,argv,error_msg,sizeof(error_msg)) )
  {
    cerr << "Invalid Command Line Arguments" << endl;
    cerr << error_msg << endl ;
    cerr << getoption_help(argv[0],command_line_arg) << endl;
    return false;
  }
  return true;
}

bool CMReport::analyse_args(int argc,const char*const * argv)
{
  QString tmp;
  if (!getopt_process(argc,argv))
    return false;

  if (debug)
    dumpSettings();
  selections.apply(debug);
  if (output_file.isEmpty() && txt_dump_file.isEmpty()  && csv_module.isEmpty() && emma.isEmpty() && csv_funtion.isEmpty())
  {
    QString global_stat=global_coverage.statistic();
    if (debug)
      cerr << "Coverage:" << global_stat.toStdString() << "%" << endl;

    cout << global_stat.toStdString() << endl;
  }

  if (!emma.isEmpty())
  {
    QString err_msg=csmes.exportEMMAStatistic(
          emma,
          global_coverage.getCoverageLevel(),
          selections.getCoverageMethod()
          );
    if (!err_msg.isEmpty())
    {
      cerr << "Error: could not generate EMMA-XML file" <<endl;
      cerr << err_msg.toStdString() << endl;
      return false;
    }
  }
  if (!csv_funtion.isEmpty())
  {
    QString err_msg=csmes.exportCSVStatisticFunction(
          csv_funtion,
          csv_field_separator,
          csv_float_separator,
          global_coverage.getCoverageLevel(),
          selections.getCoverageMethod()
          );
    if (!err_msg.isEmpty())
    {
      cerr << "Error: could not generate CSV file" <<endl;
      cerr << err_msg.toStdString() << endl;
      return false;
    }
  }
  if (!csv_module.isEmpty())
  {
    QString err_msg=csmes.exportCSVStatisticModule(
          csv_module,
          csv_field_separator,
          csv_float_separator,
          global_coverage.getCoverageLevel(),
          selections.getCoverageMethod()
          );
    if (!err_msg.isEmpty())
    {
      cerr << "Error: could not generate CSV file" <<endl;
      cerr << err_msg.toStdString() << endl;
      return false;
    }
  }
  if (export_txt_dump_file)
  {
    QString err_msg=csmes.exportTextReport(txt_dump_file,format_executed,format_unexecuted,
          global_coverage.getCoverageLevel(),
          selections.getCoverageMethod());
    if (!err_msg.isEmpty())
    {
      cerr << "Error: could not generate text report file" <<endl;
      cerr << err_msg.toStdString() << endl;
      return false;
    }
  }
  if (!output_file.isEmpty())
  {
    if (!csmes.exportHtmlReport(
          document_type,
          output_file,
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
          global_coverage.getCoverageLevel(),
          selections.getCoverageMethod(),
          methods_level,
          methods_watermark_low_level,
          methods_watermark_medium_level,
          sources_watermark_low_level,
          sources_watermark_medium_level,
          selections.isTestCountMode(),
          bargraph
            ))
          {
            cerr << "Error: could not generate HTML output" <<endl;
            return false;
          }
  }
    return true;
}

const QString &CMReport::arg_csmes_filename()const
{
  return csmes_filename;
}
