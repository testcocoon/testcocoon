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

#ifndef FUNCTION_LIST_VIEW_H
#define FUNCTION_LIST_VIEW_H
#include "WTreeView.h"
class FunctionListModel;

class FunctionListView : public WTreeView
{
  Q_OBJECT
public:
  FunctionListView ( QWidget * parent = 0 );
  virtual ~FunctionListView ( );
protected:
  virtual void DecorateContextMenu(QMenu *);
  virtual QString menuName() const;
  virtual QString saveRestoreName() const { return "FUNCTION_LIST_COLUMN";}
  FunctionListModel* _model() const;
public slots:
  void refreshView();
};
#endif
