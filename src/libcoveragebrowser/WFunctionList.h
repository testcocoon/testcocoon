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

#ifndef WFUNCTION_LIST_H
#define WFUNCTION_LIST_H
#include "ui_WFunctionListUI.h"
#include "coveragebrowserpdef.h"
#include <QStringListModel> 
#if QT_COMPLETOR
#include <QCompleter>
#include <QTimer>
#endif
#include "MatchExpr.h"
class FunctionListModel ;
class SortFilterProxy ;
#include "csmesundoredoframework.h"

class WFunctionList : public QWidget, public Ui::WFunctionListUI
{
    Q_OBJECT

public:
    WFunctionList(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WFunctionList();
    void setCSMes(CSMesUndoRedoFramework *);
    void setStatusColorLevelFunction(double low, double medium);
    void setStatusColorLevelClass(double low, double medium);
signals:
    void updateGUI();
    void selectedSource(const ModuleFile &module,const SourceFile &source,CSMesUndoRedoFramework::source_type_t,int start_line,int start_column,int end_line,int end_column);
private:
    double class_low_level, class_medium_level, function_low_level, function_medium_level;
    MatchExpr filter;
#if QT_COMPLETOR
    QTimer LRU_timer;
    QCompleter *selection_completor_p;
    QStringListModel *selection_completor_model_p;
    void appendLRU(const QString &value);
#endif
    SortFilterProxy *functionsSortModel_p;
    FunctionListModel *functionsModel_p;
    const CSMesUndoRedoFramework *csmes_p;

private slots:
  void on_more_p_toggled(bool b);
  void itemSelected ( const QModelIndex  & );
  void on_filter_type_p_clicked(bool);
  void on_filter_p_textChanged (const QString &pattern);
  void slot_updateGUI();
  void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
#if QT_COMPLETOR
  void LRUTimerEvent();
#endif
  void applyDifferenceFilter();
  public slots:
    void refreshView();
};


#endif
