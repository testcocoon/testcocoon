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

#include "WHTMLReport.h"
#include <QHeaderView>
#include <QDateTime>
#include <QPushButton>
#include <QMessageBox>
#include "ExecutionListModel.h"
#include "coveragebrowserpdef.h"
#include "SortFilterProxy.h"
#include "csmesundoredoframework.h"
#include "Service.h"
#include "options.h"
#include "CoverageSettings.h"
#include <QStringListModel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QTextDocument>

static QStringList old_comments;

WHTMLReport::WHTMLReport(const CSMesUndoRedoFramework *c,QWidget* parent,  Qt::WFlags fl) : QDialog(parent, fl)
{
  document_types[CSMes::DOCUMENT_HTML]=tr("HTML");
  //document_types[CSMes::DOCUMENT_OPEN_OFFICE]=tr("OpenOffice");
  document_types[CSMes::DOCUMENT_XML]=tr("XML");
  csmes_p=c;
  setupUi(this);
  connect(buttons_p, SIGNAL(accepted()), this, SLOT(process()));
  connect(buttons_p, SIGNAL(rejected()), this, SLOT(reject()));
  QPushButton* apply_p = buttons_p->button(QDialogButtonBox::Apply);
  connect(apply_p, SIGNAL(clicked()), this, SLOT(apply()));
#if QT_COMPLETOR
  QStringList lru=Options::get_opt_strlst(QString(),"HTML_REPORT_FILE_NAME_HISTORY");
  file_completor_p = new QCompleter(file_p);
  file_completor_model_p=new QStringListModel(lru,file_completor_p);
  file_completor_p->setModel(file_completor_model_p);
  file_completor_p->setModelSorting(QCompleter::UnsortedModel);
  file_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  file_p->setCompleter( file_completor_p );

  lru=Options::get_opt_strlst(QString(),"HTML_REPORT_TITLE_HISTORY");
  title_completor_p = new QCompleter(title_p);
  title_completor_model_p=new QStringListModel(lru,title_completor_p);
  title_completor_p->setModel(title_completor_model_p);
  title_completor_p->setModelSorting(QCompleter::UnsortedModel);
  title_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  title_p->setCompleter( title_completor_p );

  lru=Options::get_opt_strlst(QString(),"HTML_REPORT_LAYOUT_CSS_HISTORY");
  css_file_completor_p = new QCompleter(css_file_p);
  css_file_completor_model_p=new QStringListModel(lru,css_file_completor_p);
  css_file_completor_p->setModel(css_file_completor_model_p);
  css_file_completor_p->setModelSorting(QCompleter::UnsortedModel);
  css_file_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  css_file_p->setCompleter( css_file_completor_p );

  lru=Options::get_opt_strlst(QString(),"HTML_REPORT_LAYOUT_ICON_HISTORY");
  icon_completor_p = new QCompleter(icon_p);
  icon_completor_model_p=new QStringListModel(lru,icon_completor_p);
  icon_completor_p->setModel(icon_completor_model_p);
  icon_completor_p->setModelSorting(QCompleter::UnsortedModel);
  icon_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  icon_p->setCompleter( icon_completor_p );
#endif

  file_type_p->clear();
  for (QMap<CSMes::DocumentType_t,QString>::const_iterator it=document_types.begin();it!=document_types.end();++it)
  {
    QString item=*it;
    file_type_p->addItem(item);
  }
  file_p->setText(Options::get_opt_str(QString(),"HTML_REPORT_FILE_NAME",file_p->text()));
  file_type_p->setCurrentIndex(Options::get_opt_long(QString(),"HTML_REPORT_FILE_TYPE"));
  css_file_p->setText(Options::get_opt_str(QString(),"HTML_REPORT_LAYOUT_CSS",css_file_p->text()));
  icon_p->setText(Options::get_opt_str(QString(),"HTML_REPORT_LAYOUT_ICON",icon_p->text()));
  title_p->setText(Options::get_opt_str(QString(),"HTML_REPORT_TITLE",title_p->text()));

  bargraph_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_BARGRAPH",bargraph_p->isChecked()));
  statistic_global_coverage_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_STATISTIC_SUMMARY",statistic_global_coverage_p->isChecked()));
  statistic_method_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_STATISTIC_METHOD",statistic_method_p->isChecked()));
  statistic_sources_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_STATISTIC_SOURCE",statistic_sources_p->isChecked()));
  statistic_executions_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_STATISTIC_EXECUTION",statistic_executions_p->isChecked()));
  code_fragments_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_CODE_FRAGMENTS",code_fragments_p->isChecked()));

  int coverage_level=Options::get_opt_int(QString(),"COVERAGE_LEVEL",1);
  global_coverage_level_p->setValue(coverage_level);
  global_coverage_output_all_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SUMMARY_OUTPUT_ALL_LEVEL",global_coverage_output_all_level_p->isChecked()));
  global_coverage_output_selected_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SUMMARY_OUTPUT_SELECTED_LEVEL",global_coverage_output_selected_level_p->isChecked()));
  global_coverage_output_selected_level_and_level_1_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SUMMARY_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",global_coverage_output_selected_level_and_level_1_p->isChecked()));

  methods_level_p->setValue(coverage_level);
  methods_output_all_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_OUTPUT_ALL_LEVEL",methods_output_all_level_p->isChecked()));
  methods_output_selected_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_OUTPUT_SELECTED_LEVEL",methods_output_selected_level_p->isChecked()));
  methods_output_selected_level_and_level_1_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",methods_output_selected_level_and_level_1_p->isChecked()));

  sources_level_p->setValue(coverage_level);
  sources_output_all_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_OUTPUT_ALL_LEVEL",sources_output_all_level_p->isChecked()));
  sources_output_selected_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_OUTPUT_SELECTED_LEVEL",sources_output_selected_level_p->isChecked()));
  sources_output_selected_level_and_level_1_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",sources_output_selected_level_and_level_1_p->isChecked()));

  executions_level_p->setValue(Options::get_opt_int(QString(),"HTML_REPORT_EXECUTIONS_COVERAGE_LEVEL",1));
  executions_output_all_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_ALL_LEVEL",executions_output_all_level_p->isChecked()));
  executions_output_selected_level_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_SELECTED_LEVEL",executions_output_selected_level_p->isChecked()));
  executions_output_selected_level_and_level_1_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",executions_output_selected_level_and_level_1_p->isChecked()));

  toc_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_LAYOUT_TOC",toc_p->isChecked()));
  code_fragments_manually_validated_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_CODE_FRAGMENTS_MANUALLY_VALIDATED",code_fragments_manually_validated_p->isChecked()));
  code_fragments_unexecuted_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_CODE_FRAGMENTS_UNEXECUTED",code_fragments_unexecuted_p->isChecked()));
  code_fragments_executed_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_CODE_FRAGMENTS_EXECUTED",code_fragments_executed_p->isChecked()));

  executions_sorting_coverage_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_SORTING_COVERAGE",executions_sorting_coverage_p->isChecked()));
  executions_sorting_name_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_SORTING_NAME",executions_sorting_name_p->isChecked()));
  executions_filter_active_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_EXECUTIONS_FILTER_ACTIVE",executions_filter_active_p->isChecked()));
  executions_filter_min_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_EXECUTIONS_FILTER_MIN",executions_filter_min_p->value()));
  executions_filter_max_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_EXECUTIONS_FILTER_MAX",executions_filter_max_p->value()));

  sources_sorting_coverage_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_SORTING_COVERAGE",sources_sorting_coverage_p->isChecked()));
  sources_sorting_name_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_SORTING_NAME",sources_sorting_name_p->isChecked()));
  sources_filter_active_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_SOURCES_FILTER_ACTIVE",sources_filter_active_p->isChecked()));
  sources_filter_min_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_SOURCES_FILTER_MIN",sources_filter_min_p->value()));
  sources_filter_max_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_SOURCES_FILTER_MAX",sources_filter_max_p->value()));


  methods_sorting_coverage_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_SORTING_COVERAGE",methods_sorting_coverage_p->isChecked()));
  methods_sorting_name_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_SORTING_NAME",methods_sorting_name_p->isChecked()));
  methods_filter_active_p->setChecked(Options::get_opt_bool(QString(),"HTML_REPORT_METHODS_FILTER_ACTIVE",methods_filter_active_p->isChecked()));
  methods_filter_min_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_METHODS_FILTER_MIN",methods_filter_min_p->value()));
  methods_filter_max_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_METHODS_FILTER_MAX",methods_filter_max_p->value()));

  sources_watermark_medium_level_p->setValue(Options::get_opt_double(QString(),"MODULE_MEDIUM_COLOR_LEVEL",sources_watermark_medium_level_p->value()));
  sources_watermark_low_level_p->setValue(Options::get_opt_double(QString(),"MODULE_LOW_COLOR_LEVEL",sources_watermark_low_level_p->value()));
  methods_watermark_medium_level_p->setValue(Options::get_opt_double(QString(),"FUNCTION_MEDIUM_COLOR_LEVEL",methods_watermark_medium_level_p->value()));
  methods_watermark_low_level_p->setValue(Options::get_opt_double(QString(),"FUNCTION_LOW_COLOR_LEVEL",methods_watermark_low_level_p->value()));
  executions_watermark_medium_level_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_EXECUTIONS_MEDIUM_LEVEL"));
  executions_watermark_low_level_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_EXECUTIONS_LOW_LEVEL"));
  global_coverage_watermark_medium_level_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_SUMMARY_MEDIUM_LEVEL"));
  global_coverage_watermark_low_level_p->setValue(Options::get_opt_double(QString(),"HTML_REPORT_SUMMARY_LOW_LEVEL"));

  global_coverage_max_intermediate_levels_p->setValue(Options::get_opt_long(QString(),"HTML_REPORT_SUMMARY_MAX_LEVELS"));
  methods_max_intermediate_levels_p->setValue(Options::get_opt_long(QString(),"HTML_REPORT_METHODS_MAX_LEVELS"));
  executions_max_intermediate_levels_p->setValue(Options::get_opt_long(QString(),"HTML_REPORT_EXECUTIONS_MAX_LEVELS"));
  sources_max_intermediate_levels_p->setValue(Options::get_opt_long(QString(),"HTML_REPORT_SOURCES_MAX_LEVELS"));

  checkInput();

  connect(code_fragments_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(statistic_executions_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(methods_filter_active_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(executions_filter_active_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(sources_filter_active_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(statistic_method_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(statistic_sources_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(statistic_global_coverage_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(global_coverage_level_p,SIGNAL(valueChanged(int)),this,SLOT(checkInput()));
  connect(methods_level_p,SIGNAL(valueChanged(int)),this,SLOT(checkInput()));
  connect(sources_level_p,SIGNAL(valueChanged(int)),this,SLOT(checkInput()));
  connect(executions_level_p,SIGNAL(valueChanged(int)),this,SLOT(checkInput()));
  connect(executions_filter_min_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(executions_filter_max_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(sources_filter_min_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(sources_filter_max_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(methods_filter_min_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(methods_filter_max_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(sources_watermark_medium_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(sources_watermark_low_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(methods_watermark_medium_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(methods_watermark_low_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(executions_watermark_medium_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(executions_watermark_low_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(global_coverage_watermark_medium_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(global_coverage_watermark_low_level_p,SIGNAL(valueChanged(double)),this,SLOT(checkInput()));
  connect(methods_output_all_level_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(global_coverage_output_all_level_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(sources_output_all_level_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
  connect(executions_output_all_level_p,SIGNAL(toggled(bool)),this,SLOT(checkInput()));
}

WHTMLReport::~WHTMLReport()
{
#if QT_COMPLETOR
  delete file_completor_p;
#endif
}


void WHTMLReport::saveSettings ()
{
  Options::set_opt(QString(),"HTML_REPORT_BARGRAPH",bargraph_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_FILE_NAME",file_p->text());
  Options::set_opt(QString(),"HTML_REPORT_FILE_TYPE",file_type_p->currentIndex());
  appendLRU("HTML_REPORT_FILE_NAME_HISTORY",file_p->text());
  Options::set_opt(QString(),"HTML_REPORT_TITLE",title_p->text());
  appendLRU("HTML_REPORT_TITLE_HISTORY",title_p->text());
  Options::set_opt(QString(),"HTML_REPORT_LAYOUT_ICON",icon_p->text());
  appendLRU("HTML_REPORT_LAYOUT_ICON_HISTORY",icon_p->text());
  Options::set_opt(QString(),"HTML_REPORT_LAYOUT_CSS",css_file_p->text());
  appendLRU("HTML_REPORT_LAYOUT_CSS_HISTORY",css_file_p->text());

  Options::set_opt(QString(),"HTML_REPORT_STATISTIC_SUMMARY",statistic_global_coverage_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_STATISTIC_METHOD",statistic_method_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_STATISTIC_SOURCE",statistic_sources_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_STATISTIC_EXECUTION",statistic_executions_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_CODE_FRAGMENTS",code_fragments_p->isChecked());

  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_OUTPUT_ALL_LEVEL",global_coverage_output_all_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_OUTPUT_SELECTED_LEVEL",global_coverage_output_selected_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",global_coverage_output_selected_level_and_level_1_p->isChecked());

  Options::set_opt(QString(),"HTML_REPORT_METHODS_OUTPUT_ALL_LEVEL",methods_output_all_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_OUTPUT_SELECTED_LEVEL",methods_output_selected_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",methods_output_selected_level_and_level_1_p->isChecked());

  Options::set_opt(QString(),"HTML_REPORT_SOURCES_OUTPUT_ALL_LEVEL",sources_output_all_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_OUTPUT_SELECTED_LEVEL",sources_output_selected_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",sources_output_selected_level_and_level_1_p->isChecked());

  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_COVERAGE_LEVEL",executions_level_p->value());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_ALL_LEVEL",executions_output_all_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_SELECTED_LEVEL",executions_output_selected_level_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_OUTPUT_SELECTED_LEVEL_AND_LEVEL_ONE",executions_output_selected_level_and_level_1_p->isChecked());

  Options::set_opt(QString(),"HTML_REPORT_LAYOUT_TOC",toc_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_CODE_FRAGMENTS_MANUALLY_VALIDATED",code_fragments_manually_validated_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_CODE_FRAGMENTS_UNEXECUTED",code_fragments_unexecuted_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_CODE_FRAGMENTS_UNEXECUTED",code_fragments_executed_p->isChecked());


  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_SORTING_COVERAGE",executions_sorting_coverage_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_SORTING_NAME",executions_sorting_name_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_FILTER_ACTIVE",executions_filter_active_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_FILTER_MIN",executions_filter_min_p->value());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_FILTER_MAX",executions_filter_max_p->value());

  Options::set_opt(QString(),"HTML_REPORT_SOURCES_SORTING_COVERAGE",sources_sorting_coverage_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_SORTING_NAME",sources_sorting_name_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_FILTER_ACTIVE",sources_filter_active_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_FILTER_MIN",sources_filter_min_p->value());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_FILTER_MAX",sources_filter_max_p->value());


  Options::set_opt(QString(),"HTML_REPORT_METHODS_SORTING_COVERAGE",methods_sorting_coverage_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_SORTING_NAME",methods_sorting_name_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_FILTER_ACTIVE",methods_filter_active_p->isChecked());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_FILTER_MIN",methods_filter_min_p->value());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_FILTER_MAX",methods_filter_max_p->value());

  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_MEDIUM_LEVEL",executions_watermark_medium_level_p->value());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_LOW_LEVEL",executions_watermark_low_level_p->value());
  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_MEDIUM_LEVEL",global_coverage_watermark_medium_level_p->value());
  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_LOW_LEVEL",global_coverage_watermark_low_level_p->value());

  Options::set_opt(QString(),"HTML_REPORT_SUMMARY_MAX_LEVELS",global_coverage_max_intermediate_levels_p->value());
  Options::set_opt(QString(),"HTML_REPORT_METHODS_MAX_LEVELS",methods_max_intermediate_levels_p->value());
  Options::set_opt(QString(),"HTML_REPORT_EXECUTIONS_MAX_LEVELS",executions_max_intermediate_levels_p->value());
  Options::set_opt(QString(),"HTML_REPORT_SOURCES_MAX_LEVELS",sources_max_intermediate_levels_p->value());
  Options::set_opt(QString(),"HTML_REPORT_OUTPUT_TYPE",documentType());
}

void WHTMLReport::process ()
{
  saveSettings();
  Instrumentation::coverage_method_t coverage_method=CoverageSettings::object().getCoverageMethod();
  int global_coverage_level = global_coverage_level_p->value();

  CSMes::DisplayLevelType_t global_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  if (!statistic_global_coverage_p->isChecked())
    global_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  else if (global_coverage_output_all_level_p->isChecked())
    global_coverage_display_mode=CSMes::ALL_LEVEL;
  else if (global_coverage_output_selected_level_p->isChecked())
    global_coverage_display_mode=CSMes::SELECTED_LEVEL;
  else if (global_coverage_output_selected_level_and_level_1_p->isChecked())
    global_coverage_display_mode=CSMes::SELECTED_LEVEL_AND_LEVEL_1;

  CSMes::DisplayLevelType_t method_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  if (!statistic_method_p->isChecked())
    method_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  else if (methods_output_all_level_p->isChecked())
    method_coverage_display_mode=CSMes::ALL_LEVEL;
  else if (methods_output_selected_level_p->isChecked())
    method_coverage_display_mode=CSMes::SELECTED_LEVEL;
  else if (methods_output_selected_level_and_level_1_p->isChecked())
    method_coverage_display_mode=CSMes::SELECTED_LEVEL_AND_LEVEL_1;
  CSMes::DisplaySortingType_t method_coverage_sorting_mode=CSMes::SORT_NONE;
  if (methods_sorting_coverage_p->isChecked())
    method_coverage_sorting_mode=CSMes::SORT_COVERAGE;
  else if (methods_sorting_name_p->isChecked())
    method_coverage_sorting_mode=CSMes::SORT_NAME;

  CSMes::DisplayLevelType_t execution_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  if (!statistic_executions_p->isChecked())
    execution_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  else if (executions_output_all_level_p->isChecked())
    execution_coverage_display_mode=CSMes::ALL_LEVEL;
  else if (executions_output_selected_level_p->isChecked())
    execution_coverage_display_mode=CSMes::SELECTED_LEVEL;
  else if (executions_output_selected_level_and_level_1_p->isChecked())
    execution_coverage_display_mode=CSMes::SELECTED_LEVEL_AND_LEVEL_1;
  CSMes::DisplaySortingType_t execution_coverage_sorting_mode=CSMes::SORT_NONE;
  if (executions_sorting_coverage_p->isChecked())
    execution_coverage_sorting_mode=CSMes::SORT_COVERAGE;
  else if (executions_sorting_name_p->isChecked())
    execution_coverage_sorting_mode=CSMes::SORT_NAME;

  CSMes::DisplayLevelType_t source_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  if (!statistic_sources_p->isChecked())
    source_coverage_display_mode=CSMes::DISPLAY_NOTHING;
  else if (sources_output_all_level_p->isChecked())
    source_coverage_display_mode=CSMes::ALL_LEVEL;
  else if (sources_output_selected_level_p->isChecked())
    source_coverage_display_mode=CSMes::SELECTED_LEVEL;
  else if (sources_output_selected_level_and_level_1_p->isChecked())
    source_coverage_display_mode=CSMes::SELECTED_LEVEL_AND_LEVEL_1;
  CSMes::DisplaySortingType_t source_coverage_sorting_mode=CSMes::SORT_NONE;
  if (sources_sorting_coverage_p->isChecked())
    source_coverage_sorting_mode=CSMes::SORT_COVERAGE;
  else if (sources_sorting_name_p->isChecked())
    source_coverage_sorting_mode=CSMes::SORT_NAME;

  if (!csmes_p->exportHtmlReport(
        documentType(),
        file(),
        title_p->text(),
        icon_p->text(),
        css_file_p->text(),

        global_coverage_display_mode,
        method_coverage_display_mode,
        method_coverage_sorting_mode,
        source_coverage_display_mode,
        source_coverage_sorting_mode,
        execution_coverage_display_mode,
        execution_coverage_sorting_mode,

        executions_level_p->value(),

        toc_p->isChecked(),
        code_fragments_manually_validated_p->isChecked() && code_fragments_p->isChecked() ,
        code_fragments_unexecuted_p->isChecked() && code_fragments_p->isChecked(),
        code_fragments_executed_p->isChecked() && code_fragments_p->isChecked(),

        executions_filter_active_p->isChecked(),
        executions_filter_min_p->value(),
        executions_filter_max_p->value(),

        sources_filter_active_p->isChecked(),
        sources_filter_min_p->value(),
        sources_filter_max_p->value(),


        methods_filter_active_p->isChecked(),
        methods_filter_min_p->value(),
        methods_filter_max_p->value(),

        executions_watermark_medium_level_p->value(),
        executions_watermark_low_level_p->value(),
        global_coverage_watermark_medium_level_p->value(),
        global_coverage_watermark_low_level_p->value(),

        global_coverage_max_intermediate_levels_p->value(),
        methods_max_intermediate_levels_p->value(),
        executions_max_intermediate_levels_p->value(),
        sources_max_intermediate_levels_p->value(),
        global_coverage_level,
        coverage_method,

        methods_level_p->value(),
        methods_watermark_low_level_p->value(),
        methods_watermark_medium_level_p->value(),
        sources_watermark_low_level_p->value(),
        sources_watermark_medium_level_p->value(),
        CoverageSettings::object().getTestCoverageMode(),
        bargraph_p->isChecked()
          )
          )
          {
            QMessageBox::warning(this,tr("Error"),
                tr("Could not write file '%1'").arg(file()),
                QMessageBox::Close,QMessageBox::NoButton);
            accept();
            return;
          }

  accept();
}


void WHTMLReport::checkInput ()
{
  switch (documentType())
  {
    case CSMes::DOCUMENT_HTML:
      sources_watermarks_p->setEnabled(true);
      css_file_p->setEnabled(true);
      icon_p->setEnabled(true);
      executions_watermarks_p->setEnabled(true);
      global_coverage_watermarks_p->setEnabled(true);
      methods_watermarks_p->setEnabled(true);
      bargraph_p->setEnabled(true);
      toc_p->setEnabled(true);
      tabs_p->setTabEnabled(tabs_p->indexOf(tab_layout),true);
      break;
    case CSMes::DOCUMENT_XML:
      sources_watermarks_p->setEnabled(false);
      css_file_p->setEnabled(false);
      icon_p->setEnabled(false);
      executions_watermarks_p->setEnabled(false);
      global_coverage_watermarks_p->setEnabled(false);
      methods_watermarks_p->setEnabled(false);
      bargraph_p->setEnabled(false);
      toc_p->setEnabled(false);
      tabs_p->setTabEnabled(tabs_p->indexOf(tab_layout),false);
      break;
    case CSMes::DOCUMENT_OPEN_OFFICE:
      sources_watermarks_p->setEnabled(false);
      css_file_p->setEnabled(false);
      icon_p->setEnabled(false);
      executions_watermarks_p->setEnabled(false);
      global_coverage_watermarks_p->setEnabled(false);
      methods_watermarks_p->setEnabled(false);
      bargraph_p->setEnabled(false);
      toc_p->setEnabled(true);
      tabs_p->setTabEnabled(tabs_p->indexOf(tab_layout),true);
      break;
  }

  tabs_p->setTabEnabled(tabs_p->indexOf(tab_executions),statistic_executions_p->isChecked());
  tabs_p->setTabEnabled(tabs_p->indexOf(tab_global_coverage),statistic_global_coverage_p->isChecked());
  tabs_p->setTabEnabled(tabs_p->indexOf(tab_methods),statistic_method_p->isChecked());
  tabs_p->setTabEnabled(tabs_p->indexOf(tab_code_fragments),code_fragments_p->isChecked());
  tabs_p->setTabEnabled(tabs_p->indexOf(tab_sources),statistic_sources_p->isChecked());
  global_coverage_level_output_p->setEnabled(global_coverage_level_p->value()>1);
  methods_level_output_p->setEnabled(methods_level_p->value()>1);
  sources_level_output_p->setEnabled(sources_level_p->value()>1);
  executions_level_output_p->setEnabled(executions_level_p->value()>1);

  methods_filter_min_p->setMaximum(methods_filter_max_p->value());
  methods_filter_max_p->setMinimum(methods_filter_min_p->value());
  executions_filter_min_p->setMaximum(executions_filter_max_p->value());
  executions_filter_max_p->setMinimum(executions_filter_min_p->value());
  sources_filter_min_p->setMaximum(sources_filter_max_p->value());
  sources_filter_max_p->setMinimum(sources_filter_min_p->value());

  methods_filter_max_p->setEnabled(methods_filter_active_p->isChecked());
  methods_filter_min_p->setEnabled(methods_filter_active_p->isChecked());
  sources_filter_max_p->setEnabled(sources_filter_active_p->isChecked());
  sources_filter_min_p->setEnabled(sources_filter_active_p->isChecked());
  executions_filter_max_p->setEnabled(executions_filter_active_p->isChecked());
  executions_filter_min_p->setEnabled(executions_filter_active_p->isChecked());

  global_coverage_watermark_low_level_p->setMaximum(global_coverage_watermark_medium_level_p->value());
  global_coverage_watermark_medium_level_p->setMinimum(global_coverage_watermark_low_level_p->value());

  executions_watermark_low_level_p->setMaximum(executions_watermark_medium_level_p->value());
  executions_watermark_medium_level_p->setMinimum(executions_watermark_low_level_p->value());

  methods_watermark_low_level_p->setMaximum(methods_watermark_medium_level_p->value());
  methods_watermark_medium_level_p->setMinimum(methods_watermark_low_level_p->value());

  sources_watermark_low_level_p->setMaximum(sources_watermark_medium_level_p->value());
  sources_watermark_medium_level_p->setMinimum(sources_watermark_low_level_p->value());

  methods_max_intermediate_levels_p->setEnabled(methods_output_all_level_p->isChecked());
  global_coverage_max_intermediate_levels_p->setEnabled(global_coverage_output_all_level_p->isChecked());
  sources_max_intermediate_levels_p->setEnabled(sources_output_all_level_p->isChecked());
  executions_max_intermediate_levels_p->setEnabled(executions_output_all_level_p->isChecked());

  bool ok=!file_p->text().isEmpty();

  QPushButton *ok_p= buttons_p->button(QDialogButtonBox::Ok);
  ok_p->setEnabled(ok);
  QPushButton *apply_p=buttons_p->button(QDialogButtonBox::Apply);
  apply_p->setEnabled(ok);
}

void WHTMLReport::on_file_p_textChanged( const QString & )
{
  checkInput();
}

#if QT_COMPLETOR
void WHTMLReport::appendLRU(const QString &key,const QString &value)
{
  QStringList old_lru=Options::get_opt_strlst(QString(),key);
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  Options::set_opt(QString(),key,new_lru);
  file_completor_model_p->setStringList(new_lru);
}
#endif

void WHTMLReport::on_file_browse_p_clicked()
{
   QString dir=Options::get_opt_str(QString(),"HTML_OUTPUT_DIR",QString());
   QString selectedFilter=Options::get_opt_str(QString(),"HTML_OUTPUT_SELECTED_FILTER",QString());
   QString filter;
   QString title;
   switch (documentType())
   {
   case CSMes::DOCUMENT_HTML:
      filter=tr("HTML Files (*.html *.htm)");
      title=tr("HTML File Name");
      break;
   case CSMes::DOCUMENT_OPEN_OFFICE:
      filter=tr("OpenOffice Files (*.odt)");
      title=tr("OpenOffice File Name");
      break;
   case CSMes::DOCUMENT_XML:
      filter=tr("XML Files (*.xml)");
      title=tr("XML File Name");
      break;
   }
   filter+=";;";
   filter+=tr("All (*)");

   QString filename=QFileDialog::getSaveFileName ( this, title , dir , filter , &selectedFilter , FILE_DIALOG_OPTIONS) ;

   if (filename!=QString())
   {
      Options::set_opt(QString(),"HTML_OUTPUT_DIR",dir);
      Options::set_opt(QString(),"HTML_OUTPUT_SELECTED_FILTER",selectedFilter);
      file_p->setText(filename);
   }
}


void WHTMLReport::on_file_type_p_activated(int)
{
   file_p->setText(QString());
   checkInput();
}

void WHTMLReport::apply()
{
  saveSettings();
}

QString WHTMLReport::file() const
{
   return file_p->text();
}

CSMes::DocumentType_t WHTMLReport::documentType() const
{
  int index=0;
  for (QMap<CSMes::DocumentType_t,QString>::const_iterator it=document_types.begin();it!=document_types.end();++it)
  {
    if (index== file_type_p->currentIndex())
      return it.key();
    index++;
  }
  return CSMes::DOCUMENT_HTML;
}
