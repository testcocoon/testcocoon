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

#ifndef WMAINUI
#define WMAINUI
#include "ui_WMain.h"
#include <QWidget>
#include <QList>
#include "tools.h"

class WMainUI : public QDialog, public Ui::WMain
{
  Q_OBJECT
  public:
    WMainUI(QWidget *parent=0,Qt::WFlags f=0);
    virtual ~WMainUI();

  public slots:
    void on_close_p_clicked();
    void updateView();
    void on_list_p_itemClicked( QListWidgetItem * );
    void on_uninstall_p_clicked();
    void on_install_p_clicked();
    void on_set_console_p_clicked();
    void on_directory_p_clicked();
    void on_open_console_p_clicked();
    void on_list_p_currentItemChanged ( QListWidgetItem * /*current*/, QListWidgetItem * /*previous*/ );
  private:
    QList<Tools*> tools;
} ;

#endif
