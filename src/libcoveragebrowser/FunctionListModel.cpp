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
#include "FunctionListModel.h"
#include "MatchExpr.h"
#include <QStringList>
#include <QFileInfo>
#include <QMultiHash>
#include <QHash>
#include "options.h"
#include "CoverageSettings.h"

enum FUNCTION_LIST_ITEMS
{
  ITEM_NAME,
  ITEM_FUNCTION,
  ITEM_NB_TESTED,
  ITEM_NB_UNTESTED,
  ITEM_DESCRIPTION,
  ITEM_FILE_NAME,
  ITEM_START_LINE,
  ITEM_START_COLUMN,
  ITEM_END_LINE,
  ITEM_END_COLUMN,
  ITEM_DIFFERENCE,
  ITEM_FILE_NAME_REF,
  ITEM_START_LINE_REF,
  ITEM_START_COLUMN_REF,
  ITEM_END_LINE_REF,
  ITEM_END_COLUMN_REF,
  ITEM_START_LINE_ORIG,
  ITEM_END_LINE_ORIG,
  ITEM_START_LINE_REF_ORIG,
  ITEM_END_LINE_REF_ORIG,
  ITEM_MAX
};



FunctionListModel::FunctionListModel(QObject *parent)
    : TreeListModel(parent,FUNCTION_LIST_MAX_COLUMN)
{
  filter_different_indentation=true;
  filter_different=true;
  filter_different_comment=true;
  filter_not_existing=true;
  filter_new=true;
  filter_same=true;
  csmes_p=NULL;

  setStatusColorLevelFunction(-1.0,-1.0);
  setStatusColorLevelClass(-1.0,-1.0);
}

FunctionListModel::~FunctionListModel()
{
}

void FunctionListModel::setCSMes(CSMesUndoRedoFramework *c)
{
  csmes_p=c;
  setupModelData();
}

void FunctionListModel::generateStatistics(TreeList *item_p)const
{
  int nb_tested,nb_untested;
  if (item_p->data(ITEM_FUNCTION).toBool())
  {
    long start_line=item_p->data(ITEM_START_LINE).toInt();
    long start_column=item_p->data(ITEM_START_COLUMN).toInt();
    long end_line=item_p->data(ITEM_END_LINE).toInt();
    long end_column=item_p->data(ITEM_END_COLUMN).toInt();
    if (csmes_p->statisticFunctionPre(
          QString(),
          item_p->data(ITEM_FILE_NAME).toString(),
          start_line,
          start_column,
          end_line,
          end_column,
          CoverageSettings::object().getCoverageLevel(),
          CoverageSettings::object().getCoverageMethod(),
          nb_tested,
          nb_untested))
    {
      item_p->data(ITEM_NB_TESTED)= static_cast<int>(nb_tested);
      item_p->data(ITEM_NB_UNTESTED)= static_cast<int>(nb_untested);
    }
  }
  else
  {
    int nb_tested=0;
    int nb_untested=0;
    TreeList *child_p;
    for (int index=0;(child_p=item_p->child(index));index++)
    {
      generateStatistics(child_p);
      nb_tested+=child_p->data(ITEM_NB_TESTED).toInt();
      nb_untested+=child_p->data(ITEM_NB_UNTESTED).toInt();
    }
    item_p->data(ITEM_NB_TESTED)= static_cast<int>(nb_tested);
    item_p->data(ITEM_NB_UNTESTED)= static_cast<int>(nb_untested);
  }
}

