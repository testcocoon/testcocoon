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

#ifndef _WTREEVIEW_H
#define _WTREEVIEW_H
#include "coveragebrowserpdef.h"
#include <QTreeView>
#include <QTextStream>

class WTreeView : public QTreeView
{
  Q_OBJECT
  public:
    WTreeView(QWidget *p=NULL);

    void setMenuEnable(bool); 
    void setTreeView(bool); 
    void saveState();
    void restoreState();
    virtual void refreshView()=0;
  protected:
    void contextMenuEvent ( QContextMenuEvent * e )  ;
    virtual void DecorateContextMenu(QMenu *)=0;
    virtual QString menuName() const=0;
    virtual QString saveRestoreName() const=0;
    protected slots:
    
 private slots:
    void adjustSize(); 
    void adjust1Column();
    void showHideColumn(bool b);
    void saveAs();

  private:
    enum SaveAsType { HTML, CSV };
    bool is_menu_enable;
    bool is_tree_view;
	void saveAsSection(QTextStream &stream, QModelIndex parent,QChar separator,QChar numberSep, SaveAsType, int level);
};


#endif
