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

#ifndef WEXECUTION_TEXT_DUMP_H
#define WEXECUTION_TEXT_DUMP_H

class Executions;
class CSMesUndoRedoFramework;
#include <QCompleter>

#include "ui_WExecutionTextDumpUI.h"
#include <QStringListModel>
#include "csmesundoredoframework.h"

class TestExportTextReport;

class WExecutionTextDump : public QDialog, public Ui::WExecutionTextDumpUI
{
    Q_OBJECT

public:
    WExecutionTextDump(const CSMesUndoRedoFramework *,QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WExecutionTextDump();
    QString fileName() const ;
    QString formatExecuted() const;
    QString formatUnexecuted() const;

public slots:
    virtual void checkInput();

protected slots:
    virtual void languageChange();
    void apply();
    void process();
    void on_executed_format_p_textChanged( const QString & );
    void on_file_p_textChanged( const QString & );
    void on_unexecuted_format_p_textChanged( const QString & );
    void on_file_browse_p_clicked();

private:
    void saveSettings();
    friend class TestExportTextReport;
#if QT_COMPLETOR
    QCompleter *executed_format_completor_p;
    QStringListModel *executed_format_completor_model_p;
    QCompleter *file_completor_p;
    QStringListModel *file_completor_model_p;
    QCompleter *unexecuted_format_completor_p;
    QStringListModel *unexecuted_format_completor_model_p;
    static QStringList appendLRU(const QString &value,QStringList &old_lru);
#endif
};

#endif // WLOADEXECUTION_H