QVariant FunctionListModel::data (const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  TreeList *item_p = static_cast<TreeList*>(index.internalPointer());
  {
    bool ok;
    item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
    if (!ok)
    {
      generateStatistics(item_p);
    }
  }
  switch (role)
  {
    default:
      return QVariant();

    case FilterRole:
      {
        bool ok=false;
        CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
        int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
        if (ok)
          modif=static_cast<CSMes::modifications_t >(diff);
        else
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesFunction(item_p)));
        switch (modif)
        {
          case CSMes::DIFFERENT:
            if (!filter_different)
              return true;
            break;
          case CSMes::NOT_EXISTING:
            if (!filter_not_existing)
              return true;
            break;
          case CSMes::NEW:
            if (!filter_new)
              return true;
            break;
          case CSMes::SAME_EXCEPT_WHITE_SPACES:
            if (!filter_different_indentation)
              return true;
            break;
          case CSMes::SAME_EXCEPT_COMMENT:
            if (!filter_different_comment)
              return true;
            break;
          case CSMes::SAME:
            if (!filter_same)
              return true;
            break;
          case CSMes::NOTHING_TO_COMPARE:
          case CSMes::MODIFICATIONS_UNDEFINED:
            break;
        }
      }
      return false;
    case SearchTextRole:
      return functionName(item_p,true);
    case Qt::BackgroundColorRole:
      {
        if (CoverageSettings::object().getExecutionAnalysisMode())
          return QVariant();
        double medium,low;
        if (item_p->data(ITEM_FUNCTION).toBool())
        {
          medium=function_medium_level;
          low=function_low_level;
        }
        else
        {
          medium=class_medium_level;
          low=class_low_level;
        }
        bool ok;

        int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
        if (!ok) return QVariant();
        int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
        if (!ok) return QVariant();
        if (nb_tested>=0 && nb_untested>=0 && nb_tested+nb_untested>0)
        {
          double stat=static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested)*100.0;
          if (stat<low)
            return COL_RED;
          if (stat<medium)
            return COL_ORANGE;
          return COL_GREEN;
        }
        return QVariant();
      }
    case Qt::ToolTipRole:
      {
        if (!Options::get_opt_bool(QString(),"TOOLTIP_METHOD_LIST",DEF_TOOLTIP_METHOD_LIST))
          return QVariant();

        QString tooltip="<body><html>";
        tooltip+=tr("<u><I>Name:</I></U>&nbsp;<TT>%1</TT>").arg(Qt::escape(item_p->data(ITEM_NAME).toString()));
        if (item_p->data(ITEM_DESCRIPTION).toString()!=QString())
          tooltip+=tr("<BR><u><I>Prototype:</I></U>&nbsp;<TT>%1</TT>").arg(Qt::escape(item_p->data(ITEM_DESCRIPTION).toString()));
        bool ok;
        int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
        if (!ok) return QVariant();
        int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
        if (!ok) return QVariant();
        if (nb_tested+nb_untested>0)
          tooltip+=tr("<BR><U><I>Coverage:</I></U>&nbsp;%1").arg(Qt::escape(CSMesUndoRedoFramework::printStat(nb_tested,nb_untested)));
        if (item_p->data(ITEM_FILE_NAME).toString()!=QString())
        {
          tooltip+=tr("<BR><u><I>Relative&nbsp;File&nbsp;Name:</I></u>&nbsp;<TT>%1</TT>").arg(Qt::escape(csmes_p->relativeSourceName(item_p->data(ITEM_FILE_NAME).toString())));
          tooltip+=tr("<BR><u><I>Absolute&nbsp;File&nbsp;Name:</u>&nbsp;<TT>%1</TT>").arg(Qt::escape(item_p->data(ITEM_FILE_NAME).toString()));
          tooltip+=tr("<BR><u><I>Lines:</u>&nbsp;<TT>%1-%2</TT>")
            .arg(Qt::escape(item_p->data(ITEM_START_LINE).toString()))
            .arg(Qt::escape(item_p->data(ITEM_END_LINE).toString()));
        }
        if (item_p->data(ITEM_FILE_NAME_REF).toString()!=QString())
        {
          tooltip+=tr("<BR><u><I>Relative&nbsp;File&nbsp;Name&nbsp;(reference):</I></u>&nbsp;<TT>%1</TT>").arg(Qt::escape(csmes_p->relativeSourceName(item_p->data(ITEM_FILE_NAME_REF).toString())));
          tooltip+=tr("<BR><u><I>Absolute&nbsp;File&nbsp;Name&nbsp;(reference):</u>&nbsp;<TT>%1</TT>").arg(Qt::escape(item_p->data(ITEM_FILE_NAME_REF).toString()));
          tooltip+=tr("<BR><u><I>Lines&nbsp;(reference):</u>&nbsp;<TT>%1-%2</TT>")
          .arg(Qt::escape(item_p->data(ITEM_START_LINE_REF).toString()))
          .arg(Qt::escape(item_p->data(ITEM_END_LINE_REF).toString()));
        }
        CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
        int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
        if (ok)
          modif=static_cast<CSMes::modifications_t >(diff);
        else
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesFunction(item_p)));
        if (modif!=CSMes::MODIFICATIONS_UNDEFINED && modif!=CSMes::NOTHING_TO_COMPARE)
          tooltip+=tr("<BR><u><I>Comparaison:</u>&nbsp;<TT>%1</TT>").arg(
              Qt::escape(CSMes::modificationsToString(modif)));
        tooltip+="</body></html>";

        return tooltip;
      }
    case Qt::FontRole:
      {
        QFont f;
         switch (index.column())
         {
            case FUNCTION_LIST_COLUMN_STATISTIC:
               f.fromString(Options::get_opt_str(QString(),"FONT_SOURCE_CODE",DEF_FONT_SOURCE_CODE));
               break;
            default:
               f=QApplication::font();
               break;
         }
        bool ok=false;
        CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
        int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
        if (ok)
          modif=static_cast<CSMes::modifications_t >(diff);
        else
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesFunction(item_p)));
        switch (modif)
        {
          case CSMes::DIFFERENT:
            f.setBold(true);
            break;
          case CSMes::NOT_EXISTING:
            f.setBold(true);
            f.setStrikeOut(true);
            break;
          case CSMes::NEW:
            f.setBold(true);
            f.setUnderline(true);
            break;
          case CSMes::SAME_EXCEPT_WHITE_SPACES:
          case CSMes::SAME_EXCEPT_COMMENT:
          case CSMes::SAME:
          case CSMes::NOTHING_TO_COMPARE:
          case CSMes::MODIFICATIONS_UNDEFINED:
            break;
        }
        return f;
      }
    case Qt::StatusTipRole:
      return item_p->data(ITEM_DESCRIPTION);
    case Qt::DisplayRole:
      switch (index.column())
      {
        case FUNCTION_LIST_COLUMN_STATISTIC:
          {
            bool ok;
            int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
            if (!ok) return QVariant();
            int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
            if (!ok) return QVariant();
            return CSMesUndoRedoFramework::printStat(nb_tested,nb_untested);
          }
        case FUNCTION_LIST_COLUMN_NAME:
          return item_p->data(ITEM_NAME);
        case FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME:
          return item_p->data(ITEM_FILE_NAME);
        case FUNCTION_LIST_COLUMN_FILE_NAME:
          {
            QFileInfo fileInfo(item_p->data(ITEM_FILE_NAME).toString());
            return fileInfo.fileName();
          }
        case FUNCTION_LIST_COLUMN_POSITION:
          {
            int start_line, end_line;
            if (CoverageSettings::object().getSourceType()==CSMesUndoRedoFramework::CSMES)
            {
              start_line=item_p->data(ITEM_START_LINE).toInt();
              end_line=item_p->data(ITEM_END_LINE).toInt();
            }
            else
            {
              start_line=item_p->data(ITEM_START_LINE_ORIG).toInt();
              end_line=item_p->data(ITEM_END_LINE_ORIG).toInt();
            }
            if (start_line<0 || end_line<0)
              return QVariant();

            QString position;
            if (start_line==end_line)
              position=QString::number(start_line);
            else
              position=QString::number(start_line)+"-"+QString::number(end_line);

            return position;
          }
        case FUNCTION_LIST_COLUMN_DESCRIPTION:
          return item_p->data(ITEM_DESCRIPTION);
        case FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME_REF:
          return item_p->data(ITEM_FILE_NAME_REF);
        case FUNCTION_LIST_COLUMN_FILE_NAME_REF:
          {
            QFileInfo fileInfo(item_p->data(ITEM_FILE_NAME_REF).toString());
            return fileInfo.fileName();
          }
        case FUNCTION_LIST_COLUMN_POSITION_REF:
          {
            int start_line=item_p->data(ITEM_START_LINE_REF).toInt();
            int end_line=item_p->data(ITEM_END_LINE_REF).toInt();
            if (start_line<0 || end_line<0)
              return QVariant();

            QString position;
            if (start_line==end_line)
              position=QString::number(start_line);
            else
              position=QString::number(start_line)+"-"+QString::number(end_line);

            return position;
          }
        case FUNCTION_LIST_COLUMN_DIFFERENCE:
          {
            bool ok;
            CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
            int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
            if (ok)
              modif=static_cast<CSMes::modifications_t >(diff);
            else
              item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesFunction(item_p)));
            return CSMes::modificationsToString(modif);
          }
        default:
          return QVariant();
      }

  }
  return QVariant();
}

