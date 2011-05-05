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

#ifndef WCOMMENT_WIDGET_H
#define WCOMMENT_WIDGET_H
#include "ui_WCommentWidgetUI.h"

class WCommentWidget : public QWidget, public Ui::WCommentWidgetUI
{
    Q_OBJECT

public:
    WCommentWidget(QWidget* parent = 0);
    virtual ~WCommentWidget();

    QString text();
    void setName(const QString &c) { _name=c; }
    QString name() const { return _name; }
    bool isModified() const ;
    void setCommentLengthCheck(bool b);

public slots:
    virtual void update_format_information();
    virtual void on_right_p_toggled( bool b );
    virtual void on_center_p_toggled( bool b );
    virtual void on_left_p_toggled( bool b );
    virtual void on_justify_p_toggled( bool b );
    virtual void on_bold_p_toggled( bool b );
    virtual void on_underline_p_toggled( bool b );
    virtual void on_textcolor_p_clicked(bool );
    virtual void on_italic_p_toggled( bool b );
    virtual void on_strike_p_toggled( bool b );
    virtual void on_ok_p_clicked();
    virtual void on_cancel_p_clicked();
    virtual void old_list_selection(  );
    virtual void setText( const QString & c );
    virtual void on_comment_p_textChanged();
signals:
    void accept();
    void reject();

protected:
    int old_alignment;

protected slots:
    virtual void languageChange();

private:
    bool _comment_length_check;
    long minimum_comment_size;
    QString _comment;

    void init();

  QString _name;
};


#endif
