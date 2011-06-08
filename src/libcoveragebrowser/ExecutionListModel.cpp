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
#include "ExecutionListModel.h"
#include "MatchExpr.h"
#include <QStringList>
#include "options.h"
#include "CoverageSettings.h"
#include "csmesbackgroundcomputations.h"
enum EXECUTION_LIST_ITEMS
{
  ITEM_NAME,
  ITEM_MEASUREMENT,
  ITEM_CHECKSTATE,
  ITEM_NB_TESTED,
  ITEM_NB_UNTESTED,
  ITEM_MAX,
};
static const int BITMAP_COMMENT=(1<<0);
static const int BITMAP_NO_COMMENT=(1<<1);
static const int BITMAP_PASSED=(1<<2);
static const int BITMAP_FAILED=(1<<3);
static const int BITMAP_UNKNOWN=(1<<4);
static const int BITMAP_CHECK_MANUALLY=(1<<5);


ExecutionListModel::ExecutionListModel(QObject *parent) : TreeListModel(parent,EXECUTION_LIST_COLUMN_MAX)
{
  csmes_p=NULL;
  connect( & CSMesBackgroundComputations::GetObject(),
      SIGNAL(statisticExecution(const ExecutionNames &,const ExecutionNames& , bool ,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t, int ,int )),
      this,
      SLOT(statisticExecution(const ExecutionNames &,const ExecutionNames& , bool ,int ,Instrumentation::coverage_method_t,CSMes::comparaison_mode_t, int ,int )));
  filter_comment=true;
  filter_no_comment=true;
  filter_unknown=true;
  filter_passed=true;
  filter_failed=true;
  filter_check_manually=true;
}

void ExecutionListModel::statisticExecution(const ExecutionNames &mes,const ExecutionNames &comparaison , bool test_count_mode,
    int coverage_level,Instrumentation::coverage_method_t method,CSMes::comparaison_mode_t comparaison_mode,int nb_tested,int nb_untested)
{
  if (CoverageSettings::object().getCoverageMethod()!=method)
    return ;
  if (coverage_level!=CoverageSettings::object().getCoverageLevel())
    return ;
  if (test_count_mode!=CoverageSettings::object().getTestCoverageMode())
    return ;
  if (comparaison_mode!=CoverageSettings::object().getReleaseComparaisonMode())
    return ;

  QList<TreeList *> items;
  if (!comparaison.isEmpty())
  {
    if (comparaison!=CoverageSettings::object().getSelectedExecutionsComparaison())
      return;

    items = executionTreeLists(mes);
  }
  else
    items = executionTreeLists(mes);
  for (QList<TreeList*>::const_iterator it=items.begin();it!=items.end();++it)
  {
    TreeList* item_p=(*it);
    item_p->data(ITEM_NB_TESTED) = nb_tested;
    item_p->data(ITEM_NB_UNTESTED) = nb_untested;
    QModelIndex index=createIndexTreeList(item_p,EXECUTION_LIST_COLUMN_STATISTIC);
    emit dataChanged(index,index);
  }
}

ExecutionListModel::~ExecutionListModel()
{
}

void ExecutionListModel::setCSMes(CSMesUndoRedoFramework *c)
{
  stopBackgroundCalculation();
  csmes_p=c;
  setupModelData();
  CoverageSettings::object().backup(last_settings);
}

void ExecutionListModel::stopBackgroundCalculation()
{
  CSMesBackgroundComputations::GetObject().stop(false);
}

