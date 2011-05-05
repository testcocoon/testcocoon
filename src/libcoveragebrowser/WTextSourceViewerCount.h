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

#ifndef WTEXTSOURCEVIEWERCOUNTCONTAIN_H
#define WTEXTSOURCEVIEWERCOUNTCONTAIN_H
#include "coveragebrowserpdef.h"
#include <qwidget.h>
#include <qstring.h>
#include <qbitarray.h>
#include <QPaintEvent>
#include <QPixmap> 
#include <QHBoxLayout>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include "instrumentation.h"
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
class WTextSourceViewerContain ;
#include <QTextDocument>

class WTextSourceViewerCount : public QWidget
{
  Q_OBJECT
  public:
    WTextSourceViewerCount( 
        QWidget * parent = 0,Qt::WindowFlags f = 0 )  ;
    virtual ~WTextSourceViewerCount( );
    void setFont ( const QFont & f ) ;
    QFont font () const { return displayFont; }
    void setContentsPos(int /*x*/,int y);
    void setDocument(const QTextDocument *);
  protected:
    void paintEvent ( QPaintEvent * );
  public slots:
    void setCoverageCount(const QVector<int> &min,const QVector<int> &max);
  private:
    const QTextDocument *text_document_p;
    void setSizes();
    int translation_y;
    bool show_all;
    QFont displayFont;
    QVector<int> coverage_min,coverage_max;
};
#endif

