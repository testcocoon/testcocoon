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
#include "TreeList.h"
#include "ModuleListModel.h"
#include "options.h"
#include "CoverageSettings.h"
enum MODULE_LIST_ITEMS
{
  ITEM_NAME_ABSOLUTE,
  ITEM_NAME_ABSOLUTE_REF,
  ITEM_NB_TESTED,
  ITEM_NB_UNTESTED,
  ITEM_DIFFERENCE,
  ITEM_MAX,
};

ModuleListModel::ModuleListModel (QObject *parent) : TreeListModel(parent,MODULE_LIST_MAX_COLUMN)
{
  filter_different_indentation=true;
  filter_different=true;
  filter_different_comment=true;
  filter_not_existing=true;
  filter_new=true;
  filter_same=true;
  filter_other=true;
  csmes_p=NULL;
  flat_view=Options::get_opt_bool("","MODULE_VIEW_FLAT_VIEW",true);
}

ModuleListModel::~ModuleListModel()
{
}

void ModuleListModel::setCSMes(const CSMesUndoRedoFramework *c)
{
  csmes_p=c;
  setupModelData();
}

QString ModuleListModel::displayNameAbsolute(const TreeList *item_p) const
{
  if (item_p)
  {
    if (item_p->data(ITEM_NAME_ABSOLUTE).toString().isEmpty())
      return item_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
    else
      return item_p->data(ITEM_NAME_ABSOLUTE).toString();
  }
  else
    return QString();
}

QVariant ModuleListModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  TreeList *item_p = static_cast<TreeList*>(index.internalPointer());
  /* fill information into the tree structure */
  {
    bool ok;
    item_p->data(ITEM_NB_TESTED).toInt(&ok);
    if (!ok)
    {
      if (item_p->data(ITEM_NAME_ABSOLUTE).toString().isEmpty())
      {
        item_p->data(ITEM_NB_TESTED) = -1;
        item_p->data(ITEM_NB_UNTESTED) = -1;
      }
      else
      {
        int nb_tested=-1;
        int nb_untested=-1;
        TreeList *parent_p=item_p->parent();
        if (parent_p==rootItem)
        {
          if (item_p->childCount()>0)
            csmes_p->statistic(ModuleFile(item_p->data(ITEM_NAME_ABSOLUTE).toString()),SourceFile(""),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),nb_tested,nb_untested);
          else
            csmes_p->statistic(ModuleFile(""),SourceFile(item_p->data(ITEM_NAME_ABSOLUTE).toString()),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),nb_tested,nb_untested);
        }
        else
          csmes_p->statistic(parent_p->data(ITEM_NAME_ABSOLUTE).toString(),item_p->data(ITEM_NAME_ABSOLUTE).toString(),CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),nb_tested,nb_untested);
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
        bool ok=false;
        CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
        int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
        if (ok)
          modif=static_cast<CSMes::modifications_t >(diff);
        else
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesSource(item_p)));
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
            if (!filter_other)
              return true;
            break;
        }
      }
      return false;
    case SearchTextRole:
      return displayNameAbsolute(item_p);

    case Qt::ToolTipRole:
      {
        if (!Options::get_opt_bool(QString(),"TOOLTIP_SOURCE_LIST",DEF_TOOLTIP_SOURCE_LIST))
          return QVariant();

        QFileInfo fileInfo(displayNameAbsolute(item_p));

        QString tooltip="<body><html>";
        tooltip+=tr("<u><I>Source:</I></u>&nbsp;<TT>%1</TT>").arg(Qt::escape(fileInfo.fileName()));
        tooltip+=tr("<BR><u><I>Relative&nbsp;File&nbsp;Name:</I></u>&nbsp;<TT>%1</TT>").arg(Qt::escape(csmes_p->relativeSourceName(displayNameAbsolute(item_p))));
        tooltip+=tr("<BR><u><I>Absolute&nbsp;File&nbsp;Name:</u>&nbsp;<TT>%1</TT>").arg(Qt::escape(displayNameAbsolute(item_p)));
        bool ok;
        int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
        int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
        if (nb_tested+nb_untested>0)
          tooltip+=tr("<BR><U><I>Coverage:</I></U>&nbsp;%1").arg(Qt::escape(CSMesUndoRedoFramework::printStat(nb_tested,nb_untested)));
        CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
        int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
        if (ok)
          modif=static_cast<CSMes::modifications_t >(diff);
        else
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesSource(item_p)));
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
            case MODULE_LIST_COLUMN_STATISTIC:
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
          item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesSource(item_p)));
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
      return displayNameAbsolute(item_p);
    case Qt::BackgroundColorRole:
      {
        if (CoverageSettings::object().getExecutionAnalysisMode())
          return QVariant();
        bool ok;
        int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
        int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
        if (nb_tested+nb_untested>0)
        {
          double stat=static_cast<double>(nb_tested)/static_cast<double>(nb_tested+nb_untested)*100.0;
          return QVariant(colorStatus(stat));
        }
        return QVariant();
      }
    case Qt::DisplayRole:
      switch (index.column())
      {
        case MODULE_LIST_COLUMN_STATISTIC:
          {
            bool ok;
            int nb_tested=item_p->data(ITEM_NB_TESTED).toInt(&ok);
            int nb_untested=item_p->data(ITEM_NB_UNTESTED).toInt(&ok);
            if (nb_tested+nb_untested>0)
              return CSMesUndoRedoFramework::printStat(nb_tested,nb_untested) ;
            return QVariant();
          }
        case MODULE_LIST_COLUMN_RELATIVENAME:
           return csmes_p->relativeSourceName(item_p->data(ITEM_NAME_ABSOLUTE).toString());
        case MODULE_LIST_COLUMN_RELATIVENAME_REFERENCE:
           return csmes_p->relativeSourceNameReference(item_p->data(ITEM_NAME_ABSOLUTE_REF).toString());
        case MODULE_LIST_COLUMN_FULLNAME:
           {
            return item_p->data(ITEM_NAME_ABSOLUTE).toString();
          }
        case MODULE_LIST_COLUMN_FULLNAME_REFERENCE:
          {
            return item_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
          }
        case MODULE_LIST_COLUMN_BASENAME:
          {
            QFileInfo fileInfo(displayNameAbsolute(item_p));
            return fileInfo.fileName();
          }
        case MODULE_LIST_COLUMN_DIFFERENCE:
          {
            bool ok;
            CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
            int diff=item_p->data(ITEM_DIFFERENCE).toInt(&ok);
            if (ok)
              modif=static_cast<CSMes::modifications_t >(diff);
            else
              item_p->setData(ITEM_DIFFERENCE,static_cast<int>(modif=compareCSMesSource(item_p)));
            return CSMes::modificationsToString(modif);
          }
      }
  }
  return QVariant();
}

