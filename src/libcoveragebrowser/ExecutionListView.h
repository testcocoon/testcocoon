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

#ifndef EXECUTION_LIST_VIEW_H
#define EXECUTION_LIST_VIEW_H
#include "WTreeView.h"
class ExecutionListDelegate;
class CSMesUndoRedoFramework ;
class ExecutionListModel ;
class SortFilterProxy ;

class ExecutionListView : public WTreeView
{
  Q_OBJECT
public:
  ExecutionListView ( QWidget * parent = 0 );
  virtual ~ExecutionListView ( );
  void setCSMes(CSMesUndoRedoFramework *);
protected:
  virtual void DecorateContextMenu(QMenu *);
  virtual QString menuName() const;
  virtual QString saveRestoreName() const { return "EXECUTION_LIST_COLUMN";}
private slots:
  void deleteIndex();
  void renameIndex();
  void moveIndexToFolder();
  void renameRegex();
  void deleteRegex();
  void mergeRegex();
  void refreshView();
  void setComment();
private:
  CSMesUndoRedoFramework *csmes_p;
  ExecutionListDelegate *state_edit_p;
  ExecutionListModel *_model() const;
  SortFilterProxy *_sort_model() const;
};
#endif
