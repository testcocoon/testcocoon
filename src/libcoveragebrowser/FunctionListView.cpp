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

#include "FunctionListView.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMessageBox>
#include "FunctionListModel.h"
#include "SortFilterProxy.h"
#include <QHeaderView>
#include "options.h"

FunctionListView::FunctionListView ( QWidget * parent  ): WTreeView ( parent  )
{
   restoreState();
}

FunctionListView::~FunctionListView ()
{
  saveState();
}

void FunctionListView::DecorateContextMenu ( QMenu * /*menu_p*/ )
{
  QModelIndexList sel = selectedIndexes();
  if (sel.isEmpty())
    return;
}

void FunctionListView::refreshView()
{
  FunctionListModel* model_p=_model() ;
  if (model_p)
    model_p->refreshView();
}

QString FunctionListView::menuName() const
{
  return tr("Methods");
}

FunctionListModel* FunctionListView::_model() const
{
  FunctionListModel *model_p;
  SortFilterProxy *sort_model_p=dynamic_cast<SortFilterProxy*>(model()); 
  if (sort_model_p)
  {
    model_p=dynamic_cast<FunctionListModel*>(sort_model_p->sourceModel());
  }
  else
    model_p=dynamic_cast<FunctionListModel*>(model());
  return model_p;
}