CSMes::modifications_t ModuleListModel::compareCSMesSource(const TreeList *item_p) const
{
  CSMes::modifications_t modif=CSMes::MODIFICATIONS_UNDEFINED;
  QString src,mod;
  TreeList *parent_p=item_p->parent();
  if (parent_p==rootItem)
  {
    if (item_p->childCount()>0)
    {
      mod=item_p->data(ITEM_NAME_ABSOLUTE).toString();
      src="";
    }
    else
    {
      src=item_p->data(ITEM_NAME_ABSOLUTE).toString();
      mod="";
    }
  }
  else
  {
    src=item_p->data(ITEM_NAME_ABSOLUTE).toString();
    mod=parent_p->data(ITEM_NAME_ABSOLUTE).toString();
  }
  if (src.isEmpty() && mod.isEmpty())
  {
    if (parent_p==rootItem)
    {
      if (item_p->childCount()>0)
      {
        mod=item_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
        src="";
      }
      else
      {
        src=item_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
        mod="";
      }
    }
    else
    {
      src=item_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
      mod=parent_p->data(ITEM_NAME_ABSOLUTE_REF).toString();
    }
  }
  modif=csmes_p->compareCSMesSource(mod,src);
  return modif;
}

void ModuleListModel::setupModelData()
{
  beginResetModel();
  if (rootItem)
    delete rootItem;

  QVector<QVariant> rootData(MODULE_LIST_MAX_COLUMN);
  rootData[MODULE_LIST_COLUMN_BASENAME]=tr("Source") ;
  rootData[MODULE_LIST_COLUMN_STATISTIC]=tr("Coverage") ;
  rootData[MODULE_LIST_COLUMN_FULLNAME]=tr("Absolute Path") ;
  rootData[MODULE_LIST_COLUMN_FULLNAME_REFERENCE]=tr("Absolute Path (reference)") ;
  rootData[MODULE_LIST_COLUMN_RELATIVENAME]=tr("Relative Path") ;
  rootData[MODULE_LIST_COLUMN_RELATIVENAME_REFERENCE]=tr("Relative Path (reference)") ;
  rootData[MODULE_LIST_COLUMN_DIFFERENCE]=tr("Modifications") ;
  rootItem = new TreeList(rootData);

  if (csmes_p)
  {
    QStringList all;
    QStringList all_reference;
    if (flat_view)
    {
      all=csmes_p->Sources(CSMes::NON_EMPTY).toStringList();
      all_reference =csmes_p->SourcesReference(CSMes::NON_EMPTY).toStringList();
    }
    else
    {
      all=csmes_p->Modules().toStringList();
      all_reference =csmes_p->ModulesReference().toStringList();
    }

    QStringList::const_iterator itmod;
    for (itmod=all.begin();itmod!=all.end();++itmod)
    {
      QVector<QVariant> datas(ITEM_MAX);
      datas[ITEM_NAME_ABSOLUTE]=*itmod;
      TreeList *module_p = new TreeList(datas,rootItem);

      if (!flat_view)
      {
        QStringList sources=csmes_p->Sources(*itmod).toStringList();
        QStringList::const_iterator itsrc;
        for (itsrc=sources.begin();itsrc!=sources.end();++itsrc)
        {
          QVector<QVariant> datas(ITEM_MAX);
          datas[ITEM_NAME_ABSOLUTE]=*itsrc;
          new TreeList(datas,module_p);
        }
      }
    }
    for (itmod=all_reference.begin();itmod!=all_reference.end();++itmod)
    {
      QList<int> index_module;
      QList<TreeList*> items;
      const QString module(*itmod);
      QStringList sources_reference=csmes_p->SourcesReference(*itmod).toStringList();
      if (sources_reference.isEmpty())
      {
        ModuleFile mod,mod_ref;
        SourceFile src,src_ref;
        csmes_p->equivalentModulesReference(ModuleFile(),SourceFile(module),mod,src,mod_ref,src_ref);
        if (src.isEmpty())
        {
          rootItem->find(src_ref,ITEM_NAME_ABSOLUTE_REF,1,index_module,items);
          ASSERT(items.count()<=1);
          if (items.isEmpty())
          {
            QVector<QVariant> datas(ITEM_MAX);
            datas[ITEM_NAME_ABSOLUTE_REF]=module;
            new TreeList(datas,rootItem);
          }
        }
        else
        {
          rootItem->find(src,ITEM_NAME_ABSOLUTE,1,index_module,items);
          ASSERT(items.count()==1);
          for (QList<TreeList*>::iterator it=items.begin(); it!=items.end();++it)
            (*it)->data(ITEM_NAME_ABSOLUTE_REF)=src_ref;
        }
      }
      else
      {
        TreeList *module_p =NULL;
        ModuleFile mod,mod_ref;
        SourceFile src,src_ref;
        QStringList::const_iterator itsrc;
        for (itsrc=sources_reference.begin();itsrc!=sources_reference.end();++itsrc)
        {
          csmes_p->equivalentModulesReference(module,*itsrc,mod,src,mod_ref,src_ref);
          if (module_p==NULL)
          {
            rootItem->find(mod_ref,ITEM_NAME_ABSOLUTE_REF,1,index_module,items);
            ASSERT(items.count()<=1);
            if (items.isEmpty() || mod_ref.isEmpty())
            {
              ModuleFile mod_tmp;
              SourceFile src_tmp;
              ModuleFile mod_ref_tmp;
              SourceFile src_ref_tmp;
              csmes_p->equivalentModulesReference(mod_ref,src_ref,mod_tmp,src_tmp,mod_ref_tmp,src_ref_tmp);
              if (mod_tmp.isEmpty())
              {
                rootItem->find(mod_ref,ITEM_NAME_ABSOLUTE_REF,1,index_module,items);
                ASSERT(items.count()<=1);
                if (items.isEmpty())
                {
                  QVector<QVariant> datas(ITEM_MAX);
                  datas[ITEM_NAME_ABSOLUTE_REF]=mod_ref;
                  module_p = new TreeList(datas,rootItem);
                }
              }
              else
              {
                rootItem->find(mod_tmp,ITEM_NAME_ABSOLUTE,1,index_module,items);
                ASSERT(items.count()==1);
                for (QList<TreeList*>::iterator it=items.begin(); it!=items.end();++it)
                  (*it)->data(ITEM_NAME_ABSOLUTE_REF)=mod_ref;
                module_p=items.first();
              }
            }
            else
              module_p=items.first();
          }

          if (src.isEmpty())
          {
            module_p->find(src_ref,ITEM_NAME_ABSOLUTE_REF,1,index_module,items);
            ASSERT(items.count()<=1);
            if ((!flat_view) && items.isEmpty())
            {
              QVector<QVariant> datas(ITEM_MAX);
              datas[ITEM_NAME_ABSOLUTE_REF]=*itsrc;
              new TreeList(datas,module_p);
            }
          }
          else
          {
            module_p->find(src,ITEM_NAME_ABSOLUTE,1,index_module,items);
            ASSERT(items.count()<=1);
            for (QList<TreeList*>::iterator it=items.begin(); it!=items.end();++it)
               (*it)->data(ITEM_NAME_ABSOLUTE_REF)=mod_ref;
          }
        }
      }
    }
  }
  rootItem->squeeze();
  endResetModel();
}

