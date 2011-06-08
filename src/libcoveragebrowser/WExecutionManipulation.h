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

#ifndef WEXECUTIONMANIPULATION_H
#define WEXECUTIONMANIPULATION_H

class Executions;
class CSMesUndoRedoFramework;
#include <QCompleter>
#include <QStringList>

#include "ui_WExecutionManipulationUI.h"
#include <QStringListModel> 
#include "csmesundoredoframework.h"

class WExecutionManipulation : public QDialog, public Ui::WExecutionManipulationUI
{
    Q_OBJECT

public:
    WExecutionManipulation(const CSMesUndoRedoFramework *,QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WExecutionManipulation();

    ExecutionName source() const;
    ExecutionName destination() const;
    ExecutionNames sourceList() const;
    ExecutionNames destinationList() const;

public slots:
    virtual void on_destination_p_textChanged( const QString & );
    virtual void on_source_p_textChanged( const QString & );
    virtual void checkInput();

protected slots:
    virtual void languageChange();

protected:
    void setSourceName(const QString &);
    void setDestinationName(const QString &);
    void setUniqueExecutionOutput(bool b) { unique_execution_output=b; }
private:
#if QT_COMPLETOR
    QCompleter *source_completor_p;
    QStringListModel *source_completor_model_p;
    QCompleter *destination_completor_p;
    QStringListModel *destination_completor_model_p;
    static QStringList appendLRU(const QString &value,QStringList &old_lru);
    static QStringList appendLRUatEnd(const QStringList &values,QStringList &old_lru);
#endif
    const CSMesUndoRedoFramework *csmes_p;
    ExecutionName  _destination,_source;
    ExecutionNames _sourceList,_destinationList;
    void generateSourceDestinationList();
    void updatePreview();
    bool unique_execution_output;
};

#endif // WLOADEXECUTION_H