QVariant ExecutionListModel::data (const QModelIndex &index, int role) const
{
  static const QString processing_text=tr("Processing...");
  if (!index.isValid())
    return QVariant();

  TreeList *item_p = static_cast<TreeList*>(index.internalPointer());
  if (index.column()==EXECUTION_LIST_COLUMN_STATISTIC)
  {
    bool ok;
    QVariant ItemNbTested=item_p->data(ITEM_NB_TESTED);
    ItemNbTested.toInt(&ok);
    if (   (ItemNbTested.toString()!=processing_text) && (!ok) )
    {
      ExecutionNames execs;
      getAllExecutions(item_p,execs);

      bool use_thread=Options::get_opt_bool(QString(),"STATISTICS_CALULATION_BACKGROUND",DEF_STATISTICS_CALULATION_BACKGROUND);
      if (use_thread)
      {
        if (CoverageSettings::object().getExecutionAnalysisMode())
          CSMesBackgroundComputations::GetObject().calculateStatisticExecution(execs,CoverageSettings::object().getSelectedExecutionsComparaison(),CoverageSettings::object().getTestCoverageMode(),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),CoverageSettings::object().getReleaseComparaisonMode());
        else
          CSMesBackgroundComputations::GetObject().calculateStatisticExecution(execs,ExecutionNames(),CoverageSettings::object().getTestCoverageMode(),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),CoverageSettings::object().getReleaseComparaisonMode());
        item_p->data(ITEM_NB_TESTED) = processing_text;
        item_p->data(ITEM_NB_UNTESTED) = processing_text;
      }
      else
      {
        int nb_tested=-1;
        int nb_untested=-1;
        if (CoverageSettings::object().getExecutionAnalysisMode())
          csmes_p->statisticExecution(execs,CoverageSettings::object().getSelectedExecutionsComparaison(),CoverageSettings::object().getTestCoverageMode(),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),CoverageSettings::object().getReleaseComparaisonMode(),nb_tested,nb_untested,false);
        else
          csmes_p->statisticExecution(execs,ExecutionNames(),CoverageSettings::object().getTestCoverageMode(),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),CoverageSettings::object().getReleaseComparaisonMode(),nb_tested,nb_untested,false);

        item_p->data(ITEM_NB_TESTED) = nb_tested;
        item_p->data(ITEM_NB_UNTESTED) = nb_untested;
      }
    }
  }
  switch (role)
  {
    default:
      return QVariant();

    case FilterRole:
      {
        bool comment=false;
        bool no_comment=false;
        bool unknown=false;
        bool passed=false;
        bool failed=false;
        bool check_manually=false;

        ExecutionNames _executions;
        getAllExecutions(item_p,_executions);
        for (ExecutionNames::const_iterator it=_executions.begin();it!=_executions.end();it++)
        {
          const QString &execution_name=*it;
          QString comment_string;
          if (csmes_p->getExecutionComment(execution_name,comment_string) )
          {
            if (comment_string.length()>0)
              comment=true;
            else
              no_comment=true;
          }

          Executions::execution_status_t status= csmes_p->getExecutionStatus(execution_name);
          unknown= unknown || (Executions::EXECUTION_STATUS_UNKNOWN==status);
          passed= passed || (Executions::EXECUTION_STATUS_PASSED==status);
          failed= failed || (Executions::EXECUTION_STATUS_FAILED==status);
          check_manually= check_manually || (Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY==status);
        }

        bool filter_state=true;
        if ((unknown) && filter_unknown) filter_state=false;
        if ((passed) && filter_passed) filter_state=false;
        if ((failed) && filter_failed) filter_state=false;
        if ((check_manually) && filter_check_manually) filter_state=false;

        if (filter_state)
          return true;

        bool filter_com=true;
        if ((comment) && filter_comment) filter_com=false;
        if ((no_comment) && filter_no_comment) filter_com=false;

        return  filter_com;
      }
    case SearchTextRole:
      return executionName(rootItem,item_p,true);
    case Qt::CheckStateRole:
      if (index.column()==ITEM_NAME)
      {
        return item_p->data(ITEM_CHECKSTATE);
      }
      break;
    case Qt::BackgroundColorRole:
      {
        if (!item_p->data(ITEM_MEASUREMENT).toBool())
          return QVariant();
        QString name=executionName(rootItem,item_p,true);
        return CSMesUndoRedoFramework::executionStatusColor(csmes_p->getExecutionStatus(name));
      }
    case Qt::DecorationRole:
      switch (index.column())
      {
        case EXECUTION_LIST_COLUMN_NAME:
          if (CoverageSettings::object().getExecutionAnalysisMode())
          {
            QString name=executionName(rootItem,item_p,true);
            if (CoverageSettings::object().getExecutionAnalysisMode())
            {
              if (CoverageSettings::object().getSelectedExecutions().contains(name))
                return QIcon (":/images/testPerformance.svg");
            }
          }
          return QVariant();
        default:
          return QVariant();
      }
    case ExecutionNameRole:
      {
        QString execution_name=executionName(rootItem,item_p,true);
        return execution_name;
      }
      break;
    case CommentRole:
      {
        QString execution_name=executionName(rootItem,item_p,false);
        QString comment;
        if (! csmes_p->getExecutionComment(execution_name,comment) )
          return QVariant();
        return comment;
      }
      break;
    case CommentExplanationRole:
      {
        QString execution_name=executionName(rootItem,item_p,false);
        QString comment;
        if (! csmes_p->getExecutionComment(execution_name,comment) )
          return QVariant();
        if (comment.isEmpty())
          return QVariant();

        QString text="<body><html>";
        text+="<TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">";
        text+="<TR><TD>";
        text+="<I>";
        text+=QObject::tr("Execution");
        text+="&nbsp;";
        text+="</I>";
        text+="</TD><TD>";
        text+="<I>";
        text+=Qt::escape(execution_name);
        text+="</I>";
        text+="</TD></TR>";
        text+="<TR><TD>";
        text+="<I>";
        text+=QObject::tr("Comment");
        text+="&nbsp;";
        text+="</I>";
        text+="</TD><TD bgcolor=\""+COL_YELLOW_LIGHT.name()+"\">";
        text+=comment;
        text+="</TD></TR>";
        text+="</TABLE>";
        text+="</body></html>";
        return text;
      }
      break;
    case Qt::ToolTipRole:
      {
        if (!Options::get_opt_bool(QString(),"TOOLTIP_EXECUTION_LIST",DEF_TOOLTIP_EXECUTION_LIST))
          return QVariant();
        bool measurement=item_p->data(ITEM_MEASUREMENT).toBool();
        if (measurement)
        {
          QString name=executionName(rootItem,item_p,true);
          QString tooltip="<body><html>";
          tooltip+=tr("<U><I>Execution&nbsp;Name:</I></U>&nbsp;<TT>%1</TT>").arg(Qt::escape(name));
          bool ok;
          int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
          int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
          if (nb_tested+nb_untested>0)
            tooltip+=tr("<BR><U><I>Coverage:</I></U>&nbsp;%1").arg(Qt::escape(CSMesUndoRedoFramework::printStat(nb_tested,nb_untested)));
          tooltip+=tr("<BR><U><I>Status:</I></U>&nbsp;<TT>%1</TT>").arg(Qt::escape(csmes_p->getExecutionStatusStr(name)));
          tooltip+="</body></html>";

          return tooltip;
        }
        else
          return QVariant();
      }
    case Qt::StatusTipRole:
      return item_p->data(ITEM_NAME);
    case Qt::FontRole:
      {
         QFont f;
         switch (index.column())
         {
            case EXECUTION_LIST_COLUMN_STATISTIC:
               f.fromString(Options::get_opt_str(QString(),"FONT_SOURCE_CODE",DEF_FONT_SOURCE_CODE));
               break;
            default:
               f=QApplication::font();
               break;
         }
        return f;
      }
    case Qt::DisplayRole:
      switch (index.column())
      {
        case EXECUTION_LIST_COLUMN_STATISTIC:
          {
            bool ok;
            int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
            int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
            if (nb_tested+nb_untested>0)
              return CSMesUndoRedoFramework::printStat(nb_tested,nb_untested);
            return item_p->data(ITEM_NB_TESTED).toString();
          }
        case EXECUTION_LIST_COLUMN_COMMENT:
          if (item_p->data(ITEM_MEASUREMENT).toBool())
          {
            QString execution_name=executionName(rootItem,item_p,false);
            QString comment;
            if (! csmes_p->getExecutionComment(execution_name,comment) )
              return QVariant();
            if (comment.isEmpty())
              return tr("no");
            else
              return tr("yes");
          }
          else
            return QVariant();
        case EXECUTION_LIST_COLUMN_NAME:
          return item_p->data(ITEM_NAME);
        case EXECUTION_LIST_COLUMN_STATE:
          {
            bool first=true;
            QString execution_str;
            ExecutionNames executions;
            Executions::execution_status_t status=Executions::EXECUTION_STATUS_UNKNOWN;
            getAllExecutions(item_p,executions);
            for (ExecutionNames::const_iterator it=executions.begin(); it!=executions.end(); ++it)
            {
              if (first)
              {
                status= csmes_p->getExecutionStatus(*it);
                first=false;
              }
              else
              {
                if (status != csmes_p->getExecutionStatus(*it))
                  return QVariant();
              }
            }
            if (first)
              return QVariant();
            return CSMes::executionsStatusStr()[status];
          }
        default:
          return QVariant();
      }

  }
  return QVariant();
}


