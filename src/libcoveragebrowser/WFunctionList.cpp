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

#include "WFunctionList.h"
#include "CoverageSettings.h"
#include <QHeaderView>
#include "FunctionListModel.h"
#include "coveragebrowserpdef.h"
#include "SortFilterProxy.h"
#include "csmesundoredoframework.h"
#include "Service.h"
#include "options.h"
#include <QFont>
#include <QPushButton>
static QStringList old_comments;

WFunctionList::WFunctionList(QWidget* parent,  Qt::WFlags fl)
    : QWidget(parent, fl)
{
  functionsSortModel_p=NULL;
  csmes_p=NULL;
  setupUi(this);
  functionsModel_p = NULL;
  connect(different_indentation_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_indentation_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_DIFFERENT_INDENTATION",true));
  connect(different_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_DIFFERENT",true));
  connect(different_comment_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_comment_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_DIFFERENT_COMMENT",true));
  connect(not_existing_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  not_existing_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_NOT_EXISTING",true));
  connect(new_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  new_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_NEW",true));
  connect(same_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  same_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_SAME",true));
  connect(different_p,SIGNAL(toggled(bool)),this,SLOT(applyDifferenceFilter()));
  different_p->setChecked(Options::get_opt_bool(QString(),"FUNCTION_LIST_MODIFICATIONS",true));
#if QT_COMPLETOR
  connect(&LRU_timer,SIGNAL(timeout()),this,SLOT(LRUTimerEvent()));
  QStringList lru=Options::get_opt_strlst(QString(),"/SELECTION_FUNCTION_LIST");
  selection_completor_p = new QCompleter(filter_p);
  selection_completor_model_p=new QStringListModel(lru,selection_completor_p);
  selection_completor_p->setModel(selection_completor_model_p);
  selection_completor_p->setModelSorting(QCompleter::UnsortedModel);
  selection_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  filter_p->setCompleter( selection_completor_p );
#endif
  filter_p->setText(Options::get_opt_str(QString(),"FUNCTION_LIST_FILTER",QString()));
#if defined( OS_LINUX ) || defined(OS_MACX)
  connect (functionsView_p,SIGNAL(clicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
#else
  connect (functionsView_p,SIGNAL(doubleClicked(const QModelIndex &)),
      this,SLOT(itemSelected ( const QModelIndex  & )) );
#endif
  connect(&CoverageSettings::object(),SIGNAL(modified()),
          this,SLOT(refreshView()));
}

WFunctionList::~WFunctionList()
{
   Options::set_opt(QString(),"FUNCTION_LIST_DIFFERENT_INDENTATION",different_indentation_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_DIFFERENT",different_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_DIFFERENT_COMMENT",different_comment_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_NOT_EXISTING",not_existing_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_NEW",new_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_SAME",same_p->isChecked());
   Options::set_opt(QString(),"FUNCTION_LIST_MODIFICATIONS",different_p->isChecked());
  if (functionsSortModel_p)
    delete functionsSortModel_p;
  if (functionsModel_p)
    delete functionsModel_p;
#if QT_COMPLETOR
  delete selection_completor_p;
#endif
}

void WFunctionList::setCSMes(CSMesUndoRedoFramework *csmes)
{
   if (csmes_p)
      functionsView_p->saveState();

  csmes_p=csmes;
  if (functionsSortModel_p)
    delete functionsSortModel_p;
  if (functionsModel_p)
    delete functionsModel_p;

  functionsModel_p = new FunctionListModel();
  functionsModel_p->setCSMes(csmes);
  functionsSortModel_p=new SortFilterProxy;
  functionsSortModel_p->setSourceModel(functionsModel_p);
  functionsView_p->setModel(functionsSortModel_p);
  functionsView_p->setObjectName("functions view");
  functionsView_p->setWindowTitle(tr("Method Browser"));
  on_filter_p_textChanged (filter_p->text());
  setStatusColorLevelFunction(  function_low_level,  function_medium_level);
  setStatusColorLevelClass(  class_low_level,  class_medium_level);

  disconnect (functionsModel_p,SIGNAL(updateGUI()), this,SLOT(slot_updateGUI()) );
  disconnect (functionsView_p->selectionModel(),SIGNAL(selectionChanged( const QItemSelection & , const QItemSelection & )),
      this,SLOT(selectionChanged( const QItemSelection & , const QItemSelection & )) );
  connect (functionsModel_p,SIGNAL(updateGUI()), this,SLOT(slot_updateGUI()) );
  connect (functionsView_p->selectionModel(),SIGNAL(selectionChanged( const QItemSelection & , const QItemSelection & )),
      this,SLOT(selectionChanged( const QItemSelection & , const QItemSelection & )) );
  functionsView_p->restoreState();
  functionsView_p->setRootIsDecorated(true);
  functionsView_p->setItemsExpandable(true);
  functionsView_p->setSortingEnabled(true);
  functionsView_p->setUniformRowHeights(true);
  applyDifferenceFilter();
}

void WFunctionList::on_filter_type_p_clicked(bool)
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

void WFunctionList::on_filter_p_textChanged (const QString &pattern)
{
  if (!csmes_p) return;
#if QT_COMPLETOR
  LRU_timer.stop();
  LRU_timer.setSingleShot(true);
  LRU_timer.start(LRU_TIMER_UPDATE);
#endif

  Options::set_opt(QString(),"FUNCTION_LIST_FILTER",pattern);

  filter.setPattern(pattern);
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
  functionsSortModel_p->setFilterRegExp(filter);
}

void WFunctionList::slot_updateGUI()
{
  if (!csmes_p) return;

  functionsView_p->update();
  update();
  emit updateGUI();
}

#if QT_COMPLETOR
void WFunctionList::LRUTimerEvent()
{
   if (filter.isValid())
      appendLRU(filter_p->text());
}

void WFunctionList::appendLRU(const QString &value)
{
  QStringList old_lru=Options::get_opt_strlst(QString(),"/SELECTION_FUNCTION_LIST");
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  Options::set_opt(QString(),"/SELECTION_FUNCTION_LIST",new_lru);
  selection_completor_model_p->setStringList(new_lru);
}
#endif


void WFunctionList::selectionChanged( const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/ )
{
  QModelIndexList l=functionsView_p->selectionModel()->selectedIndexes();
  for (QModelIndexList::Iterator it=l.begin();it!=l.end();++it)
    *it=functionsSortModel_p->mapToSource(*it);
}

void WFunctionList::setStatusColorLevelFunction(double low, double medium)
{
  function_low_level=low;
  function_medium_level=medium;
  if (functionsModel_p)
    functionsModel_p->setStatusColorLevelFunction(low,medium);
}


void WFunctionList::setStatusColorLevelClass(double low, double medium)
{
  class_low_level=low;
  class_medium_level=medium;
  if (functionsModel_p)
    functionsModel_p->setStatusColorLevelClass(low,medium);
}


void WFunctionList::itemSelected ( const QModelIndex  & index_proxy)
{
  int start_line=0,start_column=0,end_line=0,end_column=0;
  QModelIndex index= functionsSortModel_p->mapToSource (index_proxy);
  if (functionsModel_p->instrumentationLocation(index,start_line,start_column,end_line,end_column))
  {
    emit selectedSource(functionsModel_p->module(index),
        functionsModel_p->source(index),
        CoverageSettings::object().getSourceType(),
        start_line,start_column,end_line,end_column);
  }
}


void WFunctionList::refreshView()
{
  applyDifferenceFilter();
  if (csmes_p)
  {
    functionsModel_p->refreshView();
    functionsView_p->update();
  }
  update();
#ifdef OS_MACX
  int index=pages_p->currentIndex();
  pages_p->setCurrentIndex(1);
  pages_p->setCurrentIndex(index);
#endif
}

void WFunctionList::on_more_p_toggled(bool b)
{
  if (b)
    pages_p->setCurrentIndex(1);
  else
    pages_p->setCurrentIndex(0);
}


void WFunctionList::applyDifferenceFilter()
{
  QFont f=more_p->font();
  f.setBold(false);
  if ( ! different_indentation_p->isChecked())
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


  if (functionsModel_p)
  {
    if (has_reference)
     functionsModel_p->setFilters(
        different_indentation_p->isChecked(),
        different_p->isChecked(),
        different_comment_p->isChecked(),
        not_existing_p->isChecked(),
        new_p->isChecked(),
        same_p->isChecked());
    else
      functionsModel_p->setFilters(true,true,true,true,true,true);

     functionsSortModel_p->invalidate();
  }
}