Qt::ItemFlags FunctionListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    TreeList *item_p = static_cast<TreeList*>(index.internalPointer());
    Qt::ItemFlags f=Qt::ItemIsEnabled ;
    if (item_p->data(ITEM_FUNCTION).toBool())
      f|=Qt::ItemIsSelectable ;

    return f;
}

void FunctionListModel::setupModelData()
{
 beginResetModel();
  if (rootItem)
    delete rootItem;

  QVector<QVariant> rootData(FUNCTION_LIST_MAX_COLUMN);
  rootData[FUNCTION_LIST_COLUMN_NAME]=tr("Method") ;
  rootData[FUNCTION_LIST_COLUMN_DESCRIPTION]=tr("Prototype") ;
  rootData[FUNCTION_LIST_COLUMN_STATISTIC]=tr("Coverage") ;
  rootData[FUNCTION_LIST_COLUMN_FILE_NAME]=tr("File") ;
  rootData[FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME]=tr("Absolute Path") ;
  rootData[FUNCTION_LIST_COLUMN_POSITION]=tr("Position");
  rootData[FUNCTION_LIST_COLUMN_FILE_NAME_REF]=tr("File (Reference)") ;
  rootData[FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME_REF]=tr("Absolute Path (Reference)") ;
  rootData[FUNCTION_LIST_COLUMN_POSITION_REF]=tr("Position (Reference)");
  rootData[FUNCTION_LIST_COLUMN_DIFFERENCE]=tr("Modifications");
  rootItem = new TreeList(rootData);

  if (csmes_p)
  {
    const SourceFiles sources_all= csmes_p->Sources(CSMes::NON_EMPTY) + csmes_p->SourcesReference(CSMes::NON_EMPTY) ;
    SourceFiles sources;

    for (SourceFiles::const_iterator itsrc=sources_all.begin();itsrc!=sources_all.end();++itsrc)
    {
      if (sources.contains(*itsrc))
        continue;

      sources.append(*itsrc);
      QVector<FunctionInfo> FunctionsInfo = csmes_p->FunctionInfoSource(QString(),*itsrc);
      QVector<FunctionInfo>::const_iterator itfct;
      for (itfct=FunctionsInfo.begin();itfct!=FunctionsInfo.end();++itfct)
      {
        insertFunction(
            (*itfct).getScopedName(),
            (*itfct).getPrototype(),
            *itsrc,
            (*itfct).startLineOrg() ,
            (*itfct).endLineOrg() ,
            (*itfct).startLinePre() ,
            (*itfct).startColumnPre() ,
            (*itfct).endLinePre() ,
            (*itfct).endColumnPre(),
            false
            );
      }

      QVector<FunctionInfo> FunctionsInfoReference = csmes_p->FunctionInfoSourceReference(QString(),*itsrc);
      for (itfct=FunctionsInfoReference.begin();itfct!=FunctionsInfoReference.end();++itfct)
      {
        insertFunction(
            (*itfct).getScopedName(),
            (*itfct).getPrototype(),
            *itsrc,
            (*itfct).startLineOrg() ,
            (*itfct).endLineOrg() ,
            (*itfct).startLinePre() ,
            (*itfct).startColumnPre() ,
            (*itfct).endLinePre() ,
            (*itfct).endColumnPre(),
            true
            );
      }
    }

  }
  rootItem->squeeze();
  endResetModel();
}