bool ExecutionListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid())
    return false;

  CSMesBackgroundComputations::Pauser statistic_pauser;
  TreeList *item_p = static_cast<TreeList*>(index.internalPointer());
  switch (role)
  {
    default:
      return false;

    case Qt::EditRole:
      switch (index.column())
      {
        default:
          return false;
        case EXECUTION_LIST_COLUMN_NAME:
          {
            QString name=executionName(rootItem,item_p,false);
            int pos=name.lastIndexOf("/");
            if (pos>0)
            {
              name=name.mid(0,pos);
              name+="/";
              name+=value.toString();
            }
            else
              name=value.toString();
            return renameIndex(index,name);
          }
          break;
        case EXECUTION_LIST_COLUMN_STATE:
          {
            csmes_p->beginUndoGroup(tr("Changing execution state of '%1' to '%2'").arg(executionName(rootItem,item_p,false)).arg(value.toString()));
            QString execution_str;
            ExecutionNames executions;
            getAllExecutions(item_p,executions);
            for (ExecutionNames::const_iterator it=executions.begin(); it!=executions.end(); ++it)
            {
              const ExecutionName &name=*it;
              if (!name.isEmpty())
              {
                csmes_p->setExecutionStatusStr(name,value.toString());
                emit dataChanged(index,index);
              }
            }
            csmes_p->endUndoGroup();
            return true;
          }
          break;
      }
      break;
    case Qt::CheckStateRole:
      {
        ExecutionNames selected_executions;
        ExecutionNames comparaison_items;
        if (CoverageSettings::object().getExecutionAnalysisMode())
        {
          selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
          comparaison_items=CoverageSettings::object().getSelectedExecutions();
        }
        else
        {
          selected_executions=CoverageSettings::object().getSelectedExecutions();
          comparaison_items=CoverageSettings::object().getSelectedExecutionsComparaison();
        }
        ExecutionNames new_selected_executions;
        getAllExecutions(item_p,new_selected_executions);
        for (ExecutionNames::const_iterator it=new_selected_executions.begin();
            it!=new_selected_executions.end();
            ++it)
        {
          bool ok;
          int v;
          switch ( (v=value.toInt(&ok)) )
          {
            case Qt::PartiallyChecked:
            case Qt::Checked:
              selected_executions+=*it;
              break;
            default:
              ASSERT(false);
            case Qt::Unchecked:
              selected_executions.removeAll(*it);
              break;
          }
          ASSERT(ok);;
          TreeList * i_p=executionTreeList(*it);
          if (i_p)
            i_p->data(ITEM_CHECKSTATE) =value;
        }
        if (csmes_p)
          selectComparaison(selected_executions,comparaison_items,
              CoverageSettings::object().getTestCoverageMode(),
              CoverageSettings::object().getCoverageMethod(),
              CoverageSettings::object().getExecutionAnalysisMode(),
              CoverageSettings::object().getReleaseComparaisonMode(),
              CoverageSettings::object().getCoverageLevel()
              );
        emit dataChanged(index,index);
        refreshView();
        return true;
      }
  }
  return false;
}

