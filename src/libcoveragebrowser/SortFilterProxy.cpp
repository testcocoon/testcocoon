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


#include "SortFilterProxy.h"
SortFilterProxy::SortFilterProxy(QObject *parent) : QSortFilterProxyModel(parent) 
{
   setFilterRole(SearchTextRole);
   setRecurse(true);
}

bool SortFilterProxy::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
   const int column=0;
   // Filter Mask/Value check
   QModelIndex item=sourceModel()->index(source_row,column,source_parent);
   QVariant filter=item.data(FilterRole);
   if (filter.isValid())
   {
      bool f=filter.toBool();
      if (f)
         return false;
   }
   

   // parent check
   QModelIndex p=source_parent;

   while (p.isValid())
   {
      if (QSortFilterProxyModel::filterAcceptsRow(p.row(),p.parent()))
         return true;
      p=p.parent();
   }


   // Child check
   if (QSortFilterProxyModel::filterAcceptsRow(source_row,source_parent))
      return true;
   else
   {
      if (! _recurse)
         return false;

      QModelIndex item=sourceModel()->index(source_row,column,source_parent);
      for (int r=0;;r++)
      {
         if (! item.child(r,column).isValid() ) 
            return false;

         if (filterAcceptsRow(r,item))
            return true;
      }
   }
}