TreeList *FunctionListModel::findFunction(const TreeList *parent_p,const QString &name_no_scope,const QString &prototype) const
{
  QList<int> index;
  QList<TreeList*> items;
  parent_p->find(name_no_scope,ITEM_NAME,1,index,items);
  for (QList<TreeList*>::const_iterator item=items.begin();item!=items.end();++item)
  {
    QString _prototype=(*item)->data(ITEM_DESCRIPTION).toString() ;
    if ( _prototype == prototype )
      return *item;
  }
  return NULL;
}


TreeList * FunctionListModel::insertFunction (const QString &name,const QString &prototype,const QString &filename,long start_line_org,long end_line_org,long start_line_pre,long start_column_pre,long end_line_pre, long end_column_pre,bool reference)
{
  TreeList *item_p=NULL;
  QStringList name_list = name.split("::");
  int count=name_list.size();
  int name_list_id=1;
  TreeList *parent_p=rootItem;
  for ( QStringList::const_iterator it = name_list.begin(); it != name_list.end(); ++it )
  {
    if (name_list_id==count)
    { /* Last Element */
      if ( ! (item_p=findFunction(parent_p,*it,prototype) ) )
      { /* create new entry */
        QVector <QVariant> datas(ITEM_MAX);
        datas[ITEM_FILE_NAME_REF] = QString();
        datas[ITEM_START_LINE_REF] = -1;
        datas[ITEM_START_COLUMN_REF] = -1;
        datas[ITEM_END_LINE_REF] = -1;
        datas[ITEM_END_COLUMN_REF] = -1;
        datas[ITEM_FILE_NAME] = QString();
        datas[ITEM_START_LINE] = -1;
        datas[ITEM_START_COLUMN] = -1;
        datas[ITEM_END_LINE] = -1;
        datas[ITEM_END_COLUMN] = -1;
        datas[ITEM_FUNCTION] = false;
        datas[ITEM_DESCRIPTION] = QString();
        datas[ITEM_NB_TESTED]= QVariant();
        datas[ITEM_NB_UNTESTED]= QVariant();
        datas[ITEM_NAME]=*it;
        item_p=new TreeList(datas,parent_p);

        /* insert the row in the model */
        QModelIndex parent_index;
        if (item_p->parent()!=rootItem)
          parent_index=createIndex(item_p->parent()->row(),0,item_p->parent());
      }

      item_p->setData(ITEM_FUNCTION, true);
      item_p->setData(ITEM_DESCRIPTION, prototype);
      item_p->setData(ITEM_NB_TESTED,QVariant());
      item_p->setData(ITEM_NB_UNTESTED,QVariant());
      if (reference)
      {
        item_p->setData(ITEM_FILE_NAME_REF, filename);
        item_p->setData(ITEM_START_LINE_REF_ORIG, static_cast<int>(start_line_org));
        item_p->setData(ITEM_END_LINE_REF_ORIG, static_cast<int>(end_line_org));
        item_p->setData(ITEM_START_LINE_REF, static_cast<int>(start_line_pre));
        item_p->setData(ITEM_START_COLUMN_REF, static_cast<int>(start_column_pre));
        item_p->setData(ITEM_END_LINE_REF, static_cast<int>(end_line_pre));
        item_p->setData(ITEM_END_COLUMN_REF, static_cast<int>(end_column_pre));
      }
      else
      {
        item_p->setData(ITEM_FILE_NAME, filename);
        item_p->setData(ITEM_START_LINE_ORIG, static_cast<int>(start_line_org));
        item_p->setData(ITEM_END_LINE_ORIG, static_cast<int>(end_line_org));
        item_p->setData(ITEM_START_LINE, static_cast<int>(start_line_pre));
        item_p->setData(ITEM_START_COLUMN, static_cast<int>(start_column_pre));
        item_p->setData(ITEM_END_LINE, static_cast<int>(end_line_pre));
        item_p->setData(ITEM_END_COLUMN, static_cast<int>(end_column_pre));
      }
    }
    else
    {
      if ( ! (item_p=findFunction(parent_p,*it,QString()) ) )
      { /* create new entry */
        QVector <QVariant> datas(ITEM_MAX);
        datas[ITEM_FILE_NAME_REF] = QString();
        datas[ITEM_START_LINE_REF] = -1;
        datas[ITEM_START_LINE_REF_ORIG] = -1;
        datas[ITEM_END_LINE_REF_ORIG] = -1;
        datas[ITEM_START_LINE_ORIG] = -1;
        datas[ITEM_END_LINE_ORIG] = -1;
        datas[ITEM_START_COLUMN_REF] = -1;
        datas[ITEM_END_LINE_REF] = -1;
        datas[ITEM_END_COLUMN_REF] = -1;
        datas[ITEM_FILE_NAME] = QString();
        datas[ITEM_START_LINE] = -1;
        datas[ITEM_START_COLUMN] = -1;
        datas[ITEM_END_LINE] = -1;
        datas[ITEM_END_COLUMN] = -1;
        datas[ITEM_FUNCTION] = false;
        datas[ITEM_DESCRIPTION] = QString();
        datas[ITEM_NB_TESTED]= QVariant();
        datas[ITEM_NB_UNTESTED]= QVariant();
        datas[ITEM_NAME]=*it;
        item_p=new TreeList(datas,parent_p);

        /* insert the row in the model */
        QModelIndex parent_index;
        if (item_p->parent()!=rootItem)
          parent_index=createIndex(item_p->parent()->row(),0,item_p->parent());
      }

    }
    parent_p=item_p;
    name_list_id++;
  }
  return item_p;
}

