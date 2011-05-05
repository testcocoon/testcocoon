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

#include "coveragebrowserpdef.h"
#include "ProgresBar.h"
#include <qobject.h>
#include <math.h>
#include <qwidget.h>
#include "Service.h"
#include <QProgressDialog>

static QWidget *main_widget = NULL;

ProgresBar::ProgresBar (QString titre_text, bool cancel)
{
    Titre = titre_text;

    if (cancel)
    {
        if (main_widget!=NULL)
            main_widget->hide();
        ProgresBar_p = new QProgressDialog (titre_text, QObject::tr ("Cancel"), 0, 1000);
        ProgresBar_p->show();
        ProgresBar_p->setValue (0);
        ProgresBar_p->setWindowTitle (QObject::tr ("Progression"));
    }
    else
    {
        ProgresBar_p = NULL;
        printStatus(titre_text,0.0);
    }
    currentText=titre_text;
    currentProgress=0;
    progresval = 0.0;
}


ProgresBar::~ProgresBar ()
{
    if (main_widget!=NULL)
        main_widget->show();
    if (ProgresBar_p!=NULL)
        delete  ProgresBar_p;
}


bool ProgresBar::Progress (float val)
{
    currentProgress=val;
    if (fabs (progresval - val) > PROGRESS_BAR_PAS)
    {
        if (ProgresBar_p!=NULL)
        {
            ProgresBar_p->setValue (static_cast<int>(1000 * val));
        }
        else
        {
            printStatus(currentText,currentProgress);
        }
        progresval = val;
    }
    if (ProgresBar_p!=NULL)
        if (ProgresBar_p->wasCanceled ())
            return false;
    return true;
}


bool ProgresBar::PrintLog (QString str)
{
    currentText=str;
    ProgresBar_p->setLabelText (str);
    return true;
}


void ProgresBar::setMainWidget(QWidget *w)
{
    main_widget = w;
}
