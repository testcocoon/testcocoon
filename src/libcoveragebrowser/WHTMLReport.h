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

#ifndef WHTMLReport_h
#define WHTMLReport_h
#include "ui_WHTMLReportUI.h"
#include "coveragebrowserpdef.h"
#include "csmes.h"
#include <QFile>
#include <QTextStream>
#include "instrumentation.h"
#if QT_COMPLETOR
#include <QCompleter>
#include <QStringListModel>
#endif
class ExecutionListModel ;
class CSMesUndoRedoFramework;

class WHTMLReport : public QDialog, public Ui::WHTMLReportUI
{
  Q_OBJECT

  public:
    WHTMLReport(const CSMesUndoRedoFramework *,QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WHTMLReport();
  private:
#if QT_COMPLETOR
    QCompleter *icon_completor_p;
    QStringListModel *icon_completor_model_p;
    QCompleter *css_file_completor_p;
    QStringListModel *css_file_completor_model_p;
    QCompleter *title_completor_p;
    QStringListModel *title_completor_model_p;
    QCompleter *file_completor_p;
    QStringListModel *file_completor_model_p;
    void appendLRU(const QString &key,const QString &value);
#endif
    void saveSettings();

    private slots:
      void on_file_p_textChanged( const QString & );
      void on_file_browse_p_clicked();
      void on_file_type_p_activated(int);
      void process();
      void checkInput();
      void apply();

  private:
    QMap<CSMes::DocumentType_t,QString> document_types;
    CSMes::DocumentType_t documentType() const;
    QString file() const;
    const CSMesUndoRedoFramework *csmes_p;
};


#endif
