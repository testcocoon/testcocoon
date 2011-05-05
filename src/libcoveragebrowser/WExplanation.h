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

#ifndef WEXPLANATION_H
#define WEXPLANATION_H

#include <QList>
#include "ui_WExplanationUI.h"
#include "instrumentation.h"
#include "csmesundoredoframework.h"


class WExplanation : public QWidget, public Ui::WExplanationUI
{
    Q_OBJECT

public:
    WExplanation(QWidget* parent = 0,  Qt::WFlags fl = 0);
    virtual ~WExplanation();

public slots:
    virtual void setExplanation( const QString &text );
    virtual void setCSMes( CSMesUndoRedoFramework * c_p );
    virtual void setExplanationOriginal( const QString & module, const QString & source, const QList<int> & lines ,int coverage_level,Instrumentation::coverage_method_t,int executed_by_limit );
    virtual void setExplanationPreprocessed( const QString & module, const QString & source, const QList<int> & indexs ,int coverage_level,Instrumentation::coverage_method_t ,int executed_by_limit);

protected:
    CSMesUndoRedoFramework * csmes_p;

protected slots:
  void setExplanation(const QString &,const QString &,const QList<int> &,CSMes::source_type_t ,int ,Instrumentation::coverage_method_t ,int,const QString &);
    virtual void languageChange();

private:
    void _setExplanation(const QString &module,const QString &source,const QList<int> &lines_indexs,CSMes::source_type_t source_type,int coverage_level,Instrumentation::coverage_method_t method,int executed_by_limit);
    QString _requested_module;
    QString _requested_source;
    QList<int> _requested_lines_indexs;
    CSMes::source_type_t _requested_source_type;
    int _requested_executed_by_limit;
    int _requested_coverage_level;
    Instrumentation::coverage_method_t _requested_method;
    void init();
    void destroy();

};

#endif // WEXPLANATION_H
