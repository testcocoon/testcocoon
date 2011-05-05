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

#include "WTextSourceViewerContain.h"
#include <QAbstractSlider>
#include "WTextSourceViewer.h"
#include "rtf.h"
#include <qsplitter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QWheelEvent>
#include <QList>
#include <QMenu>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTextDocument>
#include <QAbstractScrollArea>
#include <QWheelEvent> 
#include "WTextSourceViewerHighlighter.h"


WTextSourceViewerContain::WTextSourceViewerContain( 
    QWidget * parent  
    )  : 
  QTextEdit(parent)
{
  setFrameStyle(QFrame::NoFrame);
  tab_stop_width=0;
  wtextsourceviewer=NULL;
  in_wheelEvent=false;
  _highlighter=new WTextSourceViewerHighlighter();
}

WTextSourceViewerContain::~WTextSourceViewerContain( )
{
  delete _highlighter;
  //if (text_document_p) delete text_document_p;
}

void WTextSourceViewerContain::keyPressEvent ( QKeyEvent * e ) 
{
  QTextEdit::keyPressEvent(e);
}

void WTextSourceViewerContain::zoomIn()
{
  QFont f=font();

  int s=f.pointSize();
  f.setPointSize(s+1);
  setCurrentFont(f);
  emit fontChanged(f);
}

void WTextSourceViewerContain::zoomOut()
{
  QFont f=font();
  int s=f.pointSize();
  if (s>1)
  {
    f.setPointSize(s-1);
    setCurrentFont(f);
    emit fontChanged(f);
  }
}

void WTextSourceViewerContain::wheelEvent ( QWheelEvent * e ) 
{
  if (!in_wheelEvent)
  {
    in_wheelEvent=true;
    if (e->modifiers() & Qt::ControlModifier)
      e->ignore();
    else if (e->modifiers() & Qt::ShiftModifier)
      e->ignore();
    else
      QTextEdit::wheelEvent (e);
  }
  in_wheelEvent=false;
}

void WTextSourceViewerContain::contextMenuEvent(QContextMenuEvent * e) 
{
  QMenu *menu = NULL;
  if (wtextsourceviewer)
  {
    int line,column;
    QTextCursor curs=textCursor();
    column=curs.columnNumber();
    line=curs.blockNumber();
    line++;
    menu = wtextsourceviewer->popupMenu(line,column);
  }
  if (menu)
  {
    menu->exec(e->globalPos());
    delete menu;
  }
}

void WTextSourceViewerContain::setCurrentFont ( const QFont & f )
{
  displayFont=f;
  RTF::setDefaultFormating(displayFont,document());
}

void WTextSourceViewerContain::setDocument(QTextDocument *t)
{
  ASSERT(t);
  if (t)
  {
    QTextDocument *text_document_p=t;
    RTF::setDefaultFormating(displayFont,text_document_p);
    _highlighter->setDocument(text_document_p);
    rehighlight();
    QTextEdit::setDocument(text_document_p);
  }
}


void WTextSourceViewerContain::scrollContentsBy(int dx,int dy)
{
  QTextEdit::scrollContentsBy(dx,dy);
  int x = horizontalScrollBar()->value();
  int y = verticalScrollBar()->value();
  emit contentsMoving(x,y);
}

void WTextSourceViewerContain::setDisplayOffset(int y) 
{
  verticalScrollBar()->setValue(y);
}

int WTextSourceViewerContain::getDisplayOffset() const
{
  return verticalScrollBar()->value();
}


void WTextSourceViewerContain::getSelection(int &start_para,int &start_index,int &end_para,int &end_index) const
{
  int s_sel=textCursor().selectionStart();
  int e_sel=textCursor().selectionEnd();
  QTextCursor s_cur(document()),e_cur(document());
  s_cur.setPosition(s_sel);
  e_cur.setPosition(e_sel);
  start_para=s_cur.blockNumber();
  end_para=e_cur.blockNumber();
  start_index=s_cur.columnNumber();
  end_index=e_cur.columnNumber();
}

void WTextSourceViewerContain::getCursorPosition(int &para,int &index) const
{
  para=textCursor().blockNumber();
  index=textCursor().columnNumber();
}

void WTextSourceViewerContain::setCursorPosition(int para,int index)
{
  QTextCursor cur(document());
  if (para<0)
    cur.movePosition(QTextCursor::End);
  else
  {
    cur.movePosition(QTextCursor::Start);
    cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor,para);
    cur.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,index);
  }
  setTextCursor(cur);
}

void WTextSourceViewerContain::mouseReleaseEvent ( QMouseEvent * event ) 
{
  QTextCursor cur=textCursor();

  QTextEdit::mouseReleaseEvent (event);
  emit instrumentationsSelected();
  cur.clearSelection();
  setTextCursor(cur);
}


void WTextSourceViewerContain::setTabStopWidth(int w)
{
   tab_stop_width=w;
   QTextEdit::setTabStopWidth(tab_stop_width);
}

void WTextSourceViewerContain::rehighlight()
{
   _highlighter->rehighlight();
}
