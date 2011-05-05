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


#ifndef WOPTION_H
#define WOPTION_H

#include <qvariant.h>
#include "coveragebrowserpdef.h"
#include <QDialog>
#include "ui_WOptionUI.h"

class WMain ;

class WOption : public QDialog, public Ui::WOptionUI
{
    Q_OBJECT

public:
    WOption(WMain *m,QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WOption();

    double module_low_level() const ;
    double module_medium_level() const ;
    double class_low_level() const ;
    double class_medium_level() const ;
    double function_low_level() const ;
    double function_medium_level() const;
    QString window_style() const;
    QString language() const;
    int getExecutionCacheMaxCost() const;
    int getSourceCacheMaxCost() const;
    int getStatisticCacheMaxCost() const;
    void setSourceCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) ;
    void setStatisticCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) ;
    void setExecutionCacheStatistics(int cache_hit,int cache_miss,int max_cost,int cost) ;

public slots:
    void on_buttonOk_clicked();
    void on_buttonCancel_clicked();
    void checkCoherence();
    void on_function_medium_level_p_valueChanged(double);
    void on_function_low_level_p_valueChanged(double);
    void on_class_medium_level_p_valueChanged(double);
    void on_class_low_level_p_valueChanged(double);
    void on_module_medium_level_p_valueChanged(double);
    void on_module_low_level_p_valueChanged(double);
    void on_no_comment_for_validation_p_toggled( bool );
    void on_tooltip_source_view_p_toggled( bool );
    void on_tooltip_source_list_p_toggled( bool );
    void on_tooltip_execution_list_p_toggled( bool );
    void on_tooltip_method_list_p_toggled( bool );
    void on_main_font_p_clicked();
    void on_source_code_font_p_clicked();
    void on_comment_font_p_clicked();
    void on_toolbar_p_clicked();
    void on_reset_warning_p_clicked() ;

protected slots:
    virtual void languageChange();
    virtual void on_window_style_p_activated(const QString &);

private:
    void init();
    void destroy();
    WMain *wmain_p;

};

#endif // WOPTION_H
