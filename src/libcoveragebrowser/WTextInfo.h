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

#ifndef WTEXTINFO_H
#define WTEXTINFO_H

#include "ui_WTextInfoUI.h"
#include <QDialog>


class WTextInfo : public QDialog, public Ui::WTextInfoUI
{
    Q_OBJECT

public:
    WTextInfo(QWidget* parent = 0,  Qt::WindowFlags fl = 0);
    virtual ~WTextInfo();

public slots:
    virtual void close();
    virtual void setText( const QString & title, const QString & text );

protected slots:
    virtual void languageChange();

};

#endif // WTEXTINFO_H
