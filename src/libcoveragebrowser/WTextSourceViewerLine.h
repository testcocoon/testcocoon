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

#ifndef WTEXTSOURCEVIEWERLINECONTAIN_H
#define WTEXTSOURCEVIEWERLINECONTAIN_H
#include "coveragebrowserpdef.h"
#include <qwidget.h>
#include <qstring.h>
#include <qbitarray.h>
#include <QPaintEvent>
#include <QPixmap> 
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPen>
#include "instrumentation.h"
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
class WTextSourceViewerContain ;
#include <QTextDocument>

class WTextSourceViewerLine : public QWidget
{
  Q_OBJECT
  public:
    WTextSourceViewerLine( QWidget * parent = 0,  Qt::WindowFlags f = 0 )  ;
    virtual ~WTextSourceViewerLine( );
    void setFont ( const QFont & f ) ;
    void setContentsPos(int /*x*/,int y);
    void setDocument(int number_of_lines,const QTextDocument *);
    QFont font () const { return displayFont; }
    void setCommentLine(const QList <int> &v);
  signals:
    void hideLines(int current,const QBitArray &);
  protected:
    void paintEvent ( QPaintEvent * );
    void mousePressEvent ( QMouseEvent * e );
    void contextMenuEvent( QContextMenuEvent * );
  protected slots:
    void hideAll();
    void showAll();
  private:
    QImage img_comment;
    QImage img_comment_scaled;
    const QTextDocument *text_document_p;
    void setSizes();
    int translation_y;
    bool show_all;
    int number_of_lines;
    QAction *showAllAction;
    QAction *hideAllAction;
    QFont displayFont;
    inline void drawFoldingLineVisible(QPainter &p,const QPointF &p1,const QPointF &p2);
    inline void drawFoldingLineHidden(QPainter &p,const QPointF &pt);
    float char_space;
    QList <int> commented_lines;
};
#endif

