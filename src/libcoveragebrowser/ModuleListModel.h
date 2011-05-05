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

#ifndef MODULE_LIST_MODEL_H
#define MODULE_LIST_MODEL_H
#include "instrumentation.h"
#include "csmesundoredoframework.h"
#include "TreeListModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeList;
class CSMesUndoRedoFramework;
#define MODULE_LIST_COLUMN_BASENAME 0
#define MODULE_LIST_COLUMN_STATISTIC 1
#define MODULE_LIST_COLUMN_FULLNAME 2
#define MODULE_LIST_COLUMN_FULLNAME_REFERENCE 3
#define MODULE_LIST_COLUMN_RELATIVENAME 4
#define MODULE_LIST_COLUMN_RELATIVENAME_REFERENCE 5
#define MODULE_LIST_COLUMN_DIFFERENCE 6
#define MODULE_LIST_MAX_COLUMN 7


class ModuleListModel : public  TreeListModel
{
  Q_OBJECT

  public:
    ModuleListModel(QObject *parent = 0);
    void setCSMes(const CSMesUndoRedoFramework *);
    virtual ~ModuleListModel();

    QVariant data(const QModelIndex &index, int role) const;

    QString source(const QModelIndex &index) const ;
    QString module(const QModelIndex &index) const ;
    void setStatusColorLevel(double low, double medium);
    QColor colorStatus(double v) const;
    QModelIndex find(const QString & module, const QString &source) const ;
    void setFilters( bool filter_different_indentation, bool filter_different, bool filter_different_comment, bool filter_not_existing, bool filter_new, bool filter_same,bool filter_other);
    void setFlatView(bool b);

  private:
    void setupModelData();
    const CSMesUndoRedoFramework *csmes_p;
    double low_level,medium_level;
    CSMes::modifications_t compareCSMesSource(const TreeList *item_p) const;
    QString displayNameAbsolute(const TreeList *item_p) const;
    bool filter_different_indentation;
    bool filter_different;
    bool filter_different_comment;
    bool filter_not_existing;
    bool filter_new;
    bool filter_same;
    bool filter_other;
    bool flat_view;
  public slots:
    void refreshView();
};

#endif
