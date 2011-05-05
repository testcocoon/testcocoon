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

#ifndef TREE_LIST_H
#define TREE_LIST_H
#include "coveragebrowserpdef.h"

#include <QMap>
#include <QMultiHash>
#include <QVector>
#include <QVariant>

class TreeList
{
public:
    TreeList(const QVector<QVariant> &values, TreeList *parent = 0);
    virtual ~TreeList();


    void removeChild(int row);
    inline int childCount() const;
    inline int columnCount() const;
    inline int row() const;
    inline TreeList *parent() const;
    inline TreeList *child(int row) const;
    void squeeze();
    void setDataRecursive(int column, const QVariant &v,bool child_only);
    void setData(int column, const QVariant &v);
    QVariant& data(int column) 
    {
      ASSERT(column<columnCount());
      return itemData[column];
    }

    QVariant data(int column) const
    {
      ASSERT(column<columnCount());
      return itemData.value(column);
    }
    void find(const QVariant&,int column,int maxDepth,QList<int> &index,QList<TreeList*> &results) const;
    void appendDuplicateChild(const TreeList *item_p) ;


private:
    TreeList *duplicate(TreeList *new_parent_p) const;
    QVector<TreeList*> childItems;
    QVector<QVariant> itemData;
    TreeList *parentItem;
    void appendChild(TreeList *child);
    QMultiHash<QString,int> lookup;
    inline void _find(const QVariant&,int column,int maxDepth,QList<int> &index,QList<TreeList*> &results) const;
};


int TreeList::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeList*>(this));

    return 0;
}


TreeList *TreeList::child(int row) const
{
  ASSERT(childCount()>=row);
  if (row>=childCount())
    return NULL;
  return childItems.value(row);
}

int TreeList::childCount() const
{
    return childItems.count();
}

int TreeList::columnCount() const
{
    return itemData.count();
}

TreeList *TreeList::parent() const
{
    return parentItem;
}

#endif
