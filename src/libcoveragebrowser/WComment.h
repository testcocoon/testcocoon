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

#ifndef WCOMMENT_H
#define WCOMMENT_H
#include "ui_WCommentUI.h"

class WComment : public QDialog, public Ui::WCommentUI
{
    Q_OBJECT

public:
    WComment(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~WComment();

    virtual int exec();
    QString text();
    void setCommentLengthCheck(bool b) { comment_p->setCommentLengthCheck(b); }

public slots:
    virtual void acceptSlot();
    virtual void rejectSlot();
    void setText(const QString &c);


protected slots:
    virtual void languageChange();

};


#endif
