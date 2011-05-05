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

#ifndef PROGRESBAR_H
#define PROGRESBAR_H
#include "coveragebrowserpdef.h"
#include <QWidget>
#include <QProgressDialog>
#include <qstring.h>
class           ProgresBar
{
private:
    QProgressDialog * ProgresBar_p;
#define PROGRESS_BAR_PAS 0.05

    QString            Titre;
    float           progresval;
    float currentProgress;
    QString currentText;
public:
    ProgresBar (QString titre_text,
                bool cancel = false);
    virtual ~ProgresBar ();
    bool            Progress (float val);
    bool            PrintLog (QString str);

    static void setMainWidget(QWidget *w);

protected:

};
#endif