QString FunctionListModel::functionName(const TreeList *e,bool measurement_only) const
{
  if (measurement_only && ! e->data(ITEM_FUNCTION).toBool())
    return QString::null;

  QString name=e->data(ITEM_NAME).toString();
  const TreeList *it_p=e->parent();
  while (it_p!=rootItem)
  {
    name=it_p->data(ITEM_NAME).toString()+"::"+name;
    it_p = it_p->parent();
  }
  return name;
}


void FunctionListModel::invalidateStatisticsFromParents(TreeList *item_p)
{
  TreeList *parent_p;
  parent_p = item_p->parent();

  item_p=parent_p;
  while(item_p!=rootItem)
  {
    int pos=item_p->row();
    QModelIndex item_index=createIndex(pos,FUNCTION_LIST_COLUMN_STATISTIC,item_p);
    QModelIndex parent_index;
    if (item_p->parent()!=rootItem)
      parent_index=createIndex(item_p->parent()->row(),0,item_p->parent());

    item_p->data(ITEM_NB_UNTESTED)=QVariant();
    item_p->data(ITEM_NB_TESTED)=QVariant();
    emit dataChanged(item_index,item_index);

    item_p=item_p->parent();
  }
}

QString FunctionListModel::functionName(const QModelIndex &index,bool measurement_only) const
{
  const TreeList *item_p=static_cast<TreeList*>(index.internalPointer());
  return functionName(item_p,measurement_only);
}