Qt::ItemFlags ExecutionListModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  TreeList *item_p;
  item_p = static_cast<TreeList*>(index.internalPointer());

  Qt::ItemFlags f=Qt::ItemIsEnabled ;

  if (CoverageSettings::object().getExecutionAnalysisMode())
  {
    if (index.column()==EXECUTION_LIST_COLUMN_NAME)
    {
      if (item_p->data(ITEM_MEASUREMENT).toBool())
      {
        f|=Qt::ItemIsTristate;
        f|=Qt::ItemIsSelectable ;
      }
      f|=Qt::ItemIsUserCheckable;
    }
  }
  else
  {
    if (index.column()==EXECUTION_LIST_COLUMN_NAME)
    {
      if (item_p->data(ITEM_MEASUREMENT).toBool())
      {
        f|=Qt::ItemIsTristate;
        f|=Qt::ItemIsEditable ;
      }
      f|=Qt::ItemIsSelectable ;
      f|=Qt::ItemIsUserCheckable;
    }
    else if (index.column()==EXECUTION_LIST_COLUMN_STATE)
    {
      f|=Qt::ItemIsEditable ;
    }
  }
  return f;
}

void ExecutionListModel::setupModelData()
{
  beginResetModel();
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (rootItem)
    delete rootItem;

  executionList.clear();
  if (csmes_p)
    executionList=csmes_p->executionList();

  ExecutionNames selected_executions;
  if (CoverageSettings::object().getExecutionAnalysisMode())
    selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
  else
    selected_executions=CoverageSettings::object().getSelectedExecutions();
  rootItem = generateRootItem(executionList,selected_executions);
  endResetModel();
}

TreeList *ExecutionListModel::generateRootItem(const ExecutionNames &execution_list,const ExecutionNames &selected_executions)
{
  QVector <QVariant> rootData(EXECUTION_LIST_COLUMN_MAX);
  rootData[EXECUTION_LIST_COLUMN_NAME]=tr("Executions") ;
  rootData[EXECUTION_LIST_COLUMN_STATISTIC]=tr("Coverage") ;
  rootData[EXECUTION_LIST_COLUMN_STATE]=tr("State");
  rootData[EXECUTION_LIST_COLUMN_COMMENT]=tr("Comment");
  TreeList *_rootItem = new TreeList(rootData);

  /* insert the new items */
  for ( ExecutionNames::const_iterator mes_it = execution_list.begin(); mes_it != execution_list.end(); ++mes_it )
    insertExecution(_rootItem,selected_executions,*mes_it);

  updateGroupItems(_rootItem,selected_executions, _rootItem);
  _rootItem->squeeze();
  return _rootItem;
}


TreeList *ExecutionListModel::findExecution(const TreeList *parent_p,const QString &name_no_scope)
{
  QList<int> index;
  QList<TreeList*> items;
  parent_p->find(name_no_scope,ITEM_NAME,1,index,items);
  if (items.isEmpty())
    return NULL;
  ASSERT(items.count()<=1);
  return items.first();
}


TreeList * ExecutionListModel::insertExecution (TreeList *_rootItem,const ExecutionNames &selected_executions,const ExecutionName &name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  TreeList *item_p=NULL;
  QStringList name_list = name.split('/');
  int count=name_list.size();
  int name_list_id=1;
  TreeList *parent_p=_rootItem;
  for ( QStringList::const_iterator it = name_list.begin(); it != name_list.end(); ++it )
  {
    if ( ! (item_p=findExecution(parent_p,*it)) )
    { /* create new entry */
      QVariantList d;

      /* insert the row in the model */
      QVector<QVariant> datas(ITEM_MAX);
      datas[ITEM_MEASUREMENT] = false;
      datas[ITEM_CHECKSTATE] = Qt::PartiallyChecked;
      datas[ITEM_NB_UNTESTED]=QVariant();
      datas[ITEM_NB_TESTED]=QVariant();
      datas[ITEM_NAME]=*it;
      item_p=new TreeList(datas,parent_p);
    }

    if (name_list_id==count)
    { /* Last Element */
      item_p->setData(ITEM_MEASUREMENT, true);
      bool selected=selected_executions.contains(name);
      if (selected)
        item_p->setData(ITEM_CHECKSTATE, Qt::Checked);
      else
        item_p->setData(ITEM_CHECKSTATE, Qt::Unchecked);
      item_p->setData(ITEM_MEASUREMENT, true);
      item_p->setData(ITEM_NB_TESTED,QVariant());
      item_p->setData(ITEM_NB_UNTESTED,QVariant());
    }
    parent_p=item_p;
    name_list_id++;
  }
  return item_p;
}

QList<TreeList *> ExecutionListModel::executionTreeLists(const ExecutionNames &names) const
{
  QList<TreeList *> result;
  int nb_names=names.count();

  TreeList* item_p=executionTreeList(names) ;
  if (item_p)
    result.append(item_p);

  for(;item_p && item_p!=rootItem;item_p=item_p->parent())
  {
    ExecutionNames exec_names;
    getAllExecutions(item_p,exec_names);

    if (exec_names.count()==nb_names)
      result.append(item_p);
    else
      return result;
  }

  return result;
}

TreeList * ExecutionListModel::executionTreeList(const ExecutionNames &names) const
{
  if (names.isEmpty())
    return NULL;

  ExecutionName testsName=names.at(0);
  if (names.count()==1)
    return executionTreeList(testsName);

  for (ExecutionNames::const_iterator it=names.begin();it!=names.end();++it)
  {
    QString str=*it;
    str=str.left(testsName.size()+1);
    while ( (testsName+'/')!=str )
    {
      int pos=testsName.lastIndexOf('/');
      if (pos<0)
        return NULL;
      testsName=testsName.mid(0,pos);
      str=str.left(testsName.size()+1);
    }
  }
  return executionTreeList(testsName);
}

