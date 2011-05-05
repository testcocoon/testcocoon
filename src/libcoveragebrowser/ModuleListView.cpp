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

#include "ModuleListView.h"
#include "ModuleListModel.h"
#include "options.h"
#include <QHeaderView>
#include <QMenu>
#include "SortFilterProxy.h"


ModuleListView::ModuleListView ( QWidget * parent  ): WTreeView ( parent  )
{
  setTreeView(false);
  flatViewAction_p = new QAction(tr("Flat View"),this);
  flatViewAction_p->setCheckable(true);
  bool flat_view=Options::get_opt_bool("","MODULE_VIEW_FLAT_VIEW",true);
  flatViewAction_p->setChecked(flat_view);
  setFlatView(flat_view);
  connect (flatViewAction_p,SIGNAL(triggered(bool)),this,SLOT(setFlatView(bool)));
  setTextElideMode(Qt::ElideLeft);
  restoreState();
}

ModuleListView::~ModuleListView ()
{
  saveState();
  delete flatViewAction_p;
}

void ModuleListView::DecorateContextMenu ( QMenu *menu_p )
{
  menu_p->addAction(flatViewAction_p);
}

QString ModuleListView::menuName() const
{
  return tr("Sources");
}

ModuleListModel *ModuleListView::_model() const
{
  ModuleListModel *model_p;
  SortFilterProxy *sort_model_p=dynamic_cast<SortFilterProxy*>(model());
  if (sort_model_p)
  {
    model_p=dynamic_cast<ModuleListModel*>(sort_model_p->sourceModel());
  }
  else
    model_p=dynamic_cast<ModuleListModel*>(model());
  return model_p;
}

void ModuleListView::refreshView()
{
  ModuleListModel *model_p=_model();
  if (model_p)
     model_p->refreshView();
}

void ModuleListView::setFlatView(bool b)
{
   Options::set_opt("","MODULE_VIEW_FLAT_VIEW",b);
  ModuleListModel *model_p=_model();
  if (model_p)
     model_p->setFlatView(b);
  setRootIsDecorated(!b);
}