void FunctionListModel::setStatusColorLevelFunction(double low, double medium)
{
  function_low_level=low;
  function_medium_level=medium;
}


void FunctionListModel::setStatusColorLevelClass(double low, double medium)
{
  class_low_level=low;
  class_medium_level=medium;
}


QString FunctionListModel::source(const QModelIndex &index) const
{
  if (index.isValid())
  {
    TreeList *item_p;
    item_p = static_cast<TreeList*>(index.internalPointer());
    QString src=item_p->data(ITEM_FILE_NAME).toString();
    if (src.isEmpty())
      return item_p->data(ITEM_FILE_NAME_REF).toString();
    else
      return src;
  }
  return QString::null;
}

QString FunctionListModel::module (const QModelIndex &/*index*/) const
{
  return QString();
}

bool FunctionListModel::instrumentationLocationOrg(const QModelIndex &index,int &start_line,int &end_line) const
{
  if (index.isValid())
  {
    TreeList *item_p;
    item_p = static_cast<TreeList*>(index.internalPointer());
    start_line=item_p->data(ITEM_START_LINE_ORIG).toInt();
    end_line=item_p->data(ITEM_END_LINE_ORIG).toInt();
    return true;
  }
  return false;
}

bool FunctionListModel::instrumentationLocationPre(const QModelIndex &index,int &start_line,int &start_column,int &end_line,int &end_column) const
{
  if (index.isValid())
  {
    TreeList *item_p;
    item_p = static_cast<TreeList*>(index.internalPointer());
    start_line=item_p->data(ITEM_START_LINE).toInt();
    end_line=item_p->data(ITEM_END_LINE).toInt();
    start_column=item_p->data(ITEM_START_COLUMN).toInt();
    end_column=item_p->data(ITEM_END_COLUMN).toInt();
    return true;
  }
  return false;
}


QList<int> FunctionListModel::instrumentationIndexes(const QModelIndex &index) const
{
  QList<int>  instrumentation_list;
  if (index.isValid())
  {
    TreeList *item_p;
    item_p = static_cast<TreeList*>(index.internalPointer());
    int start_line_pre   = item_p->data(ITEM_START_LINE).toInt();
    int end_line_pre     = item_p->data(ITEM_END_LINE).toInt();
    int start_column_pre = item_p->data(ITEM_START_COLUMN).toInt();
    int end_column_pre   = item_p->data(ITEM_END_COLUMN).toInt();
    csmes_p->instrumentationListFunctionPre(module(index),source(index),start_line_pre,start_column_pre,end_line_pre,end_column_pre,instrumentation_list);
    return instrumentation_list;
  }
  return instrumentation_list;
}

void FunctionListModel::refreshView()
{
  beginResetModel();
  rootItem->setDataRecursive(ITEM_NB_TESTED,QVariant(),true);
  rootItem->setDataRecursive(ITEM_NB_UNTESTED,QVariant(),true);
  endResetModel();
}

