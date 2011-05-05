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

#ifndef WEXECUTION_LIST_H
#define WEXECUTION_LIST_H
#include "ui_WExecutionListUI.h"
#include "instrumentation.h"
#include "coveragebrowserpdef.h"
#include <QStringListModel>
#if QT_COMPLETOR
#include <QCompleter>
#include <QTimer>
#endif
#include "MatchExpr.h"
#include "csmes.h"
class ExecutionListModel ;
class SortFilterProxy ;
class CSMesUndoRedoFramework;

class WExecutionList : public QWidget, public Ui::WExecutionListUI
{
    Q_OBJECT

public:
    WExecutionList(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WExecutionList();
    void setCSMes(CSMesUndoRedoFramework *);
    QString Module() const ;
    QString Source() const;
    void selectComparaison( const QStringList& selection_list, const QStringList &comparaison_list,bool test_coverage_mode,Instrumentation::coverage_method_t, bool execution_comparaison_mode, CSMes::comparaison_mode_t, int coverage_level);
signals:
    void updateGUI();
    void setExplanation(const QString &);
    void setExecutionComment(const QString &name,const QString &);
    void setExecutionCommentAvailable(bool);
private:
    MatchExpr filter;
#if QT_COMPLETOR
    QTimer LRU_timer;
    QCompleter *selection_completor_p;
    QStringListModel *selection_completor_model_p;
    void appendLRU(const QString &value);
#endif
    SortFilterProxy *executionsSortModel_p;
    ExecutionListModel *executionsModel_p;
    const CSMesUndoRedoFramework *csmes_p;

public slots:
    void applyFilters();

private slots:
  void executionsView_p_doubleClicked ( const QModelIndex & index );
  void executionsView_p_clicked ( const QModelIndex & index );
  void on_filter_type_p_clicked(bool);
  void slot_updateGUI();
  void on_more_p_toggled(bool);
  void on_select_p_clicked();
  void on_deselect_p_clicked();
#if QT_COMPLETOR
  void LRUTimerEvent();
#endif
  public slots:
    void refreshView();
};


#endif