QColor ModuleListModel::colorStatus (double v) const
{
  if (v<-1)
    return COL_WHITE;
  if (v<low_level)
    return COL_RED;
  if (v<medium_level)
    return COL_ORANGE;
  return COL_GREEN;
}

void ModuleListModel::setStatusColorLevel(double low, double medium)
{
  low_level=low;
  medium_level=medium;
}

QString ModuleListModel::source(const QModelIndex &index) const
{
  if (index.isValid())
  {
    TreeList *item_p;
    item_p = static_cast<TreeList*>(index.internalPointer());
    QModelIndex parent=index.parent();
    if (parent.isValid())
      return displayNameAbsolute(item_p);
    else
    {
      if (item_p->childCount()==0)
        return displayNameAbsolute(item_p);
    }
  }
  return QString::null;
}

QString ModuleListModel::module (const QModelIndex &index) const
{
  if (index.isValid())
  {
    TreeList *item_p;
    QModelIndex parent=index.parent();
    item_p = static_cast<TreeList*>(index.internalPointer());
    if (parent.isValid())
    {
      TreeList *parent_p;
      parent_p = static_cast<TreeList*>(parent.internalPointer());
      return displayNameAbsolute(parent_p);
    }
    else
    {
      if (item_p->childCount()!=0)
        return displayNameAbsolute(item_p);
    }
  }
  return QString::null;
}

