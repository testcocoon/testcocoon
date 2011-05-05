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

#include "WExecutionList.h"
#include "CoverageSettings.h"
#include <QHeaderView>
#include "ExecutionListModel.h"
#include "coveragebrowserpdef.h"
#include "csmesbackgroundcomputations.h"
#include "SortFilterProxy.h"
#include "csmesundoredoframework.h"
#include "Service.h"
#include "options.h"
static QStringList old_comments;

WExecutionList::WExecutionList(QWidget* parent,  Qt::WFlags fl)
    : QWidget(parent, fl)
{
  executionsSortModel_p=NULL;
  csmes_p=NULL;
  setupUi(this);
  executionsModel_p = NULL;
#if QT_COMPLETOR
  connect(&LRU_timer,SIGNAL(timeout()),this,SLOT(LRUTimerEvent()));
  QStringList lru=Options::get_opt_strlst(QString(),"/SELECTION_EXECUTION_LIST");
  selection_completor_p = new QCompleter(filter_p);
  selection_completor_model_p=new QStringListModel(lru,selection_completor_p);
  selection_completor_p->setModel(selection_completor_model_p);
  selection_completor_p->setModelSorting(QCompleter::UnsortedModel);
  selection_completor_p->setCompletionMode(QCompleter::PopupCompletion);
  filter_p->setCompleter( selection_completor_p );
#endif
  filter_p->setText(Options::get_opt_str(QString(),"EXECUTION_LIST_FILTER",QString()));
  with_comment_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_WITH_COMMENT",false));
  without_comment_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_WITHOUT_COMMENT",false));
  all_comment_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_ALL_COMMENT",true));
  unknown_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_STATE_UNKNOWN",true));
  passed_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_STATE_PASSED",true));
  failed_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_STATE_FAILED",true));
  check_manually_p->setChecked(Options::get_opt_bool(QString(),"EXECUTION_LIST_FILTER_STATE_CHECK_MANUALLY",true));
}

WExecutionList::~WExecutionList()
{
  if (executionsSortModel_p)
    delete executionsSortModel_p;
  if (executionsModel_p)
    delete executionsModel_p;
#if QT_COMPLETOR
  delete selection_completor_p;
#endif
}

void WExecutionList::refreshView()
{
  if (csmes_p)
  {
    executionsModel_p->refreshView();
    executionsView_p->update();
  }
  update();
}

void WExecutionList::setCSMes(CSMesUndoRedoFramework *csmes)
{
   if (csmes_p)
      executionsView_p->saveState();
  csmes_p=csmes;
  if (executionsSortModel_p)
    delete executionsSortModel_p;
  if (executionsModel_p)
    delete executionsModel_p;

  executionsModel_p = new ExecutionListModel();
  executionsModel_p->setCSMes(csmes);
  executionsView_p->setCSMes(csmes);
  executionsSortModel_p=new SortFilterProxy;
  executionsSortModel_p->setSourceModel(executionsModel_p);
  executionsView_p->setModel(executionsSortModel_p);
  executionsView_p->setObjectName("executions view");
  executionsView_p->setWindowTitle(tr("Source Browser"));
  applyFilters();

  disconnect (executionsModel_p,SIGNAL(updateGUI()),
      this,SLOT(slot_updateGUI()) );
  disconnect (executionsView_p,SIGNAL(doubleClicked( const QModelIndex &  )),
      this,SLOT(executionsView_p_doubleClicked( const QModelIndex & )) );
  disconnect (executionsView_p,SIGNAL(clicked( const QModelIndex &  )),
      this,SLOT(executionsView_p_clicked( const QModelIndex & )) );
  connect (executionsView_p,SIGNAL(clicked( const QModelIndex &  )),
      this,SLOT(executionsView_p_clicked( const QModelIndex & )) );
  connect (executionsView_p,SIGNAL(doubleClicked( const QModelIndex &  )),
      this,SLOT(executionsView_p_doubleClicked( const QModelIndex & )) );
  connect (executionsModel_p,SIGNAL(updateGUI()),
      this,SLOT(slot_updateGUI()) );
  executionsView_p->restoreState();
  executionsSortModel_p->setDynamicSortFilter(false);
  executionsView_p->setRootIsDecorated(true);
  executionsView_p->setItemsExpandable(true);
  executionsView_p->setSortingEnabled(true);
  executionsView_p->setUniformRowHeights(true);
}

void WExecutionList::on_filter_type_p_clicked(bool)
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

