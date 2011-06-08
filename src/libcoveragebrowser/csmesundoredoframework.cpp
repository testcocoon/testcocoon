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

#include "csmesundoredoframework.h"
#include "undocomment.h"
#include "undoexecutioncomment.h"
#include "undoexecutionstatus.h"
#include "undoexecutionstatusstr.h"
#include "undoexecutioninsert.h"
#include "undoexecutiondelete.h"
#include "undoexecutionbackup.h"
#include "undoexecutionmerge.h"
#include "undoexecutionsettings.h"
#include "undoexecutionrename.h"
#include "undomanuallyvalidated.h"
#include "WMain.h"
#include <QList>
#include <QFile>
#include <QBuffer>
#include <QTextStream>
#include <QUndoStack>
#include <QTime>
#include "Service.h"

CSMesUndoRedoFramework::CSMesUndoRedoFramework(WMain *wmain_p) : QObject(),CSMes()
{
  m_wmain_p=wmain_p;
  undo_stack_p =  new QUndoStack();
  connect(undo_stack_p,SIGNAL(indexChanged ( int ) ),this, SLOT(undoRedoPerformed()));
  number_of_undo_group_call=0;
  number_of_undo_group_children=0;
  undo_group_p=NULL;
}

CSMesUndoRedoFramework::~CSMesUndoRedoFramework() 
{
  delete undo_stack_p;
}

void CSMesUndoRedoFramework::undoRedoPerformed()
{
  CoverageSettings::object().notifyModification();
}
  
void CSMesUndoRedoFramework::beginUndoGroup(const QString &description)
{
  ASSERT(number_of_undo_group_call>=0);
  if (number_of_undo_group_call==0)
  {
    ASSERT(undo_group_p==NULL);
    undo_group_p=new QUndoCommand(QTime::currentTime().toString(UNDO_TIME_FORMAT)+description);
    number_of_undo_group_children=0;
  }
  number_of_undo_group_call++;
}

void CSMesUndoRedoFramework::endUndoGroup(const QString &description)
{
  ASSERT(undo_group_p!=NULL);
  ASSERT(number_of_undo_group_call>0);
  number_of_undo_group_call--;
  if (number_of_undo_group_call==0)
  {
    if (number_of_undo_group_children==0)
      delete undo_group_p;
    else
    {
      if (!description.isEmpty())
        undo_group_p->setText(description);
      undo_stack_p->push(undo_group_p);
    }
    undo_group_p=NULL;
  }
}

QUndoCommand *CSMesUndoRedoFramework::undoStackInsertItem()
{
  if (undo_group_p)
  {
    number_of_undo_group_children++;
    return undo_group_p;
  }
  else
    return NULL;
}

void CSMesUndoRedoFramework::pushUndoItem(QUndoCommand *c)
{
  if (undo_group_p==NULL)
    undo_stack_p->push(c);
}

void CSMesUndoRedoFramework::setManuallyValidated(const ModuleFile & mod,const SourceFile & src,int index,bool b)
{
  bool old_v=getManuallyValidated(mod,src,index);
  if (old_v!=b)
    pushUndoItem(new UndoCmdManuallyValidated(this,mod,src,index,old_v,b,undoStackInsertItem()));
  
}

bool CSMesUndoRedoFramework::setExecutionComment(const ExecutionName &name, const QString &comment) 
{
  QString old_comment;
  getExecutionComment(name,old_comment);
  if (old_comment!=comment)
     pushUndoItem(new UndoCmdExecutionComment(this,m_wmain_p,name,old_comment,comment,undoStackInsertItem()));
  
  return true;
}


bool CSMesUndoRedoFramework::deleteExecution(const ExecutionName &v) 
{
  pushUndoItem(new UndoCmdExecutionDelete(this,v,undoStackInsertItem()));
  return true;
}

bool CSMesUndoRedoFramework::renameExecution(const ExecutionName &old_name,const ExecutionName &new_name) 
{
  pushUndoItem(new UndoCmdExecutionRename(this,old_name,new_name,undoStackInsertItem()));
  return true;
}