CSMes::modifications_t FunctionListModel::compareCSMesFunction(TreeList *item_p) const
{
  bool ok;
  CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
  int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
  if (ok)
  {
    modif=static_cast<CSMes::modifications_t >(diff);
    return modif;
  }
  QString scoped_name=functionName(item_p,true);
  if (scoped_name.isEmpty())
  { // Namespace or class
    int nb=item_p->childCount();
    for (int i=0;i<nb;i++)
    {
      bool ok;
      TreeList *child_item_p=item_p->child(i);
      CSMes::modifications_t modif_item=CSMes::MODIFICATIONS_UNDEFINED;
      int diff=child_item_p->data(ITEM_DIFFERENCE).toInt(&ok);
      if (ok)
        modif_item=static_cast<CSMes::modifications_t >(diff);
      else
        modif_item=compareCSMesFunction(child_item_p);

      modif=CSMes::combineModifications(modif,modif_item);
    }
  }
  else
  {
    QString proto=item_p->data(ITEM_DESCRIPTION).toString();
    CSMes::functionskey_t func;
    {
      QString src=item_p->data(ITEM_FILE_NAME).toString();
      bool valid=!src.isEmpty();
      int start_line_pre   = valid?item_p->data(ITEM_START_LINE).toInt():-1;
      int start_line_org   = valid?item_p->data(ITEM_START_LINE_ORIG).toInt():-1;
      int end_line_pre     = valid?item_p->data(ITEM_END_LINE).toInt():-1;
      int end_line_org     = valid?item_p->data(ITEM_END_LINE_ORIG).toInt():-1;
      int start_column_pre = valid?item_p->data(ITEM_START_COLUMN).toInt():-1;
      int end_column_pre   = valid?item_p->data(ITEM_END_COLUMN).toInt():-1;
      int end_column_org   = -1;
      int start_column_org = -1;
      func.source=src;
      func.method=FunctionInfo(scoped_name,proto,start_line_org,start_column_org,end_line_org,end_column_org,start_line_pre,start_column_pre,end_line_pre,end_column_pre);
    }

    CSMes::functionskey_t func_ref;
    {
      QString src_ref=item_p->data(ITEM_FILE_NAME_REF).toString();
      bool valid=!src_ref.isEmpty();
      int start_line_ref_org   = valid?item_p->data(ITEM_START_LINE_REF_ORIG).toInt():-1;
      int end_line_ref_org     = valid?item_p->data(ITEM_END_LINE_REF_ORIG).toInt():-1;
      int start_line_ref_pre   = valid?item_p->data(ITEM_START_LINE_REF).toInt():-1;
      int end_line_ref_pre     = valid?item_p->data(ITEM_END_LINE_REF).toInt():-1;
      int start_column_ref_pre = valid?item_p->data(ITEM_START_COLUMN_REF).toInt():-1;
      int end_column_ref_pre   = valid?item_p->data(ITEM_END_COLUMN_REF).toInt():-1;
      int end_column_ref_org   = -1;
      int start_column_ref_org = -1;
      func_ref.source=src_ref;
      func_ref.method=FunctionInfo(scoped_name,proto,start_line_ref_org,start_column_ref_org,end_line_ref_org,end_column_ref_org,start_line_ref_pre,start_column_ref_pre,end_line_ref_pre,end_column_ref_pre);
    }

    modif=csmes_p->compareCSMesFunction(func,func_ref);
  }

  item_p->setData(ITEM_DIFFERENCE,modif);
  return modif;
}


void FunctionListModel::setFilters( bool _filter_different_indentation, bool _filter_different, bool _filter_different_comment, bool _filter_not_existing, bool _filter_new, bool _filter_same)
{
   filter_different_indentation=_filter_different_indentation;
   filter_different=_filter_different;
   filter_different_comment=_filter_different_comment;
   filter_not_existing=_filter_not_existing;
   filter_new=_filter_new;
   filter_same=_filter_same;
}

bool FunctionListModel::instrumentationLocation (const QModelIndex &index,int &start_line,int &start_column,int &end_line,int &end_column) const
{
  if (CoverageSettings::object().getSourceType()==CSMesUndoRedoFramework::CSMES)
    return instrumentationLocationPre(index,start_line,start_column,end_line,end_column);
  else
  {
    start_column=-1;
    end_column=-1;
    return instrumentationLocationOrg(index,start_line,end_line);
  }
}

