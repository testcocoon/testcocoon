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

#ifndef WEXECUTION_ITEM_H
#define WEXECUTION_ITEM_H

#include "coveragebrowserpdef.h"
#include <qcolor.h>
#include <Q3ListView>
#include <qstring.h>

class WExecutionListViewItem : public Q3CheckListItem
{
public:
    WExecutionListViewItem (Q3ListViewItem *parent,const QString &,Q3CheckListItem::Type tt);
    WExecutionListViewItem (Q3ListView *parent,const QString &,Q3CheckListItem::Type tt);
    void paintCell(QPainter *painter,
                   const QColorGroup &cg, int column,
                   int width, int align);
    void setColor(const QColor &c) { color=c; }
public:
    void setup();
private:
    QColor color;
};
#endif