bool CSMesUndoRedoFramework::selectExecutionsComparaison(const ExecutionNames &ms,const ExecutionNames &comparaison,bool test_coverage_mode, int coverage_level,Instrumentation::coverage_method_t method, CSMes::comparaison_mode_t m, bool execution_analysis_mode) 
{
  UndoCmdExecutionSettings *undo_item_p = new UndoCmdExecutionSettings(this,ms,comparaison,test_coverage_mode, coverage_level,method, m, execution_analysis_mode,undoStackInsertItem());
  if (undo_item_p->nop())
  {
    delete undo_item_p;
    return true;
  }
  pushUndoItem(undo_item_p);
  return true;
}

bool CSMesUndoRedoFramework::renameExecutionPath(const ExecutionName &old_name,const ExecutionName &new_name) 
{
  ExecutionName old_path=old_name;
  if (old_path.right(1)!="/")
    old_path+="/";
  ExecutionName new_path=new_name;
  if (new_path.right(1)!="/")
    new_path+="/";
  ExecutionNames execution_list=executions.getExecutionList();
  ExecutionNames::const_iterator it;
  if (!executionPathExists(old_name))
    return false;
  int old_path_lg=old_path.length();

  for (it = execution_list.begin(); it != execution_list.end() ; ++it ) 
  {
    if ((*it).indexOf(old_path)==0)
    {
      QString new_item=(*it);
      new_item.replace(0,old_path_lg,new_path);
      if (executionExists(new_item))
        return false;
    }
  }

  beginUndoGroup(tr("Renaming executions '%1' -> '%2'").arg(old_name).arg(new_name));
  for (it = execution_list.begin(); it != execution_list.end() ; ++it ) 
  {
    if ((*it).indexOf(old_path)==0)
    {
      QString old_item=(*it);
      QString new_item=(*it);
      new_item.replace(0,old_path_lg,new_path);
      renameExecution(old_item,new_item);
    }
  }
  endUndoGroup();
  return true;
}

bool CSMesUndoRedoFramework::deleteExecution(const ExecutionNames &ms)
{
  if (ms.isEmpty())
    return false;
  if (ms.count()>1)
    beginUndoGroup(tr("Delete %1 executions").arg(QString::number(ms.count())));
  else
    beginUndoGroup(tr("Delete execution '%1'").arg(ms.at(0)));
  for (ExecutionNames::const_iterator it=ms.begin();it!=ms.end();++it)
    pushUndoItem(new UndoCmdExecutionDelete(this,*it,undoStackInsertItem()));
  endUndoGroup();
  return true;
}

bool CSMesUndoRedoFramework::deleteExecutionPath(const QString &v) 
{
  ExecutionNames to_remove;
  QString path=v;
  if (v.right(1)!="/")
    path+="/";

  beginUndoGroup(tr("Delete executions '%1*'").arg(path));
  ExecutionNames execution_list=executions.getExecutionList();
  for (ExecutionNames::const_iterator it = execution_list.begin(); it != execution_list.end(); ++it ) 
  {
    if ((*it).indexOf(path)==0)
      to_remove+=*it;
  }
  bool ret = deleteExecution(to_remove);
  endUndoGroup();
  return ret;
}

bool CSMesUndoRedoFramework::mergeExecutions(const ExecutionNames &sources,const ExecutionName &dest)
{
  pushUndoItem(new UndoCmdExecutionMerge(this,sources,dest,undoStackInsertItem()));
  return true;
}

void CSMesUndoRedoFramework::setExecutionStatus(const ExecutionName &name,Executions::execution_status_t e)
{
  pushUndoItem(new UndoCmdExecutionStatus(this,name,e,undoStackInsertItem()));
}

void CSMesUndoRedoFramework::setExecutionStatusStr(const ExecutionName &name,const QString &execution_status) 
{
  pushUndoItem(new UndoCmdExecutionStatusStr(this,name,execution_status,undoStackInsertItem()));
}