TreeList * ExecutionListModel::executionTreeList(const ExecutionName &name) const
{
  QStringList name_list = name.split('/');
  int count=name_list.size();
  int name_list_id=1;
  TreeList *parent_p=rootItem;
  for ( QStringList::const_iterator it = name_list.begin(); it != name_list.end(); ++it )
  {
    TreeList *item_p;
    if ( ! (item_p=findExecution(parent_p,*it)) )
      return NULL;

    if (name_list_id==count)
    { /* Last Element */
      return item_p;
    }
    parent_p=item_p;
    name_list_id++;
  }
  return NULL;
}

QString ExecutionListModel::executionName(const TreeList *_rootItem,const TreeList *e,bool measurement_only)
{
  if (measurement_only && ! e->data(ITEM_MEASUREMENT).toBool())
    return QString::null;

  QString name;
  const TreeList *it_p=e;
  while (it_p!=_rootItem)
  {
    if (name.isEmpty())
      name=it_p->data(ITEM_NAME).toString();
    else
      name=it_p->data(ITEM_NAME).toString()+"/"+name;
    it_p = it_p->parent();
  }
  return name;
}

void ExecutionListModel::getAllExecutions(const TreeList *e,ExecutionNames &execs) const
{
  int cnt=e->childCount();
  for (int i=0;i<cnt;i++)
  {
    getAllExecutions(e->child(i),execs);
  }
  if (e->data(ITEM_MEASUREMENT).toBool())
  {
    execs+=executionName(rootItem,e,true);
  }
}


void ExecutionListModel::updateGroupItems(const TreeList *_rootItem,const ExecutionNames &selected_executions,TreeList *e)
{
  bool first_item=true;
  Qt::CheckState st=Qt::Unchecked;
  int cnt=e->childCount();
  for (int i=0;i<cnt;i++)
  {
    TreeList *child=e->child(i);
    updateGroupItems(_rootItem,selected_executions,child);
    if (first_item)
    {
      st=static_cast<Qt::CheckState>(child->data(ITEM_CHECKSTATE).toInt());
      first_item=false;
    }
    switch (st)
    {
      case Qt::Unchecked:
        switch (static_cast<Qt::CheckState>(child->data(ITEM_CHECKSTATE).toInt()))
        {
          case Qt::Unchecked:
            break;
          case Qt::Checked:
          case Qt::PartiallyChecked:
            st=Qt::PartiallyChecked;
            break;
        }
        break;
      case Qt::Checked:
        switch (static_cast<Qt::CheckState>(child->data(ITEM_CHECKSTATE).toInt()))
        {
          case Qt::Checked:
            break;
          case Qt::Unchecked:
          case Qt::PartiallyChecked:
            st=Qt::PartiallyChecked;
            break;
        }
        break;
      case Qt::PartiallyChecked:
        break;
    }
  }
  if ( e->data(ITEM_MEASUREMENT).toBool())
  { // Real execution
    QString name=executionName(_rootItem,e,true);
    if (selected_executions.contains(name))
      st=Qt::Checked;
    else
      st=Qt::Unchecked;
  }

  if (e!=_rootItem)
  {
    if (e->data(ITEM_CHECKSTATE)!=st)
      e->data(ITEM_CHECKSTATE)=st;
  }
}

bool ExecutionListModel::deleteIndex(const QModelIndex &index)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  TreeList *item_p=static_cast<TreeList*>(index.internalPointer());
  ExecutionNames execs;
  getAllExecutions(item_p,execs);
  if (!csmes_p->deleteExecution(execs))
    return false;
  emitRefreshView();
  return true;
}

bool ExecutionListModel::deleteExecution(const ExecutionNames &names)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  bool result=true;
  int sz=names.count();
  for (int i=0;i<sz;i++)
  {
    result = deleteExecution(names.at(i)) && result ;
  }
  return result;
}

bool ExecutionListModel::renameExecution(const ExecutionName &old_name,const ExecutionName &new_name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  TreeList *item_p=executionTreeList(old_name);
  if (!item_p)
    return false;
  QModelIndex index=createIndexTreeList(item_p,ITEM_NAME);
  return renameIndex(index,new_name);
}

bool ExecutionListModel::deleteExecution(const ExecutionName &name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  TreeList *item_p=executionTreeList(name);
  if (!item_p)
    return false;
  QModelIndex index=createIndexTreeList(item_p,ITEM_NAME);
  return deleteIndex(index);
}

bool ExecutionListModel::mergeExecutions(const ExecutionNames &executions,const ExecutionName &name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (!csmes_p->mergeExecutions(executions,name))
    return false;

  emitRefreshView();
  return true;
}

bool ExecutionListModel::setExecutionComment (const QModelIndex &index,const QString &comment)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (index.isValid())
  {
    TreeList *item_p=static_cast<TreeList*>(index.internalPointer());
    QString name=executionName(rootItem,item_p,false);
    return csmes_p->setExecutionComment(name,comment);
  }
  return false;
}

bool ExecutionListModel::renameIndex (const QModelIndex &index,const ExecutionName &new_name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (new_name == QString::null)
    return false;
  if (new_name == "")
    return false;
  if (new_name.left(1) == "/")
    return false;
  if (new_name.right(1) == "/")
    return false;

  if (index.isValid())
  {
    bool ok;
    TreeList *item_p=static_cast<TreeList*>(index.internalPointer());
    QString old_name=executionName(rootItem,item_p,false);
    if (csmes_p->executionExists(old_name))
      ok=csmes_p->renameExecution(old_name,new_name);
    else
      ok=csmes_p->renameExecutionPath(old_name,new_name);
    if (ok)
    {
      emitRefreshView();
      return true;
    }
  }
  return false;
}

