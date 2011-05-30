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

#include "WOption.h"
#include "WMain.h"
#include "settingscheck.h"
#include <QLocale>
#ifdef QT_TOOLBAR_DIALOG
#include <qttoolbardialog.h> 
#endif
#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>
#include "options.h"
#include "Service.h"
#include <qfontdialog.h> 
#include <QStyleFactory>
#include <QDir>
#include <QApplication>
#include <QStringList>


/*
 *  Constructs a WOption as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
WOption::WOption(WMain *w,QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent,  fl)
{
    setupUi(this);
    wmain_p=w;

#ifndef QT_TOOLBAR_DIALOG
    toolbar_p->hide();
#endif

    // signals and slots connections

    connect(no_comment_for_validation_p, SIGNAL(toggled(bool)), this, SLOT(on_no_comment_for_validation_p_toggled(bool)));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
WOption::~WOption()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void WOption::languageChange()
{
    retranslateUi(this);
}


void WOption::init()
{
  QDir application(QCoreApplication::applicationDirPath ());
  QStringList translations=application.entryList(QStringList("*.qm"));
  if (translations.isEmpty())
  {
    language_p->hide();
    language_label_p->hide();
  }
  else
  {
    QRegExp normalize("([^\\\\/.]*).qm");
    translations+=QLocale::languageToString(QLocale::English);
    translations.replaceInStrings(normalize,"\\1");
    language_p->addItems(translations);
    QString default_language=Options::get_opt_str(QString(),"LANGUAGE",DEF_LANGUAGE);
    if (default_language.isEmpty())
      default_language=QLocale::languageToString(QLocale::English);
    language_p->setCurrentIndex(language_p->findText(default_language));
  }

  window_style_p->addItems(QStyleFactory::keys());
  window_style_p->setCurrentIndex(window_style_p->findText(Options::get_opt_str(QString(),"STYLE",DEF_WINDOW_STYLE)));
  save_window_p->setChecked(Options::get_opt_bool(QString(),"RESTORE_WINDOW_POSITION",save_window_p->isChecked()));
  save_on_exit_p->setChecked(Options::get_opt_bool(QString(),"SAVE_ON_EXIT",save_on_exit_p->isChecked()));
  reload_p->setChecked(Options::get_opt_bool(QString(),"RELOAD_PROJECT",reload_p->isChecked()));
  minimum_comment_size_p->setValue(Options::get_opt_long(QString(),"MINIMUM_COMMENT_SIZE",minimum_comment_size_p->value()));
  executed_by_limit_p->setValue(Options::get_opt_long(QString(),"EXECUTED_BY_LIMIT",executed_by_limit_p->value()));
  class_medium_level_p->setValue(Options::get_opt_double(QString(),"CLASS_MEDIUM_COLOR_LEVEL",class_medium_level_p->value()));
  class_low_level_p->setValue(Options::get_opt_double(QString(),"CLASS_LOW_COLOR_LEVEL",class_low_level_p->value()));
  function_medium_level_p->setValue(Options::get_opt_double(QString(),"FUNCTION_MEDIUM_COLOR_LEVEL",function_medium_level_p->value()));
  function_low_level_p->setValue(Options::get_opt_double(QString(),"FUNCTION_LOW_COLOR_LEVEL",function_low_level_p->value()));
  module_medium_level_p->setValue(Options::get_opt_double(QString(),"MODULE_MEDIUM_COLOR_LEVEL",module_medium_level_p->value()));
  module_low_level_p->setValue(Options::get_opt_double(QString(),"MODULE_LOW_COLOR_LEVEL",module_low_level_p->value()));
  no_comment_for_validation_p->setChecked(Options::get_opt_bool(QString(),"NO_COMMENT_FOR_MANUAL_VALIDATION"));
  source_code_font_p->setText(Options::get_opt_str(QString(),"FONT_SOURCE_CODE",DEF_FONT_SOURCE_CODE));
  comment_font_p->setText(Options::get_opt_str(QString(),"FONT_COMMENT",DEF_FONT_COMMENT));
  main_font_p->setText(Options::get_opt_str(QString(),"FONT_MAIN",DEF_FONT_MAIN));
  tooltip_source_view_p->setChecked(Options::get_opt_bool(QString(),"TOOLTIP_SOURCE_VIEW",tooltip_source_view_p->isChecked()));
  tooltip_source_list_p->setChecked(Options::get_opt_bool(QString(),"TOOLTIP_SOURCE_LIST",tooltip_source_list_p->isChecked()));
  tooltip_execution_list_p->setChecked(Options::get_opt_bool(QString(),"TOOLTIP_EXECUTION_LIST",tooltip_execution_list_p->isChecked()));
  tooltip_method_list_p->setChecked(Options::get_opt_bool(QString(),"TOOLTIP_METHOD_LIST",tooltip_method_list_p->isChecked()));
  tab_size_p->setValue(tabSize());
  statistic_background_p->setChecked(Options::get_opt_bool(QString(),"STATISTICS_CALULATION_BACKGROUND",statistic_background_p->isChecked()));
  checkCoherence();    
}

void WOption::destroy()
{}


void WOption::on_buttonCancel_clicked()
{
  on_window_style_p_activated(Options::get_opt_str(QString(),"STYLE",DEF_WINDOW_STYLE));
  reject();
}

void WOption::on_buttonOk_clicked()
{
  Options::set_opt(QString(),"MINIMUM_COMMENT_SIZE",(long)minimum_comment_size_p->value());
  Options::set_opt(QString(),"EXECUTED_BY_LIMIT",(long)executed_by_limit_p->value());
  Options::set_opt(QString(),"RELOAD_PROJECT",reload_p->isChecked());
  Options::set_opt(QString(),"RESTORE_WINDOW_POSITION",save_window_p->isChecked());
  Options::set_opt(QString(),"SAVE_ON_EXIT",save_on_exit_p->isChecked());
  Options::set_opt(QString(),"FUNCTION_MEDIUM_COLOR_LEVEL",function_medium_level());
  Options::set_opt(QString(),"FUNCTION_LOW_COLOR_LEVEL",function_low_level());
  Options::set_opt(QString(),"CLASS_MEDIUM_COLOR_LEVEL",class_medium_level());
  Options::set_opt(QString(),"CLASS_LOW_COLOR_LEVEL",class_low_level());
  Options::set_opt(QString(),"MODULE_MEDIUM_COLOR_LEVEL",module_medium_level());
  Options::set_opt(QString(),"MODULE_LOW_COLOR_LEVEL",module_low_level());
  Options::set_opt(QString(),"NO_COMMENT_FOR_MANUAL_VALIDATION",no_comment_for_validation_p->isChecked());
  Options::set_opt(QString(),"FONT_SOURCE_CODE",source_code_font_p->text());
  Options::set_opt(QString(),"FONT_COMMENT",comment_font_p->text());
  Options::set_opt(QString(),"FONT_MAIN",main_font_p->text());
  Options::set_opt(QString(),"TOOLTIP_SOURCE_VIEW",tooltip_source_view_p->isChecked());
  Options::set_opt(QString(),"TOOLTIP_SOURCE_LIST",tooltip_source_list_p->isChecked());
  Options::set_opt(QString(),"TOOLTIP_EXECUTION_LIST",tooltip_execution_list_p->isChecked());
  Options::set_opt(QString(),"TOOLTIP_METHOD_LIST",tooltip_method_list_p->isChecked());
  Options::set_opt(QString(),"STATISTICS_CALULATION_BACKGROUND",statistic_background_p->isChecked());
  setTabSize(tab_size_p->value());
  Options::set_opt(QString(),QString("TAB_SIZE"),(long)tab_size_p->value());
  QDialog::accept();
}

double WOption::class_low_level( ) const
{
  return class_low_level_p->value();
}


double WOption::class_medium_level( ) const
{
  return class_medium_level_p->value();
}


double WOption::function_low_level( ) const
{
  return function_low_level_p->value();
}


double WOption::function_medium_level( ) const
{
  return function_medium_level_p->value();
}


double WOption::module_low_level( ) const
{
  return module_low_level_p->value();
}


double WOption::module_medium_level( ) const
{
  return module_medium_level_p->value();
}

QString WOption::window_style( ) const
{
  Options::set_opt(QString(),"STYLE",window_style_p->currentText());
  return window_style_p->currentText();
}

void WOption::checkCoherence()
{
  bool coherent=true;

  if (module_medium_level()<-0.0001) coherent=false;
  if (module_low_level()<-0.00001) coherent=false;
  if (module_low_level()>module_medium_level()) coherent=false;
  if (module_medium_level()>100) coherent=false;
  module_low_level_p->setMaximum(module_medium_level_p->value());
  module_medium_level_p->setMinimum(module_low_level_p->value());

  if (class_medium_level()<-0.0001) coherent=false;
  if (class_low_level()<-0.00001) coherent=false;
  if (class_low_level()>class_medium_level()) coherent=false;
  if (class_medium_level()>100) coherent=false;
  class_low_level_p->setMaximum(class_medium_level_p->value());
  class_medium_level_p->setMinimum(class_low_level_p->value());

  if (function_medium_level()<-0.0001) coherent=false;
  if (function_low_level()<-0.00001) coherent=false;
  if (function_low_level()>function_medium_level()) coherent=false;
  if (function_medium_level()>100) coherent=false;
  function_low_level_p->setMaximum(function_medium_level_p->value());
  function_medium_level_p->setMinimum(function_low_level_p->value());

  buttonOk->setEnabled(coherent);
}


void WOption::on_function_medium_level_p_valueChanged(double)
{
  checkCoherence();
}


void WOption::on_function_low_level_p_valueChanged(double)
{
  checkCoherence();
}

void WOption::on_class_medium_level_p_valueChanged(double)
{
  checkCoherence();
}


void WOption::on_class_low_level_p_valueChanged(double)
{
  checkCoherence();
}

void WOption::on_module_medium_level_p_valueChanged(double)
{
  checkCoherence();
}


void WOption::on_module_low_level_p_valueChanged(double)
{
  checkCoherence();
}


void WOption::on_no_comment_for_validation_p_toggled( bool )
{

}


void WOption::on_main_font_p_clicked()
{
  bool ok;
  QFont f;
  f.fromString(main_font_p->text());
  f=QFontDialog::getFont(&ok,f,this);
  if (ok)
  {
    main_font_p->setText(f.toString());
  }
}


void WOption::on_comment_font_p_clicked()
{
  bool ok;
  QFont f;
  f.fromString(comment_font_p->text());
  f=QFontDialog::getFont(&ok,f,this);
  if (ok)
  {
    comment_font_p->setText(f.toString());
  }
}


void WOption::on_source_code_font_p_clicked()
{
  bool ok;
  QFont f;
  f.fromString(source_code_font_p->text());
  f=QFontDialog::getFont(&ok,f,this);
  if (ok)
  {
    source_code_font_p->setText(f.toString());
  }
}

void WOption::on_toolbar_p_clicked()
{
#ifdef QT_TOOLBAR_DIALOG
  QtToolBarDialog dialog(this);
  dialog.setToolBarManager(wmain_p->toolBarManager());
  dialog.exec();
#endif
}

void WOption::on_tooltip_source_view_p_toggled( bool )
{

}

void WOption::on_tooltip_source_list_p_toggled( bool )
{
}

void WOption::on_tooltip_execution_list_p_toggled( bool )
{
}
void WOption::on_tooltip_method_list_p_toggled( bool )
{
}

void WOption::on_window_style_p_activated(const QString &styleName)
{
  QStyle *style_p=QStyleFactory::create(styleName);
  if (style_p)
  {
    QApplication::setStyle(style_p);
    if (styleName!="WindowsXP")
       QApplication::setPalette(QApplication::style()->standardPalette());
  }
}

QString WOption::language() const
{
  if (language_p->isHidden())
    return QString();

  if (language_p->currentText()!=QLocale::languageToString(QLocale::English))
  {
    return language_p->currentText();
  }
  else
    return QString();
}

static QString formatCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) 
{
   QString text;
   text+=QObject::tr("Cache Size")+": "+QString::number(max_cost)+"\n";
   text+=QObject::tr("Actual Size")+": "+QString::number(cost)+"\n";
   text+=QObject::tr("Cache Hit")+": "+QString::number(cache_hit)+"\n";
   text+=QObject::tr("Cache Miss")+": "+QString::number(cache_miss);
   return text;
}

int WOption::getExecutionCacheMaxCost() const
{
   return execution_cache_p->value();
}

int WOption::getStatisticCacheMaxCost() const
{
   return statistic_cache_p->value();
}

int WOption::getSourceCacheMaxCost() const
{
   return source_cache_p->value();
}

void WOption::setSourceCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) 
{
   source_cache_p->setValue(max_cost);
   source_cache_info_p->setText(formatCacheStatistics(cache_hit,cache_miss,max_cost,cost) );
}

void WOption::setStatisticCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) 
{
   statistic_cache_p->setValue(max_cost);
   statistic_cache_info_p->setText(formatCacheStatistics(cache_hit,cache_miss,max_cost,cost) );
}

void WOption::setExecutionCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) 
{
   execution_cache_p->setValue(max_cost);
   execution_cache_info_p->setText(formatCacheStatistics(cache_hit,cache_miss,max_cost,cost) );
}

void WOption::on_reset_warning_p_clicked() 
{
   SettingsCheck::object().clear();
}
