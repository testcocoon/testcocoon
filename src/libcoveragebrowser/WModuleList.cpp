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

#include "WModuleList.h"
#include "CoverageSettings.h"
#include <QHeaderView>
#include "ModuleListModel.h"
#include "SortFilterProxy.h"
#include "coveragebrowserpdef.h"
#include "options.h"
#include "Service.h"
#include <QVector>
static QStringList old_comments;

WModuleList::WModuleList(QWidget* parent,  Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
  csmes_p=NULL;
  setupUi(this);
  modulesSortModel_p=NULL;
  modulesModel_p = NULL;
  connect(different_indentation_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_indentation_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_DIFFERENT_INDENTATION",true));
  connect(different_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_DIFFERENT",true));
  connect(different_comment_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_comment_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_DIFFERENT_COMMENT",true));
  connect(not_existing_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  not_existing_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_NOT_EXISTING",true));
  connect(new_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  new_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_NEW",true));
  connect(same_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  same_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_SAME",true));
  connect(different_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_MODIFICATIONS",true));
  connect(other_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  other_p->setChecked(Options::get_opt_bool(QString(),"MODULE_LIST_OTHER",true));
#if QT_COMPLETOR
  connect(&LRU_timer,SIGNAL(timeout()),this,SLOT(LRUTimerEvent()));
  QStringList lru=Options::get_opt_strlst(QString(),"/SELECTION_MODULE_LIST");
  filter_completor_p = new QCompleter(filter_p);
  filter_completor_model_p=new QStringListModel(lru,filter_completor_p);
  filter_completor_p->setModel(filter_completor_model_p);
  filter_completor_p->setModelSorting(QCompleter::UnsortedModel);
  filter_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  filter_p->setCompleter( filter_completor_p );
#endif
  filter_p->setText(Options::get_opt_str(QString(),"MODULE_LIST_FILTER",QString()));
  connect(&CoverageSettings::object(),SIGNAL(modified()),
          this,SLOT(refreshView()));
}

WModuleList::~WModuleList()
{
   Options::set_opt(QString(),"MODULE_LIST_OTHER",other_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_DIFFERENT_INDENTATION",different_indentation_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_DIFFERENT",different_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_DIFFERENT_COMMENT",different_comment_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_NOT_EXISTING",not_existing_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_NEW",new_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_SAME",same_p->isChecked());
   Options::set_opt(QString(),"MODULE_LIST_MODIFICATIONS",different_p->isChecked());
   if (modulesSortModel_p)
    delete modulesSortModel_p;
  if (modulesModel_p)
    delete modulesModel_p;
#if QT_COMPLETOR
  delete filter_completor_p;
#endif
}


void WModuleList::setStatusColorLevel(double low, double medium)
{
  low_level=low;
  medium_level=medium;
}

void WModuleList::setCSMes(const CSMesUndoRedoFramework *csmes)
{
   if (csmes_p)
      modulesView_p->saveState();
  csmes_p=csmes;
  if (modulesSortModel_p)
    delete modulesSortModel_p;
  if (modulesModel_p)
    delete modulesModel_p;

  modulesModel_p = new ModuleListModel();
  modulesModel_p->setCSMes(csmes);
  modulesModel_p->setStatusColorLevel(low_level,medium_level);
  modulesSortModel_p=new SortFilterProxy;
  modulesSortModel_p->setSourceModel(modulesModel_p);
  modulesView_p->setModel(modulesSortModel_p);
  modulesView_p->setObjectName("modules view");
  modulesView_p->setWindowTitle(tr("Source Browser"));

  on_filter_p_textChanged (filter_p->text());
#if defined( OS_LINUX) || defined(OS_MACX)
  disconnect (modulesView_p,SIGNAL(clicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
  connect (modulesView_p,SIGNAL(clicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
#else
  disconnect (modulesView_p,SIGNAL(doubleClicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
  connect (modulesView_p,SIGNAL(doubleClicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
#endif
  modulesView_p->restoreState();
  modulesView_p->setItemsExpandable(true);
  modulesView_p->setSortingEnabled(true);
  modulesView_p->setUniformRowHeights(true);
  applyDifferenceFilter();
}

void WModuleList::on_filter_type_p_clicked(bool)
{
#if QT_COMPLETOR
   if (filter.isValid())
      appendLRU(filter_p->text());
#endif
   if (filter.isValid())
   {
      if (filter.patternSyntax()==MatchExpr::Wildcard)
         filter_p->setText(filter.convertToRegExp(filter_p->text()));
      else if (filter.patternSyntax()==MatchExpr::RegExp)
         filter_p->setText(filter.convertToWilcard(filter_p->text()));
   }
}

void WModuleList::on_filter_p_textChanged (const QString &pattern)
{
  if (!csmes_p) return;
#if QT_COMPLETOR
  LRU_timer.stop();
  LRU_timer.setSingleShot(true);
  LRU_timer.start(LRU_TIMER_UPDATE);
#endif

  Options::set_opt(QString(),"MODULE_LIST_FILTER",pattern);
  filter.setPattern(pattern);
#ifdef OS_WIN32
  filter.setCaseSensitivity(Qt::CaseInsensitive);
#endif
  bool filter_valid=filter.isValid();
  if (!filter_valid)
  {
    filter_type_p->setIcon(QPixmap(":/images/filterinvalid.svg"));
    filter_type_p->setStatusTip(tr("Error:")+filter.errorString());
  }
  else if (filter.pattern().length()==0)
  {
    filter_type_p->setIcon(QPixmap(":/images/nofilter.svg"));
    filter_type_p->setStatusTip(tr("No filter used."));
  }
  else if (filter.patternSyntax()==MatchExpr::Wildcard)
  {
    filter_type_p->setIcon(QPixmap(":/images/filterwildcard.svg"));
    filter_type_p->setStatusTip(tr("Filter type: Wildcard Expressions"));
  }
  else
  {
    filter_type_p->setIcon(QPixmap(":/images/filterregexp.svg"));
    filter_type_p->setStatusTip(tr("Filter type: Regular Expressions"));
  }
  filter_p->setStatusTip(filter_type_p->statusTip());
  modulesSortModel_p->setFilterRegExp(filter);
}

void WModuleList::selectSource (const ModuleFile &module,const SourceFile &source)
{
  if (modulesSortModel_p==NULL)
    return ;

  QModelIndex curr_index= modulesSortModel_p->mapToSource (modulesView_p->currentIndex());
  QString curr_module=modulesModel_p->module(curr_index);
  QString curr_source=modulesModel_p->source(curr_index);
  if (module!=curr_module || source!=curr_source)
  {
    QModelIndex found=modulesModel_p->find(module,source);
    if (found.isValid())
    {
      QModelIndex display_index= modulesSortModel_p->mapFromSource (found);
      modulesView_p->setCurrentIndex(display_index);
    }
  }
}

QString WModuleList::Module() const
{
  if (modulesSortModel_p==NULL)
    return QString::null;

  QModelIndex curr_index= modulesSortModel_p->mapToSource (modulesView_p->currentIndex());
  return modulesModel_p->module(curr_index);
}

QString WModuleList::Source() const
{
  if (modulesSortModel_p==NULL)
    return QString::null;

  QModelIndex curr_index= modulesSortModel_p->mapToSource (modulesView_p->currentIndex());
  return modulesModel_p->source(curr_index);
}

void WModuleList::itemSelected ( const QModelIndex  & index_proxy)
{
  QModelIndex index= modulesSortModel_p->mapToSource (index_proxy);
  emit selectedSource(modulesModel_p->module(index),
      modulesModel_p->source(index),
      CoverageSettings::object().getSourceType(),
      -1,-1,-1,-1);
}


#if QT_COMPLETOR
void WModuleList::LRUTimerEvent()
{
   if (filter.isValid())
      appendLRU(filter_p->text());
}

void WModuleList::appendLRU(const QString &value)
{
  QStringList old_lru=Options::get_opt_strlst(QString(),"/SELECTION_MODULE_LIST");
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  Options::set_opt(QString(),"/SELECTION_MODULE_LIST",new_lru);
  filter_completor_model_p->setStringList(new_lru);
}
#endif

void WModuleList::refreshView()
{
  if (csmes_p)
  {
    modulesModel_p->refreshView();
    modulesView_p->update();
  }
  applyDifferenceFilter();
  if (csmes_p)
    modulesView_p->update();
  update();
#ifdef OS_MACX
  int index=pages_p->currentIndex();
  pages_p->setCurrentIndex(1);
  pages_p->setCurrentIndex(index);
#endif
}

void WModuleList::on_more_p_toggled(bool b)
{
  if (b)
    pages_p->setCurrentIndex(1);
  else
    pages_p->setCurrentIndex(0);
}

void WModuleList::applyDifferenceFilter()
{
  QFont f=more_p->font();
  f.setBold(false);
  if ( ! other_p->isChecked())
    f.setBold(true);
  else if ( ! different_indentation_p->isChecked())
    f.setBold(true);
  else if ( ! different_p->isChecked())
    f.setBold(true);
  else if ( ! different_comment_p->isChecked())
    f.setBold(true);
  else if ( ! not_existing_p->isChecked())
    f.setBold(true);
  else if ( ! new_p->isChecked())
    f.setBold(true);
  else if ( ! same_p->isChecked())
    f.setBold(true);
  else if ( ! different_p->isChecked())
    f.setBold(true);
  more_p->setFont(f);

  bool has_reference=false;
  if (csmes_p)
    has_reference=csmes_p->hasReference();
  more_p->setShown(has_reference);

   if (modulesModel_p)
   {
     if (has_reference)
       modulesModel_p->setFilters(
         different_indentation_p->isChecked(),
         different_p->isChecked(),
         different_comment_p->isChecked(),
         not_existing_p->isChecked(),
         new_p->isChecked(),
         same_p->isChecked(),
         other_p->isChecked());
      else
        modulesModel_p->setFilters(true,true,true,true,true,true,true);

     modulesSortModel_p->invalidate();
   }
}