void ExecutionListModel::invalidateStatisticsFromParents(TreeList *item_p)
{
  if (! item_p)
    return ;

  while(item_p!=rootItem)
  {
    int pos=item_p->row();
    QModelIndex item_index=createIndex(pos,EXECUTION_LIST_COLUMN_STATISTIC,item_p);
    QModelIndex parent_index;
    TreeList *parent_p=item_p->parent();
    if (parent_p!=rootItem)
      parent_index=createIndex(parent_p->row(),0,parent_p);

    if (!item_p->data(ITEM_MEASUREMENT).toBool() && item_p->childCount()==0)
    {
      beginRemoveRows(parent_index,pos,pos);
      parent_p->removeChild(pos);
      endRemoveRows();
    }
    else
    {
      item_p->data(ITEM_NB_UNTESTED)=QVariant();
      item_p->data(ITEM_NB_TESTED)=QVariant();
      emit dataChanged(item_index,item_index);
    }
    item_p=parent_p;
  }
}

QString ExecutionListModel::executionName(const QModelIndex &index,bool execution_only) const
{
  const TreeList *item_p=static_cast<TreeList*>(index.internalPointer());
  return executionName(rootItem,item_p,execution_only);
}

void ExecutionListModel::Select(const QString &pattern,bool filter_comment, bool filter_no_comment, bool filter_unknown, bool filter_passed, bool filter_failed, bool filter_check_manually)
{
  if (csmes_p==NULL) return;
  MatchExpr match(pattern);
  ExecutionNames selected_executions;
  ExecutionNames comparaison_items;
  if (CoverageSettings::object().getExecutionAnalysisMode())
  {
    selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
    comparaison_items=CoverageSettings::object().getSelectedExecutions();
  }
  else
  {
    selected_executions=CoverageSettings::object().getSelectedExecutions();
    comparaison_items=CoverageSettings::object().getSelectedExecutionsComparaison();
  }
  ExecutionNames selection_list=selected_executions;
  const ExecutionNames &list=csmes_p->executionList();
  for (ExecutionNames::const_iterator list_it=list.begin();list_it!=list.end();++list_it)
  {
    if (match.indexIn(*list_it)>=0)
    {
      if (!selection_list.contains(*list_it))
      {
        bool comment=false;
        bool no_comment=false;
        const QString &execution_name=*list_it;
        QString comment_string;
        if (csmes_p->getExecutionComment(execution_name,comment_string) )
        {
          if (comment_string.length()>0)
            comment=true;
          else
            no_comment=true;
        }

        Executions::execution_status_t status= csmes_p->getExecutionStatus(execution_name);
        bool unknown=  (Executions::EXECUTION_STATUS_UNKNOWN==status);
        bool passed=  (Executions::EXECUTION_STATUS_PASSED==status);
        bool failed=  (Executions::EXECUTION_STATUS_FAILED==status);
        bool check_manually=  (Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY==status);

        bool filter_state=true;
        if ((unknown) && filter_unknown) filter_state=false;
        if ((passed) && filter_passed) filter_state=false;
        if ((failed) && filter_failed) filter_state=false;
        if ((check_manually) && filter_check_manually) filter_state=false;

        if (filter_state)
          continue;

        bool filter_com=true;
        if ((comment) && filter_comment) filter_com=false;
        if ((no_comment) && filter_no_comment) filter_com=false;

        if (filter_com)
          continue;
        selection_list.push_back(*list_it);
        TreeList * i_p=executionTreeList(*list_it);
        if (i_p)
          i_p->data(ITEM_CHECKSTATE) =Qt::Checked;
      }
    }
  }
  if (csmes_p)
    selectComparaison(selection_list,comparaison_items,
        CoverageSettings::object().getTestCoverageMode(),
        CoverageSettings::object().getCoverageMethod(),
        CoverageSettings::object().getExecutionAnalysisMode(),
        CoverageSettings::object().getReleaseComparaisonMode(),
        CoverageSettings::object().getCoverageLevel()
        );
}

