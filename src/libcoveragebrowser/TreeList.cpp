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

#include <QStringList>
#include "TreeList.h"

TreeList::TreeList(const QVector<QVariant> &values,TreeList *parent)
{
  itemData=values;
  parentItem = parent;
  if (parent)
    parent->appendChild(this);
}

TreeList::~TreeList()
{
  qDeleteAll(childItems);
}

void TreeList::squeeze()
{
  childItems.squeeze();
  for (QVector<TreeList*>::Iterator it=childItems.begin(); it!=childItems.end(); ++it)
    (*it)->squeeze();
}

void TreeList::appendChild(TreeList *item)
{
  childItems.append(item);
  QString key=item->itemData.at(0).toString();
  lookup.insert(key,childItems.count()-1);
}

void TreeList::find(const QVariant&v,int column,int maxDepth,QList<int> &index,QList<TreeList*> &result) const
{
  index.clear();
  result.clear();
  _find(v,column,maxDepth,index,result) ;
}

void TreeList::_find(const QVariant&v,int column,int maxDepth,QList<int> &indexs,QList<TreeList*> &result) const
{
  if (maxDepth==0)
    return ; 
  if (childItems.isEmpty())
    return ;
  if (column==0)
    indexs=lookup.values(v.toString());
  else
  {
    QVector<TreeList*>::const_iterator it;
    int index;
    for (index=0,it=childItems.begin();it != childItems.end(); ++it,index++)
    {
      TreeList *item=*it;
      if (item->itemData.at(column)==v)
        indexs+=index;
    }
  }
  for (QList<int>::const_iterator itindex=indexs.begin();itindex!=indexs.end();++itindex)
    result+=childItems.at(*itindex);

  for (QVector<TreeList*>::const_iterator itchild=childItems.begin();itchild != childItems.end(); ++itchild)
  {
    const TreeList *item=*itchild;
    item->_find (v,column,maxDepth-1,indexs,result);
  }
}

void TreeList::setData(int column, const QVariant &v)
{
  ASSERT(column!=0);
  ASSERT(column<columnCount());
  itemData[column]=v;
}

void TreeList::setDataRecursive(int column, const QVariant &v,bool child_only)
{
  ASSERT(column!=0);
  if (!child_only)
  {
    ASSERT(column<columnCount());
    itemData[column]=v;
  }
  for (QVector<TreeList*>::Iterator it=childItems.begin();it != childItems.end(); ++it)
  {
    TreeList *item=*it;
    item->setDataRecursive(column,v,false);
  }
}

void TreeList::removeChild(int row)
{
  ASSERT(row<childCount());
  childItems.remove(row);
  lookup.clear();

  int max=childItems.count();
  for (int index=0;index<max;index++)
  {
    QString key=childItems.at(index)->itemData.at(0).toString();
    ASSERT (!lookup.contains(key));
    lookup.insert(key,index);
  }
}

TreeList *TreeList::duplicate(TreeList *parent_p) const
{
  TreeList *new_item_p=new TreeList(this->itemData, parent_p);
  for (QVector<TreeList*>::const_iterator it=childItems.begin();it != childItems.end(); ++it)
  {
    TreeList *item=*it;
    item->duplicate(new_item_p);
  }
  return new_item_p;
}

void TreeList::appendDuplicateChild(const TreeList *item_p) 
{
  item_p->duplicate(this);
}
