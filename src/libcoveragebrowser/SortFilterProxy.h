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

#ifndef SORTFILTERPROXY_H
#define SORTFILTERPROXY_H
#include "coveragebrowserpdef.h"

#include <QSortFilterProxyModel>

class SortFilterProxy : public QSortFilterProxyModel
{
   public: 
      SortFilterProxy(QObject *parent=0) ;
      void setRecurse(bool b) { _recurse=b; }

   protected:
      virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

   private:
      bool _recurse;
} ;

#endif