void ExecutionListModel::DeSelect(const QString &pattern,bool filter_comment, bool filter_no_comment, bool filter_unknown, bool filter_passed, bool filter_failed, bool filter_check_manually)
{
  if (csmes_p==NULL) return;
  MatchExpr match(pattern);
  ExecutionNames selected_executions;
  ExecutionNames comparaison_items;
  if (CoverageSettings::object().getExecutionAnalysisMode())
  {
    selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
    comparaison_items=CoverageSettings::object().getSelectedExecutions();
  }
  else
  {
    selected_executions=CoverageSettings::object().getSelectedExecutions();
    comparaison_items=CoverageSettings::object().getSelectedExecutionsComparaison();
  }
  ExecutionNames selection_list=selected_executions;
  for (ExecutionNames::const_iterator list_it=selection_list.begin();list_it!=selection_list.end();++list_it)
  {
    if (match.indexIn(*list_it)>=0)
    {
      if (selection_list.contains(*list_it))
      {
        bool comment=false;
        bool no_comment=false;
        const QString &execution_name=*list_it;
        QString comment_string;
        if (csmes_p->getExecutionComment(execution_name,comment_string) )
        {
          if (comment_string.length()>0)
            comment=true;
          else
            no_comment=true;
        }

        Executions::execution_status_t status= csmes_p->getExecutionStatus(execution_name);
        bool unknown=  (Executions::EXECUTION_STATUS_UNKNOWN==status);
        bool passed=  (Executions::EXECUTION_STATUS_PASSED==status);
        bool failed=  (Executions::EXECUTION_STATUS_FAILED==status);
        bool check_manually=  (Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY==status);

        bool filter_state=true;
        if ((unknown) && filter_unknown) filter_state=false;
        if ((passed) && filter_passed) filter_state=false;
        if ((failed) && filter_failed) filter_state=false;
        if ((check_manually) && filter_check_manually) filter_state=false;

        if (filter_state)
          continue;

        bool filter_com=true;
        if ((comment) && filter_comment) filter_com=false;
        if ((no_comment) && filter_no_comment) filter_com=false;

        if (filter_com)
          continue;
        selection_list.removeAll(*list_it);
        TreeList * i_p=executionTreeList(*list_it);
        if (i_p)
          i_p->data(ITEM_CHECKSTATE) =Qt::Unchecked;
      }
    }
  }
  if (csmes_p)
    selectComparaison(selection_list,comparaison_items,
        CoverageSettings::object().getTestCoverageMode(),
        CoverageSettings::object().getCoverageMethod(),
        CoverageSettings::object().getExecutionAnalysisMode(),
        CoverageSettings::object().getReleaseComparaisonMode(),
        CoverageSettings::object().getCoverageLevel()
        );
}

void ExecutionListModel::Comparaison( const QModelIndexList & indexes)
{
  QModelIndex index;
  ExecutionNames selected_executions;
  if (CoverageSettings::object().getExecutionAnalysisMode())
    selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
  else
    selected_executions=CoverageSettings::object().getSelectedExecutions();
  ExecutionNames selection_list=selected_executions;
  if (selection_list.isEmpty())
    return;
  ExecutionNames comparaison_items;

  foreach(index, indexes)
  {
    QString text=executionName(index,true);
    comparaison_items+=text;
  }
  if (CoverageSettings::object().getExecutionAnalysisMode() && csmes_p)
    selectComparaison(selection_list,comparaison_items,
        CoverageSettings::object().getTestCoverageMode(),
        CoverageSettings::object().getCoverageMethod(),
        CoverageSettings::object().getExecutionAnalysisMode(),
        CoverageSettings::object().getReleaseComparaisonMode(),
        CoverageSettings::object().getCoverageLevel()
        );
}


void ExecutionListModel::selectComparaison( const ExecutionNames& selection_list, const ExecutionNames &cmp_items,bool test_coverage_mode,Instrumentation::coverage_method_t method, bool execution_comparaison_mode, CSMes::comparaison_mode_t csmes_comparaison_mode,int coverage_level)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_p==NULL)
    return;

  emit layoutAboutToBeChanged();

  ExecutionNames executions=csmes_p->executionList();
  ExecutionNames selected_executions;
  ExecutionNames comparaison_items;
  for ( ExecutionNames::const_iterator it = selection_list.begin(); it != selection_list.end(); ++it )
  {
    if ( executions.contains(*it) && (!selected_executions.contains(*it)))
      selected_executions += *it;
  }
  for ( ExecutionNames::const_iterator _it = cmp_items.begin(); _it != cmp_items.end(); ++_it )
  {
    if ( executions.contains(*_it) && (!comparaison_items.contains(*_it)))
      comparaison_items += *_it;
  }

  if (execution_comparaison_mode)
  {
    if (selected_executions.isEmpty())
      csmes_p->selectExecutionsComparaison(ExecutionNames(),ExecutionNames(),test_coverage_mode,coverage_level,method,csmes_comparaison_mode,execution_comparaison_mode);
    else
      csmes_p->selectExecutionsComparaison(comparaison_items,selected_executions,test_coverage_mode,coverage_level,method,csmes_comparaison_mode,execution_comparaison_mode);
  }
  else
    csmes_p->selectExecutionsComparaison(selected_executions,ExecutionNames(),test_coverage_mode,coverage_level,method,csmes_comparaison_mode,execution_comparaison_mode);

  emit layoutChanged();
}

void ExecutionListModel::refresh()
{
  setupModelData();
  CoverageSettings::object().backup(last_settings);
}

void ExecutionListModel::refreshView()
{
  bool modified=false;
  executionList.clear();
  if (csmes_p)
    executionList=csmes_p->executionList();

  if (updateModelData())
    modified=true;

  if (   CoverageSettings::object().isReleaseComparaisonModeModified(last_settings)
      || CoverageSettings::object().isExecutionAnalysisModeModified(last_settings)
      || CoverageSettings::object().isCoverageLevelModified(last_settings)
      || CoverageSettings::object().isExecutionAnalysisModeModified(last_settings)
      || CoverageSettings::object().isSelectedExecutionsModified(last_settings)
      || CoverageSettings::object().isCoverageMethodModified(last_settings) )
  {
    recalculateStatistics();
    modified=true;
  }
  CoverageSettings::object().backup(last_settings);
}

void ExecutionListModel::recalculateStatistics()
{
  stopBackgroundCalculation();
  rootItem->setDataRecursive(ITEM_NB_UNTESTED,QVariant(),true);
  rootItem->setDataRecursive(ITEM_NB_TESTED,QVariant(),true);
  recalculateStatistics(QModelIndex());
}

