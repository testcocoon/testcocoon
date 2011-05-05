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

#ifndef WLOADEXECUTION_H
#define WLOADEXECUTION_H

class Executions;
class CSMesUndoRedoFramework;
#include <QCompleter>

#include "ui_WLoadExecutionUI.h"
#include <QStringListModel>
#include "csmesundoredoframework.h"

class WLoadExecution : public QDialog, public Ui::WLoadExecutionUI
{
    Q_OBJECT

public:
    enum type_t { CSEXE_SCRIPT, CSEXE_FILE} ;
    enum watch_t { WATCH_DO_NOTHING=0, WATCH_OPEN_DIALOG=1, WATCH_IMPORT_AUTOMATICALLY=2 } ;
    WLoadExecution(const CSMesUndoRedoFramework *,QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WLoadExecution();

    type_t type() const;
    QString file() const;
    QString script() const;
    QStringList arguments() const;
    QString name() const;
    bool deleteAfterLoading() const;
    watch_t watch() const;
    bool applyOnly() const;
    bool minimizeMemoryUsage() const;
    CSMesUndoRedoFramework::csexe_import_policy_t importPolicy() const;
    Executions::execution_status_t defaultStatus() const;

public slots:
    virtual void on_browse_p_clicked();
    virtual void on_browse_script_p_clicked();
    virtual void on_name_p_textChanged( const QString & );
    virtual void on_file_p_textChanged( const QString & );
    virtual void checkInput();
    virtual void on_display_import_summary_p_toggled(bool value);
    void on_select_file_p_toggled(bool );
    void on_script_p_textChanged( const QString & );

protected slots:
    virtual void languageChange();
    void apply();
    void process();

private:
#if QT_COMPLETOR
    QCompleter *file_completor_p;
    QStringListModel *file_completor_model_p;
    QCompleter *name_completor_p;
    QStringListModel *name_completor_model_p;
    QCompleter *script_completor_p;
    QStringListModel *script_completor_model_p;
    QCompleter *arguments_completor_p;
    QStringListModel *arguments_completor_model_p;
    static QStringList appendLRU(const QString &value,QStringList &old_lru);
    static QStringList appendLRUatEnd(const QStringList &values,QStringList &old_lru);
#endif
    bool apply_only;
};

#endif // WLOADEXECUTION_H