QModelIndex ModuleListModel::find(const QString & module, const QString &source) const
{
  QList<int> index_modules;
  QList<TreeList*> found_modules;
  rootItem->find(module,ITEM_NAME_ABSOLUTE,1,index_modules,found_modules);
  if (!found_modules.isEmpty())
  {
    //ASSERT(found_modules.count()<=1);
    TreeList* found_module=found_modules.first();
    int index_module=index_modules.first();
    if (source==module)
      return createIndex(index_module,0,found_module);
    QList<int> index_sources;
    QList<TreeList*> found_sources;
    found_module->find(source,ITEM_NAME_ABSOLUTE,1,index_sources,found_sources);
    if (!found_sources.isEmpty())
    {
      ASSERT(found_sources.count()<=1);
      int index_source=index_sources.first();
      TreeList* found_source=found_sources.first();
      return createIndex(index_source,0,found_source);
    }
    else
      return createIndex(index_module,0,found_module);
  }
  else
  {
    QList<int> index_sources;
    QList<TreeList*> found_sources;
    rootItem->find(source,ITEM_NAME_ABSOLUTE,1,index_sources,found_sources);
    if (!found_sources.isEmpty())
    {
      ASSERT(found_sources.count()<=1);
      int index_source=index_sources.first();
      TreeList* found_source=found_sources.first();
      return createIndex(index_source,0,found_source);
    }
    else
    {
      rootItem->find(module,ITEM_NAME_ABSOLUTE_REF,1,index_modules,found_modules);
      if (!found_modules.isEmpty())
      {
        ASSERT(found_modules.count()<=1);
        TreeList* found_module=found_modules.first();
        int index_module=index_modules.first();
        if (source==module)
          return createIndex(index_module,0,found_module);
        QList<int> index_sources;
        QList<TreeList*> found_sources;
        found_module->find(source,ITEM_NAME_ABSOLUTE_REF,1,index_sources,found_sources);
        if (!found_sources.isEmpty())
        {
          ASSERT(found_sources.count()<=1);
          int index_source=index_sources.first();
          TreeList* found_source=found_sources.first();
          return createIndex(index_source,0,found_source);
        }
        else
          return createIndex(index_module,0,found_module);
      }
      else
      {
        QList<int> index_sources;
        QList<TreeList*> found_sources;
        rootItem->find(source,ITEM_NAME_ABSOLUTE_REF,1,index_sources,found_sources);
        if (!found_sources.isEmpty())
        {
          ASSERT(found_sources.count()<=1);
          int index_source=index_sources.first();
          TreeList* found_source=found_sources.first();
          return createIndex(index_source,0,found_source);
        }
      }
    }
  }

  return QModelIndex();
}

void ModuleListModel::refreshView()
{
  beginResetModel();
  rootItem->setDataRecursive(ITEM_NB_UNTESTED,QVariant(),true);
  rootItem->setDataRecursive(ITEM_NB_TESTED,QVariant(),true);
  endResetModel();
}


void ModuleListModel::setFlatView( bool v )
{
  bool refresh= (flat_view!=v);
   flat_view=v;
   if (refresh)
     setupModelData();
}

void ModuleListModel::setFilters( bool _filter_different_indentation, bool _filter_different, bool _filter_different_comment, bool _filter_not_existing, bool _filter_new, bool _filter_same, bool _filter_other)
{
   filter_different_indentation=_filter_different_indentation;
   filter_different=_filter_different;
   filter_different_comment=_filter_different_comment;
   filter_not_existing=_filter_not_existing;
   filter_new=_filter_new;
   filter_same=_filter_same;
   filter_other=_filter_other;
}
