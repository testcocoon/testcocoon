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

#ifndef WMODULE_LIST_H
#define WMODULE_LIST_H
#include "ui_WModuleListUI.h"
#include "MatchExpr.h"
#include "coveragebrowserpdef.h"
#if QT_COMPLETOR
#include <QCompleter>
#include <QTimer>
#endif
#include <QStringListModel> 
class ModuleListModel ;
class SortFilterProxy ;
#include "csmesundoredoframework.h"

class WModuleList : public QWidget, public Ui::WModuleListUI
{
    Q_OBJECT

public:
    WModuleList(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WModuleList();
    void setStatusColorLevel(double low, double medium);
    void setCSMes(const CSMesUndoRedoFramework *);
    ModuleFile Module() const ;
    SourceFile Source() const;
public slots:
    void refreshView();
    void selectSource(const ModuleFile &module,const SourceFile &source);
signals:
    void selectedSource(const ModuleFile &module,const SourceFile &source,CSMesUndoRedoFramework::source_type_t,int start_line,int start_column,int end_line,int end_column);
private:
#if QT_COMPLETOR
    QCompleter *filter_completor_p;
    QStringListModel *filter_completor_model_p;
    QTimer LRU_timer;
    void appendLRU(const QString &value);
#endif
    const CSMesUndoRedoFramework *csmes_p;
    SortFilterProxy *modulesSortModel_p;
    ModuleListModel *modulesModel_p;
    double low_level,medium_level;
    MatchExpr filter;
private slots:
#if QT_COMPLETOR
  void LRUTimerEvent();
#endif
  void applyDifferenceFilter();
  void itemSelected ( const QModelIndex  & );
  void on_filter_type_p_clicked(bool);
  void on_filter_p_textChanged (const QString &pattern);
  void on_more_p_toggled(bool b);
};


#endif
