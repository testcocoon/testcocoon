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

#ifndef WSTATISTIC_H
#define WSTATISTIC_H
#include "ui_WStatisticUI.h"
#include "coveragebrowserpdef.h"
#include "csmesundoredoframework.h"
#include <QStyle>

class WStatistic : public QWidget, public Ui::WStatisticUI
{
    Q_OBJECT

public:
    WStatistic(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WStatistic();
    void setCSMes(const CSMesUndoRedoFramework *);
public slots:
    void refreshView();	
    void on_coverage_level1_label_p_clicked(bool);
    void on_coverage_label_p_clicked(bool);
private:
    void setCoverageLevel(int);
    const CSMesUndoRedoFramework *csmes_p;
    void setCoverageLevel1Text(const QString &t);
    void setCoverageText(const QString &t);
    void setCoverageLevel1Value(int nb_tested,int nb_untested);
    void setCoverageValue(int nb_tested,int nb_untested);
    void showLevel1(bool);
};


#endif