void WExecutionList::applyFilters()
{
  if (!csmes_p) return;
#if QT_COMPLETOR
  LRU_timer.stop();
  LRU_timer.setSingleShot(true);
  LRU_timer.start(LRU_TIMER_UPDATE);
#endif

  Options::set_opt(QString(),"EXECUTION_LIST_FILTER",filter_p->text());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_WITH_COMMENT",with_comment_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_WITHOUT_COMMENT",without_comment_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_ALL_COMMENT",all_comment_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_STATE_UNKNOWN",unknown_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_STATE_PASSED",passed_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_STATE_FAILED",failed_p->isChecked());
  Options::set_opt(QString(),"EXECUTION_LIST_FILTER_STATE_CHECK_MANUALLY",check_manually_p->isChecked());

  filter.setPattern(filter_p->text());
  bool filter_valid=filter.isValid();
  deselect_p->setEnabled(filter_valid);
  select_p->setEnabled(filter_valid);
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

  bool bold=false;
  if (!unknown_p->isChecked())
     bold=true;
  else if (!passed_p->isChecked())
     bold=true;
  else if (!failed_p->isChecked())
     bold=true;
  else if (!check_manually_p->isChecked())
     bold=true;
  else if (!all_comment_p->isChecked())
     bold=true;

  QFont f=more_p->font();
  f.setBold(bold);
  more_p->setFont(f);

  executionsModel_p->setFilter(
      all_comment_p->isChecked() || with_comment_p->isChecked(),
      all_comment_p->isChecked() || without_comment_p->isChecked(),
      unknown_p->isChecked(),
      passed_p->isChecked(),
      failed_p->isChecked(),
      check_manually_p->isChecked());
  executionsSortModel_p->setFilterRegExp(filter);
  //executionsSortModel_p->invalidate();
}

void WExecutionList::slot_updateGUI()
{
  if (!csmes_p) return;

  executionsView_p->repaint();
  emit updateGUI();
}


void WExecutionList::on_select_p_clicked()
{
  if (!csmes_p) return;
  CSMesBackgroundComputations::Pauser statistic_pauser;

  executionsModel_p->Select(filter_p->text(),
     all_comment_p->isChecked() || with_comment_p->isChecked(),
     all_comment_p->isChecked() || without_comment_p->isChecked(),
     unknown_p->isChecked(),
     passed_p->isChecked(),
     failed_p->isChecked(),
     check_manually_p->isChecked());
#if QT_COMPLETOR
  if (filter.isValid())
     appendLRU(filter_p->text());
#endif
}

void WExecutionList::on_deselect_p_clicked()
{
  if (!csmes_p) return;
  CSMesBackgroundComputations::Pauser statistic_pauser;

  executionsModel_p->DeSelect(filter_p->text(),
     all_comment_p->isChecked() || with_comment_p->isChecked(),
     all_comment_p->isChecked() || without_comment_p->isChecked(),
     unknown_p->isChecked(),
     passed_p->isChecked(),
     failed_p->isChecked(),
     check_manually_p->isChecked());
#if QT_COMPLETOR
  if (filter.isValid())
     appendLRU(filter_p->text());
#endif
}

#if QT_COMPLETOR
void WExecutionList::LRUTimerEvent()
{
   if (filter.isValid())
      appendLRU(filter_p->text());
}

void WExecutionList::appendLRU(const QString &value)
{
  QStringList old_lru=Options::get_opt_strlst(QString(),"/SELECTION_EXECUTION_LIST");
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  Options::set_opt(QString(),"/SELECTION_EXECUTION_LIST",new_lru);
  selection_completor_model_p->setStringList(new_lru);
}
#endif

void WExecutionList::executionsView_p_doubleClicked ( const QModelIndex & index )
{
  if (executionsSortModel_p)
  {
    CSMesBackgroundComputations::Pauser statistic_pauser;
    QModelIndex modelIndex=executionsSortModel_p->mapToSource(index);
    QVariant data=executionsModel_p->data(modelIndex,ExecutionNameRole);
    if (data.type()!=QVariant::Invalid)
    {
      QModelIndexList l;
      l+=modelIndex;
      executionsModel_p->Comparaison(l);
    }
  }
}

void WExecutionList::executionsView_p_clicked ( const QModelIndex & index )
{
  if (executionsSortModel_p)
  {
    CSMesBackgroundComputations::Pauser statistic_pauser;
    QModelIndex modelIndex=executionsSortModel_p->mapToSource(index);
    QVariant data=executionsModel_p->data(modelIndex,CommentExplanationRole);
    QString text=data.toString();
    setExplanation(text);
    data=executionsModel_p->data(modelIndex,CommentRole);
    if (data.type()==QVariant::Invalid)
    {
      emit setExecutionCommentAvailable(false);
      emit setExecutionComment(QString(),QString());
    }
    else
    {
      text=data.toString();
      data=executionsModel_p->data(modelIndex,ExecutionNameRole);
      if (data.type()==QVariant::Invalid)
      {
        emit setExecutionCommentAvailable(false);
        emit setExecutionComment(QString(),QString());
      }
      else
      {
        QString execution_name=data.toString();
        emit setExecutionCommentAvailable(true);
        emit setExecutionComment(execution_name,text);
      }
    }
  }
}

void WExecutionList::on_more_p_toggled(bool b)
{
  if (b)
    pages_p->setCurrentIndex(1);
  else
    pages_p->setCurrentIndex(0);
}

void WExecutionList::selectComparaison( const QStringList& selection_list, const QStringList &cmp_items,bool test_coverage_mode,Instrumentation::coverage_method_t method, bool execution_comparaison_mode, CSMes::comparaison_mode_t csmes_comparaison_mode, int coverage_level)
{
  if (executionsModel_p)
    executionsModel_p->selectComparaison(selection_list, cmp_items,test_coverage_mode,method, execution_comparaison_mode, csmes_comparaison_mode, coverage_level);
}
