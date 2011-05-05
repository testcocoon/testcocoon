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

#include "wmodulelistviewitem.h"
#include <qpainter.h>
#include <qpalette.h>
#include <qcolor.h>

WModuleListViewItem::WModuleListViewItem (Q3ListView *parent,const QString &s1,const QString &s2)
  : Q3ListViewItem(parent,s1,s2)
{
  color=COL_WHITE;
}

WModuleListViewItem::WModuleListViewItem (Q3ListViewItem *parent,const QString &s1,const QString &s2)
  : Q3ListViewItem(parent,s1,s2)
{
  color=COL_WHITE;
}

void WModuleListViewItem::setup()
{
    Q3ListViewItem::setup();
    int number_of_lines = 1;
    QFontMetrics fontmetric(listView()->font());
    setHeight(number_of_lines*fontmetric.height());
}

void WModuleListViewItem::paintCell(QPainter *painter,
                                    const QColorGroup &cg, int column,
                                    int width, int align)
{
    painter->save();
    QColorGroup grp(cg);
    grp.setColor(QColorGroup::Text,QColor("#000000"));
    grp.setColor(QColorGroup::Base,color);
    Q3ListViewItem::paintCell(painter,grp,column,width,align);
    painter->restore();
}


