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

#include "Service.h"
#include <qstatusbar.h>
#include <qstring.h>
#include <qobject.h>
#include <QPixmap>
#include <stdlib.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include "libinstrumentationpdef.h"
#include <QMainWindow>
#include <qapplication.h>
#include <qworkspace.h>
#include <qimage.h>
#include <qstatusbar.h>
#include <QProgressBar>
#include <QLabel>
#include <QProgressDialog> 

static QMainWindow *mainwin_p=NULL;
static QProgressBar *progress_bar_p = NULL;
void            printStatus(const QString &t, float progress)
{
  if (mainwin_p!=NULL)
  {
    QString p;
    if (progress>=0.0)
    {
      if (progress_bar_p!=NULL && !mainwin_p->isHidden())
      {
        progress_bar_p->setValue(static_cast<int>(progress*10000));
        if (progress_bar_p->isHidden())
        {
          progress_bar_p->show();
          QApplication::sendPostedEvents();
        }
      }
    }
    else if (progress_bar_p!=NULL)
    {
      progress_bar_p->reset();
      if (!progress_bar_p->isHidden())
      {
        progress_bar_p->hide();
        QApplication::sendPostedEvents();
      }
    }
    
    mainwin_p->statusBar()->showMessage(t);
  }
}

void            set_MainWindow(QMainWindow *p)
{
  mainwin_p = p;
  if (mainwin_p!=NULL)
  {
    if (progress_bar_p==NULL)
    {
      progress_bar_p = new QProgressBar();
      progress_bar_p->setRange(0,10000);
      progress_bar_p->setTextVisible(false);
    }
    mainwin_p->statusBar()->addPermanentWidget(progress_bar_p,0);
    progress_bar_p->hide();
  }
  else
  {
    if (progress_bar_p!=NULL)
      delete progress_bar_p ;
  }
}


QMainWindow *   get_MainWindow()
{
  return  mainwin_p;
}

static QWorkspace *workspace_p=NULL;
void            set_Workspace(QWorkspace *w)
{
  workspace_p = w;
}


QWorkspace *   get_Workspace()
{
  return workspace_p ;
}


static int tab_size=-1;
int tabSize()
{
  if (tab_size<=0)
    tab_size=8;
  return tab_size;
}

void setTabSize(int v)
{
    tab_size=v;
}

