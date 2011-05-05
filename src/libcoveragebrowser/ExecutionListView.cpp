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

#include "ExecutionListView.h"
#include "WExecutionRename.h"
#include "WExecutionDelete.h"
#include "WExecutionMerge.h"
#include "WComment.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMessageBox>
#include "ExecutionListModel.h"
#include "ExecutionListDelegate.h"
#include "SortFilterProxy.h"
#include <QHeaderView>
#include <QMessageBox>
#include "options.h"
#include "Service.h"
#include "csmesbackgroundcomputations.h"
#include "csmesundoredoframework.h"

ExecutionListView::ExecutionListView ( QWidget * parent  ): WTreeView ( parent  )
{
  csmes_p=NULL;
  state_edit_p=new ExecutionListDelegate();
  setItemDelegate(state_edit_p);
  restoreState();
}

ExecutionListView::~ExecutionListView ()
{
  saveState();
  delete state_edit_p;
}

void ExecutionListView::setCSMes(CSMesUndoRedoFramework *c)
{
  csmes_p=c;
}


void ExecutionListView::DecorateContextMenu ( QMenu * menu_p )
{
  QModelIndexList sel = selectedIndexes();
  if (sel.isEmpty())
    return;
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  SortFilterProxy *sort_model_p=_sort_model();
  bool editable=false;
  for (int i=0;i<sel.count();i++)
  {
    QModelIndex index;
    if (sort_model_p)
      index=sort_model_p->mapToSource(sel.at(i));
    else
      index=sel.at(i);
    Qt::ItemFlags flags=model_p->flags(index);
    if (index.column()==0 && ( (flags&Qt::ItemIsEditable) == Qt::ItemIsEditable) )
      editable=true;
  }

  menu_p->addAction(tr("Delete"),this,SLOT(deleteIndex()));
  if (editable)
  {
    menu_p->addAction(tr("Rename..."),this,SLOT(renameIndex()));
    menu_p->addAction(tr("Place in folder..."),this,SLOT(moveIndexToFolder()));
    menu_p->addAction(tr("Comment..."),this,SLOT(setComment()));
  }
  if (csmes_p)
  {
    menu_p->addSeparator();
    menu_p->addAction(tr("Rename multiple executions..."),this,SLOT(renameRegex()));
    menu_p->addAction(tr("Delete multiple executions..."),this,SLOT(deleteRegex()));
    menu_p->addAction(tr("Merge executions together..."),this,SLOT(mergeRegex()));
  }
}


void ExecutionListView::refreshView()
{
  ExecutionListModel *model_p=_model();
  if (model_p)
     model_p->refresh();
}