void ExecutionListModel::recalculateStatistics(QModelIndex parent)
{
  int rows=rowCount(parent);
  if (rows==0)
    return;

  for (int r=0;r<rows;r++)
    recalculateStatistics(index(r,EXECUTION_LIST_COLUMN_STATISTIC,parent));

  QModelIndex firstIndex= index(0, EXECUTION_LIST_COLUMN_STATISTIC, parent);
  QModelIndex lastIndex= index(rows-1, EXECUTION_LIST_COLUMN_STATISTIC, parent);
  emit dataChanged(firstIndex,lastIndex);
}

bool ExecutionListModel::updateModelData()
{
  bool modified=false;
  ExecutionNames selected_executions;
  if (CoverageSettings::object().getExecutionAnalysisMode())
    selected_executions=CoverageSettings::object().getSelectedExecutionsComparaison();
  else
    selected_executions=CoverageSettings::object().getSelectedExecutions();
  TreeList *newRootItem = generateRootItem(executionList,selected_executions);

  if (updateModelDataRemoveObselete(newRootItem,rootItem))
    modified=true;
  if (updateModelDataInsertItems(newRootItem,rootItem))
    modified=true;
  delete newRootItem;
  updateModelDataExecutionStatus(rootItem);
  updateGroupItems(rootItem,selected_executions, rootItem);
  return modified;
}

void ExecutionListModel::updateModelDataExecutionStatus(TreeList *item_p)
{
  int item_rows=item_p->childCount();
  for ( int item_row=0;item_row<item_rows;++item_row)
  {
    updateModelDataExecutionStatus(item_p->child(item_row));
  }
  if (item_p!=rootItem)
  {
    QModelIndex index=createIndexTreeList(item_p,EXECUTION_LIST_COLUMN_STATE);
    emit dataChanged(index,index);
  }
}

bool ExecutionListModel::updateModelDataRemoveObselete(const TreeList *item_reference_p,TreeList *item_p)
{
  bool modified=false;
  int item_reference_rows=item_reference_p->childCount();
  int item_reference_row=0;
  int item_row;
  for ( item_row=0;item_row<item_p->childCount();++item_row)
  {
    int actual_reference_row=item_reference_row;
    bool found=false;
    int item_reference_max=item_reference_rows;
    for (int pass=0;pass<2;pass++)
    {
      for (;item_reference_row<item_reference_max;++item_reference_row)
      {
        if (item_reference_p->child(item_reference_row)->data(ITEM_NAME)
            ==
            item_p->child(item_row)->data(ITEM_NAME))
        {
          found=true;
          break;
        }
      }
      if (found)
        break;

      item_reference_max=actual_reference_row;
      item_reference_row=0;
    }
    if (!found)
    {
      QModelIndex index;
      if (item_p!=rootItem)
        index=createIndex(item_p->row(),0,item_p);
      beginRemoveRows(index,item_row,item_row);
      item_p->removeChild(item_row);
      endRemoveRows();
      item_row--;
      modified=true;
    }
    else
    { // process subchild
      bool modif = updateModelDataRemoveObselete(item_reference_p->child(item_reference_row),item_p->child(item_row));
      modified= modified || modif;
    }
  }
  if (modified && (item_p!=rootItem))
  {
    item_p->setData(ITEM_NB_UNTESTED,QVariant());
    item_p->setData(ITEM_NB_TESTED,QVariant());
    rowChanged(item_p);
  }
  return modified;
}

bool ExecutionListModel::updateModelDataInsertItems(const TreeList *item_reference_p, TreeList *item_p)
{
  bool modified=false;
  int item_reference_rows=item_reference_p->childCount();
  int item_reference_row;
  int item_row=0;
  for ( item_reference_row=0;item_reference_row<item_reference_rows;++item_reference_row)
  {
    int actual_row=item_row;
    bool found=false;
    int item_max=item_p->childCount();
    for (int pass=0;pass<2;pass++)
    {
      for (;item_row<item_max;++item_row)
      {
        if (item_reference_p->child(item_reference_row)->data(ITEM_NAME)
            ==
            item_p->child(item_row)->data(ITEM_NAME))
        {
          found=true;
          break;
        }
      }
      if (found)
        break;

      item_max=actual_row;
      item_row=0;
    }
    if (!found)
    {
      QModelIndex index;
      if (item_p!=rootItem)
        index=createIndex(item_p->row(),0,item_p);
      beginInsertRows(index,item_p->childCount(),item_p->childCount());
      item_p->appendDuplicateChild(item_reference_p->child(item_reference_row));
      endInsertRows();
      modified=true;
    }
    else
    { // process subchild
      bool modif = updateModelDataInsertItems(item_reference_p->child(item_reference_row),item_p->child(item_row));
      modified = modif || modified;
    }
  }
  if (modified && (item_p!=rootItem))
  {
    item_p->setData(ITEM_NB_UNTESTED,QVariant());
    item_p->setData(ITEM_NB_TESTED,QVariant());
    rowChanged(item_p);
  }
  return modified;
}

void ExecutionListModel::setFilter(bool comments, bool no_comments, bool unknown, bool passed, bool failed, bool check_manually)
{
  filter_comment=comments;
  filter_no_comment=no_comments;
  filter_unknown=unknown;
  filter_passed=passed;
  filter_failed=failed;
  filter_check_manually=check_manually;
}

void ExecutionListModel::emitRefreshView() const
{
  CoverageSettings::object().notifyModification();
}