bool CSMesUndoRedoFramework::loadCSMes(const QString &file)
{
  bool ret = CSMes::loadCSMes(file);
  if (ret)
    undo_stack_p->clear();
  return ret;
}

void CSMesUndoRedoFramework::insert_execution_undo_stack(const ExecutionNames &new_executions)
{
  if (!new_executions.isEmpty())
  {
    QString title="Load "+QString::number(new_executions.count())+ " Executions";
    beginUndoGroup(title);
    for (ExecutionNames::const_iterator it=new_executions.begin();it!=new_executions.end();++it)
      pushUndoItem(new UndoCmdExecutionInsert(this,*it,undoStackInsertItem()));
    endUndoGroup();
  }
}

bool CSMesUndoRedoFramework::loadCSExe(const QString &file,const ExecutionName &name,csexe_import_policy_t p,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,bool use_undo,progress_function_t progress)
{
  QFile f(file);
  return loadCSExe(f,name,p,default_execution_status,new_executions,info,short_status,use_undo,progress);
}

bool CSMesUndoRedoFramework::loadCSExe(const QByteArray &data,const ExecutionName &name,csexe_import_policy_t p,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,bool use_undo,progress_function_t progress)
{
  QBuffer file;
  file.setData(data);
  return loadCSExe(file,name,p,default_execution_status,new_executions,info,short_status,use_undo,progress);
}

bool CSMesUndoRedoFramework::loadCSExe(QIODevice &data,const ExecutionName &name,csexe_import_policy_t p,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,bool use_undo,progress_function_t progress)
{
  if (!lock_csexe(data))
    return false;
  bool ret=false;
  if (use_undo)
  {
    QString title=tr("Load Executions");
    beginUndoGroup(title);
    QHash<ExecutionName,Executions::modules_executions_private_t> undo_backup;
    QString err;
    ret = CSMes::loadCSExe(data,name,p,default_execution_status,new_executions,info,short_status,err,&undo_backup,progress);

    for (QHash<ExecutionName,Executions::modules_executions_private_t>::const_iterator modifit=undo_backup.begin();
        modifit!=undo_backup.end();++modifit)
      pushUndoItem(new UndoCmdExecutionBackup(this,modifit.key(),modifit.value(),undoStackInsertItem()));
    for (ExecutionNames::const_iterator it=new_executions.begin();it!=new_executions.end();++it)
      pushUndoItem(new UndoCmdExecutionInsert(this,*it,undoStackInsertItem()));

    title += " ("+short_status+")";
    endUndoGroup(title);
    unlock_csexe(data);
  }
  else
  {
    clearUndoStack();
    QString err;
    ret = CSMes::loadCSExe(data,name,p,default_execution_status,new_executions,info,short_status,err,NULL,progress);
  }

  return ret;
}

bool CSMesUndoRedoFramework::mergeCSMes(const QString &s,CSMESFile::merge_policy_t policy)
{
  bool ret = CSMes::mergeCSMes(s,policy);
  undo_stack_p->clear();
  return ret;
}

bool CSMesUndoRedoFramework::saveCSMes()
{
  bool ret = CSMes::saveCSMes();
  if (ret)
    undo_stack_p->clear();
  return ret;
}


void CSMesUndoRedoFramework::closeCSMes()
{
  CSMes::closeCSMes();
  undo_stack_p->clear();
}

void CSMesUndoRedoFramework::clear()
{
  CSMes::clear();
  undo_stack_p->clear();
}

void CSMesUndoRedoFramework::clearUndoStack()
{
  undo_stack_p->clear();
}

bool CSMesUndoRedoFramework::setComment(const ModuleFile & module,const SourceFile & source,int instrument_index,const QString &comment)
{
  QString old_comment=getComment(module,source,instrument_index);
  if (old_comment!=comment)
     pushUndoItem(new UndoCmdComment(this,module,source,instrument_index,old_comment,comment,undoStackInsertItem()));
  
  return true;
}

