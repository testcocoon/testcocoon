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

#ifndef FUNCTION_LIST_MODEL_H
#define FUNCTION_LIST_MODEL_H
#include "instrumentation.h"
#include "csmes.h"
#include <QAbstractItemModel>
#include "TreeListModel.h"
#include <QModelIndex>
#include <QVariant>
#include <QStringList>

class TreeList;
class CSMesUndoRedoFramework;
#define FUNCTION_LIST_COLUMN_NAME 0
#define FUNCTION_LIST_COLUMN_DESCRIPTION 1
#define FUNCTION_LIST_COLUMN_STATISTIC 2
#define FUNCTION_LIST_COLUMN_FILE_NAME 3
#define FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME 4
#define FUNCTION_LIST_COLUMN_POSITION 5
#define FUNCTION_LIST_COLUMN_DIFFERENCE 6
#define FUNCTION_LIST_COLUMN_FILE_NAME_REF 7
#define FUNCTION_LIST_COLUMN_ABSOLUTE_FILE_NAME_REF 8
#define FUNCTION_LIST_COLUMN_POSITION_REF 9
#define FUNCTION_LIST_MAX_COLUMN 10


class FunctionListModel : public  TreeListModel
{
    Q_OBJECT

public:
    FunctionListModel(QObject *parent = 0);
    void setCSMes(CSMesUndoRedoFramework *);
    virtual ~FunctionListModel();

    bool instrumentationLocation(const QModelIndex &index,int &start_line,int &start_column,int &end_line,int &end_column) const ;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QModelIndex find(const QString & module, const QString &source) const ;
    QString functionName(const QModelIndex &,bool measurement_only=true) const;

    void setStatusColorLevelFunction(double low, double medium);
    void setStatusColorLevelClass(double low, double medium);

    QString source(const QModelIndex &index) const ;
    QString module(const QModelIndex &index) const ;
    QList<int> instrumentationIndexes(const QModelIndex &index) const ;

    void setFilters( bool filter_different_indentation, bool filter_different, bool filter_different_comment, bool filter_not_existing, bool filter_new, bool filter_same);

signals:
    void updateGUI();

private:
    QString functionName(const TreeList *e,bool measurement_only=true) const;
    void setupModelData();
    CSMesUndoRedoFramework *csmes_p;
    void invalidateStatisticsFromParents(TreeList *item_p);
    TreeList * insertFunction (const QString &name,const QString &description,const QString &filename,long,long,long start_line,long start_column,long end_line, long end_column, bool reference);
    double class_low_level, class_medium_level, function_low_level, function_medium_level;
    void generateStatistics(TreeList *item_p) const;
    inline TreeList *findFunction(const TreeList *parent_p,const QString &name_no_scope,const QString &prototype) const;
    bool filter_different_indentation;
    bool filter_different;
    bool filter_different_comment;
    bool filter_not_existing;
    bool filter_new;
    bool filter_same;
    bool instrumentationLocationPre(const QModelIndex &index,int &start_line,int &start_column,int &end_line,int &end_column) const ;
    bool instrumentationLocationOrg(const QModelIndex &index,int &start_line,int &end_line) const ;
    CSMes::modifications_t compareCSMesFunction(TreeList *item_p) const;
  public slots:
    void refreshView();
};

#endif
