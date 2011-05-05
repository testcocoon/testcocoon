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

#ifndef WSOURCEVIEW_SEARCH_H
#define WSOURCEVIEW_SEARCH_H
#include "ui_WTextSourceViewerSearchUI.h"
#include "coveragebrowserpdef.h"
#include "csmesundoredoframework.h"
#include <QTextDocument>
#include <QStyle>
#include <QCompleter>
#include <QStringListModel>

class WTextSourceViewerSearch : public QWidget, public Ui::WTextSourceViewerSearchUI
{
    Q_OBJECT

public:
    WTextSourceViewerSearch(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WTextSourceViewerSearch();
public slots:
  void on_search_text_p_returnPressed();
  void on_search_text_p_textChanged(const QString &text);
  void on_previous_p_clicked();
  void on_next_p_clicked();
  void toggleVisible();
signals:
  void findText ( const QString & exp, QTextDocument::FindFlags options  );
private:
  void display(bool);
  void search();
  void setSizes();
  bool backward;
  bool _force_visible;
#if QT_COMPLETOR
  QCompleter *completor_p;
  QStringListModel *completor_model_p;
  static QStringList appendLRU(const QString &value,QStringList &old_lru);
  void appendLRU();
#endif
};


#endif