void ExecutionListView::deleteIndex()
{
  QModelIndexList sel = selectedIndexes();
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
    
  switch (QMessageBox::question(this, tr("Deleting Execution"),
     tr("Are you sure?"),QMessageBox::Ok,QMessageBox::Cancel))
  {
  case QMessageBox::Ok:
     break;
  default:
     return ;
  }
  
  SortFilterProxy *sort_model_p=_sort_model();
  for (QModelIndexList::Iterator it=sel.begin();it!=sel.end();++it)
  {
    bool ok;
    QModelIndex index;
    if (sort_model_p)
      index=sort_model_p->mapToSource(*it);
    else
      index=*it;

    QString name=model_p->executionName(index);
    ok=model_p->deleteIndex(index);
    if (!ok)
      QMessageBox::warning ( this, tr("Deletion") , tr("Deletion of  %1 failed").arg(name),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  }
}

QString ExecutionListView::menuName() const
{
  return tr("Executions");
}

SortFilterProxy *ExecutionListView::_sort_model() const
{
  SortFilterProxy *sort_model_p=dynamic_cast<SortFilterProxy*>(model()); 
  return sort_model_p;
}

ExecutionListModel *ExecutionListView::_model() const
{
  ExecutionListModel *model_p;
  SortFilterProxy *sort_model_p=_sort_model();
  if (sort_model_p)
  {
    model_p=dynamic_cast<ExecutionListModel*>(sort_model_p->sourceModel());
  }
  else
    model_p=dynamic_cast<ExecutionListModel*>(model());
  return model_p;
}

void ExecutionListView::moveIndexToFolder()
{
  QModelIndexList sel = selectedIndexes();
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  SortFilterProxy *sort_model_p=_sort_model();
  for (QModelIndexList::Iterator it=sel.begin();it!=sel.end();++it)
  {
    bool ok;
    QModelIndex index;
    if (sort_model_p)
      index=sort_model_p->mapToSource(*it);
    else
      index=*it;

    QString old_full_name=model_p->executionName(index);
    int pos=old_full_name.lastIndexOf("/");
    QString old_path,old_name;
    if (pos>0)
    {
      old_path=old_full_name.mid(0,pos);
      old_name=old_full_name.mid(pos+1);
    }
    else
    {
      old_path="";
      old_name=old_full_name;
    }
    QString new_path=QInputDialog::getText(this,tr("Move to folder"),tr("Folder:"),QLineEdit::Normal,old_path,&ok);
    QString new_full_name;
    if (new_path=="")
      new_full_name=old_name;
    else 
      new_full_name=new_path+"/"+old_name;

    if (old_full_name==new_full_name)
      return;

    if (ok)
      ok=model_p->renameIndex(index,new_full_name);
    else 
       return ;
       
    if (!ok)
      QMessageBox::warning ( this, tr("Renaming") , tr("Renaming %1 failed").arg(old_full_name),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  }
}

void ExecutionListView::setComment()
{
  QModelIndexList sel = selectedIndexes();
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;

  SortFilterProxy *sort_model_p=_sort_model();
  for (QModelIndexList::Iterator it=sel.begin();it!=sel.end();++it)
  {
    QModelIndex index;
    if (sort_model_p)
      index=sort_model_p->mapToSource(*it);
    else
      index=*it;

    QString name=model_p->executionName(index);
    QString comment ;
    if (!csmes_p->getExecutionComment(name,comment))
    {
      QMessageBox::warning ( this, 
          tr("Commenting") , 
          tr("Reading comment of %1 failed").arg(name),
          QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
      return ;
    }
    WComment *comment_p = new WComment();
    comment_p->setCommentLengthCheck(false);
    comment_p->setText(comment);
    if (comment_p->exec())
    {
      comment=comment_p->text();
      if (!model_p->setExecutionComment(index,comment))
      {
        QMessageBox::warning ( this, 
            tr("Commenting") , 
            tr("Changing comment of %1 failed").arg(name),
            QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return ;
      }
    }
  }
}

void ExecutionListView::renameIndex()
{
  QModelIndexList sel = selectedIndexes();
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  SortFilterProxy *sort_model_p=_sort_model();
  for (QModelIndexList::Iterator it=sel.begin();it!=sel.end();++it)
  {
    bool ok;
    QModelIndex index;
    if (sort_model_p)
      index=sort_model_p->mapToSource(*it);
    else
      index=*it;

    QString old_name=model_p->executionName(index);
    QString new_name=QInputDialog::getText(this,tr("Rename execution"),tr("Name:"),QLineEdit::Normal,old_name,&ok);
    if (ok)
      ok=model_p->renameIndex(index,new_name);
    else 
       return ;
      
    if (!ok)
      QMessageBox::warning ( this, tr("Renaming") , tr("Renaming %1 failed").arg(old_name),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  }
}

void ExecutionListView::renameRegex()
{
  CSMesBackgroundComputations::Pauser pauser;
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  WExecutionRename dialog(csmes_p,this);
  if (dialog.exec())
  {
    csmes_p->beginUndoGroup(tr("Rename executions '%1' -> '%2'").arg(dialog.source()).arg(dialog.destination()));
    QStringList sources=dialog.sourceList();
    QStringList destinations=dialog.destinationList();
    int sz=sources.size();
    for (int i=0;i<sz;i++)
    {
      printStatus(tr("Renaming '%1' in '%2'...").arg(sources.at(i)).arg(destinations.at(i)),static_cast<float>(i)/static_cast<float>(sz));
      if (!model_p->renameExecution(sources.at(i),destinations.at(i)))
      {
         if (QMessageBox::warning(this,tr("Error"),tr("Renaming '%1' in '%2' not possible").arg(sources.at(i)).arg(destinations.at(i)),
              QMessageBox::Abort, QMessageBox::Ignore)==QMessageBox::Abort)
           break;
      }
    }
    csmes_p->endUndoGroup();
    printStatus("",-1);
  }
}

void ExecutionListView::deleteRegex()
{
  CSMesBackgroundComputations::Pauser pauser;
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  WExecutionDelete dialog(csmes_p,this);
  if (dialog.exec())
  {
    QStringList sources=dialog.sourceList();
    csmes_p->beginUndoGroup(tr("Delete executions '%1'").arg(dialog.source()));
    int sz=sources.size();
    for (int i=0;i<sz;i++)
    {
      printStatus(tr("Deleting '%1'...").arg(sources.at(i)),static_cast<float>(i)/static_cast<float>(sz));
      if (!model_p->deleteExecution(sources.at(i)))
      {
        if (QMessageBox::warning(this,tr("Error"),tr("Deleting '%1' not possible.").arg(sources.at(i)),
              QMessageBox::Abort, QMessageBox::Ignore)==QMessageBox::Abort)
            break;
      }
    }
    csmes_p->endUndoGroup();
    printStatus("",-1);
  }
}

void ExecutionListView::mergeRegex()
{
  CSMesBackgroundComputations::Pauser pauser;
  ExecutionListModel *model_p=_model();
  if (! model_p)
    return;
  WExecutionMerge dialog(csmes_p,this);
  if (dialog.exec())
  {
    csmes_p->beginUndoGroup(tr("Merge executions '%1' -> '%2'").arg(dialog.source()).arg(dialog.destination()));
    QStringList sources=dialog.sourceList();
    QString name=dialog.destinationExecution();
    printStatus(tr("Generating '%1'...").arg(name),0.0);
    if (!model_p->mergeExecutions(sources,name))
    {
       QMessageBox::warning(this,tr("Error"),tr("Merging not possible."));
    }
    else
    {
      QStringList sources=dialog.sourceList();
      int sz=sources.size();
      for (int i=0;i<sz;i++)
      {
        printStatus(tr("Deleting '%1'...").arg(sources.at(i)),static_cast<float>(i+1)/static_cast<float>(sz+1));
        if (!model_p->deleteExecution(sources.at(i)))
        {
          if (QMessageBox::warning(this,tr("Error"),tr("Deleting '%1' not possible.").arg(sources.at(i)),
                QMessageBox::Abort, QMessageBox::Ignore)==QMessageBox::Abort)
            break;
        }
      }
    }
    csmes_p->endUndoGroup();
  }
  printStatus("",-1);
}

