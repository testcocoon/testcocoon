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

#ifndef WTEXTSOURCEVIEWER_CONTAIN_H
#define WTEXTSOURCEVIEWER_CONTAIN_H
#include "coveragebrowserpdef.h"
#include <qwidget.h>
#include <qstring.h>
#include <QTextEdit>
#include <QLabel>
#include <QList>
#include <QWheelEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include "WTextSourceViewer.h"
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
class WTextSourceViewerLine ;
class WTextSourceViewerContain ;
class WTextSourceViewer ;
class WTextSourceViewerHighlighter;

class WTextSourceViewerContain : public QTextEdit
{
  Q_OBJECT
  public:
    WTextSourceViewerContain(
        QWidget  * parent = 0)  ;
    virtual ~WTextSourceViewerContain( );
    void setWTextSourceViewer (WTextSourceViewer *v) { wtextsourceviewer=v; }
    virtual void wheelEvent ( QWheelEvent * e ) ;
    QFont font () const { return displayFont; }
    QFont currentFont () const { return displayFont; }
    void setDocument(QTextDocument *);
    void setCursorPosition(int para,int index);
    void getCursorPosition(int &para,int &index) const;
    void getSelection(int &start_para,int &start_index,int &end_para,int &end_index) const;
    int getDisplayOffset() const ;
    void setDisplayOffset(int);
    void rehighlight();
  signals:
	void instrumentationsSelected();
    void fontChanged(const QFont &);
    void contentsMoving(int,int);
  public slots:
    void zoomIn();
    void zoomOut();
    void setCurrentFont ( const QFont & f );
    void setFont ( const QFont & f ) { setCurrentFont(f) ; }
    void setTabStopWidth(int);
  protected:
    void mouseReleaseEvent ( QMouseEvent * event ) ;
    virtual void keyPressEvent ( QKeyEvent * e ) ;
    void scrollContentsBy ( int dx, int dy );
    virtual void contextMenuEvent(QContextMenuEvent * e) ;
    WTextSourceViewer *wtextsourceviewer;
    QList<WTextSourceViewer::background_t> bg_line_color;
  private:
    QFont displayFont;
    bool in_wheelEvent;
    int tab_stop_width;
    WTextSourceViewerHighlighter *_highlighter;
};

#endif

