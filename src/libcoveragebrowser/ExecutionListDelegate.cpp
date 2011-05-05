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

#include "ExecutionListDelegate.h"
#include "ExecutionListModel.h"
#include <QComboBox>
#include "csmes.h"

ExecutionListDelegate::ExecutionListDelegate(QObject *parent):QItemDelegate(parent)
{
}

QWidget *ExecutionListDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option ,
    const QModelIndex & index ) const
{
  if (index.column()==EXECUTION_LIST_COLUMN_STATE)
  {
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(CSMes::executionsStatusStr());
    editor->setDuplicatesEnabled(false);
    editor->setEditable(false);
    editor->installEventFilter(const_cast<ExecutionListDelegate*>(this));

    return editor;
  }
  else
    return QItemDelegate::createEditor(parent,option , index );
}

void ExecutionListDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
  if (index.column()==EXECUTION_LIST_COLUMN_STATE)
  {
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

    QComboBox *combo_p = static_cast<QComboBox*>(editor);
    int id=combo_p->findText(value);
    combo_p->setCurrentIndex(id);
  }
  else
    QItemDelegate::setEditorData(editor, index);
}

void ExecutionListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
  if (index.column()==EXECUTION_LIST_COLUMN_STATE)
  {
    QComboBox *combo_p = static_cast<QComboBox*>(editor);
    QString value = combo_p->currentText();

    model->setData(index, value);
  }
  else
    QItemDelegate::setModelData(editor, model, index);
}

void ExecutionListDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
  if (index.column()==EXECUTION_LIST_COLUMN_STATE)
    editor->setGeometry(option.rect);
  else
    QItemDelegate::updateEditorGeometry(editor, option, index );
}
