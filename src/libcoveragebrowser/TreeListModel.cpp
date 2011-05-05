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

#include <QtGui>
#include "csmesundoredoframework.h"
#include "TreeList.h"
#include "TreeListModel.h"
#include "MatchExpr.h"
#include <QStringList>
#include "options.h"
#include "CoverageSettings.h"
#include "csmesbackgroundcomputations.h"


TreeListModel::TreeListModel(QObject *parent,const int column_count) : QAbstractItemModel(parent),_column_count(column_count)
{
  rootItem=NULL;
}

TreeListModel::~TreeListModel()
{
  delete rootItem;
}

int TreeListModel::columnCount(const QModelIndex &/*parent*/) const
{
  return _column_count;
}



Qt::ItemFlags TreeListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeListModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

QModelIndex TreeListModel::index(int row, int column, const QModelIndex &parent) const
{
  TreeList *parentItem;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<TreeList*>(parent.internalPointer());

  TreeList *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeListModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TreeList *childItem = static_cast<TreeList*>(index.internalPointer());
  if (childItem == rootItem)
    return QModelIndex();

  TreeList *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int TreeListModel::rowCount(const QModelIndex &parent) const
{
  TreeList *parentItem;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<TreeList*>(parent.internalPointer());

  return parentItem->childCount();
}


void TreeListModel::rowChanged(TreeList *e)
{
  QModelIndex index1=createIndexTreeList(e,0);
  QModelIndex index2=createIndexTreeList(e,_column_count-1);
  emit dataChanged(index1,index2);
}

QModelIndex TreeListModel::createIndexTreeList(TreeList *item_p,int column) const
{
  int row=item_p->row();
  return createIndex(row,column,item_p);
}

