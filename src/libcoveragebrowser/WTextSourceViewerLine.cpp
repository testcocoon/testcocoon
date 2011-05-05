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

#include "WTextSourceViewerLine.h"
#include "TextSourceInstrumentationData.h"
#include "rtf.h"
#include <QMenu>
#include <QPolygon>
#include <QTextBlock>
#include <QPainter>
#include <QPaintEvent>
#include <QList>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <math.h>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <math.h>


  WTextSourceViewerLine::WTextSourceViewerLine( 
      QWidget * parent , Qt::WindowFlags f )  : QWidget(parent,f)
{
  displayFont.setBold(true);
  img_comment=QImage(":/images/comments.svg");
  img_comment_scaled=img_comment;
  char_space=1;

  text_document_p=NULL;
  showAllAction = new QAction(tr("Show All"),this);
  showAllAction->setShortcut(Qt::CTRL+Qt::Key_S);
  showAllAction->setStatusTip(tr("Display the complete source code"));
  connect (showAllAction,SIGNAL(triggered()),this,SLOT(showAll()));

  hideAllAction = new QAction(tr("Hide All"),this);
  hideAllAction->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_S);
  hideAllAction->setStatusTip(tr("Display only the instrumented lines"));
  connect (hideAllAction,SIGNAL(triggered()),this,SLOT(hideAll()));

  show_all=true;
  translation_y=0;
}

WTextSourceViewerLine::~WTextSourceViewerLine( )
{
  delete showAllAction;
  delete hideAllAction;
}

void WTextSourceViewerLine::setDocument(int n,const QTextDocument *doc)
{
  text_document_p=doc;
  number_of_lines=n;
  setSizes();
  update();
}

void WTextSourceViewerLine::setContentsPos(int /*x*/,int y)
{
  translation_y=y;
  update();
}



void WTextSourceViewerLine::setFont ( const QFont & f ) 
{
  QFont curfont=f;
  curfont.setBold(false);
  curfont.setItalic(false);
  curfont.setUnderline(false);
  if (curfont!=displayFont)
  {
    displayFont=curfont;
    QFontMetrics fm(displayFont);
    char_space=fm.ascent();
    int img_size=static_cast<int>(char_space);
    img_comment_scaled=img_comment.scaled(img_size,img_size);
    update();
  }
}

void WTextSourceViewerLine::paintEvent ( QPaintEvent * ev_p )
{
  QPainter p(this);
  bool place_for_comment=!commented_lines.isEmpty();

  p.setRenderHint(QPainter::SmoothPixmapTransform );
  p.setRenderHint( QPainter::Antialiasing);
  
  p.setClipRect(ev_p->rect());
  p.translate(1,-translation_y);
  p.setFont(font());
  p.setPen(QPen(Qt::black,0));
  QAbstractTextDocumentLayout::PaintContext context;
  QRect r=ev_p->rect();
  if (text_document_p)
  {
    QRectF lineRect;
    int last_line=0;
    QPointF folding_pos_close;
    QPointF folding_pos_open_start;
    QPointF folding_pos_open_end;
    bool has_folding_close=false;
    bool has_folding_open=false;
    bool last_block_contains_instrumentation=true;
    for (QTextBlock textBlock=text_document_p->begin();
        textBlock!=text_document_p->end();
        textBlock=textBlock.next())
    {
      TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
      if (instrumentationData)
      {
        int current_line = instrumentationData->current_line;
        lineRect=text_document_p->documentLayout()->blockBoundingRect(textBlock);
        QRectF line_text_area(char_space*(2.0f+ (place_for_comment?1:0) ),lineRect.top(),width()-char_space*2.0f,lineRect.height());
        QRectF comment_area(char_space*2,lineRect.top(),img_comment_scaled.width(),img_comment_scaled.height());

        bool instrumentation_point=false;
        for (QVector<TextSourceInstrumentationData::InstrumentationData>::const_iterator it=instrumentationData->instrumentations.begin();it!=instrumentationData->instrumentations.end();++it)
        {
          Instrumentation::status_t st=(*it).status(instrumentationData->coverage_level,instrumentationData->coverage_method);
          if ( st != Instrumentation::STATUS_NONE )
          {
            instrumentation_point=true;
            break;
          }
        }

        if (instrumentation_point)
        {
          QRectF backRect(0,lineRect.top(),width(),lineRect.height());
          p.drawText(line_text_area,Qt::AlignVCenter|Qt::AlignLeft,QString::number(current_line));
          last_block_contains_instrumentation=true;
          if (has_folding_open)
            drawFoldingLineVisible(p,folding_pos_open_start,folding_pos_open_end);
          bool current_line_has_comment=commented_lines.contains(current_line);
          if (current_line_has_comment)
            p.drawImage(comment_area,img_comment_scaled);
          if (last_line+1!=current_line)
          {
            if (has_folding_close)
              drawFoldingLineHidden(p,folding_pos_close);
            if (current_line_has_comment || !place_for_comment)
              folding_pos_close.setX(char_space*1.5f);
            else
              folding_pos_close.setX(char_space*2.5f);
            folding_pos_close.setY((lineRect.bottom()+lineRect.top())/2);
            drawFoldingLineHidden(p,folding_pos_close);
          }
          else
          {
            if (current_line_has_comment || !place_for_comment)
              folding_pos_close.setX(char_space*1.5f);
            else
              folding_pos_close.setX(char_space*2.5f);
            folding_pos_close.setY((lineRect.bottom()+lineRect.top())/2);
          }
          has_folding_open=false;
          has_folding_close=true;
        }
        else
        {
          if ( (has_folding_close) && (last_line+1!=current_line) )
            drawFoldingLineHidden(p,folding_pos_close);
          if (place_for_comment)
             folding_pos_open_end.setX(char_space*2.5f);
          else
             folding_pos_open_end.setX(char_space*1.5f);
          folding_pos_open_end.setY((lineRect.bottom()+lineRect.top())/2);
          if (last_block_contains_instrumentation)
            folding_pos_open_start=folding_pos_open_end;

          p.drawText(line_text_area,Qt::AlignVCenter|Qt::AlignLeft,QString::number(current_line));
          last_block_contains_instrumentation=false;
          has_folding_close=false;
          has_folding_open=true;
        }
        last_line=current_line;
      }
    }
    if (has_folding_close)
      drawFoldingLineHidden(p,folding_pos_close);
    if (has_folding_open)
      drawFoldingLineVisible(p,folding_pos_open_start,folding_pos_open_end);
  }
  r.translate(-1,+translation_y);
  context.clip=r;
}


void WTextSourceViewerLine::drawFoldingLineVisible(QPainter &p,const QPointF &p1,const QPointF &p2)
{
  QPolygonF area;
  float demi_char_space=char_space/2.0f;
  QPointF pos(0.0f,(p1.y()+p2.y())/2.0f-demi_char_space);
  QRectF  text_pos(pos,QSizeF(char_space,char_space));

  QPointF p3(demi_char_space,p2.y());
  QRectF endPoint2(p2.x()-2,p2.y()-2,4,4);
  QBrush b=p.brush();
  p.setBrush(Qt::black);
  if (p1!=p2)
  {
    QPointF p0(demi_char_space,p1.y());
    QPointF p1b(demi_char_space,pos.y());
    QPointF p2b(demi_char_space,pos.y()+char_space);
    QRectF endPoint1(p1.x()-2,p1.y()-2,4,4);
    area << p0 << p1 ;
    area << p0 << p1b ;
    area << p2b << p3 ;
    area << p3 << p2 ;
    p.drawEllipse(endPoint1);
  }
  else
  {
    QPointF p1c(char_space,p2.y());
    area << p1c << p2 ;
  }

  p.drawEllipse(endPoint2);
  p.setBrush(b);
  p.drawLines(area);
  float pos_size=char_space;
  QRectF rectangle(pos.x(),pos.y(),pos_size,pos_size);
  p.drawRoundRect(rectangle);
  float _x1=pos.x()+3.0f;
  float _y=pos.y()+pos_size/2.0f;
  float _x2=pos.x()+pos_size-3.0f;
  p.drawLine(QPointF(_x1,_y),QPointF(_x2,_y));
}


void WTextSourceViewerLine::drawFoldingLineHidden(QPainter &p,const QPointF &pt)
{
  float demi_char_space=char_space/2.0f;
  QPointF pos(0,pt.y()-demi_char_space);
  QRectF  text_pos(pos,QSizeF(char_space,char_space));

  QPointF p3(pt.x(),pt.y());
  QPointF p1c(char_space,pt.y());
  QBrush b=p.brush();
  p.setBrush(Qt::black);
  QRectF endPoint3(p3.x()-2,p3.y()-2,4,4);

  p.drawEllipse(endPoint3);
  p.setBrush(b);
  p.drawLine(p1c,p3);
  float pos_size=char_space;
  QRectF rectangle(pos.x(),pos.y(),pos_size,pos_size);
  p.drawRoundRect(rectangle);
  float _x1=pos.x()+3.0f;
  float _y=pos.y()+pos_size/2.0f;
  float _x2=pos.x()+pos_size-3.0f;
  float _x=pos.x()+pos_size/2;
  float _y1=pos.y()+3;
  float _y2=pos.y()+pos_size-3;
  p.drawLine(QPointF(_x1,_y),QPointF(_x2,_y));
  p.drawLine(QPointF(_x,_y1),QPointF(_x,_y2));
}


void WTextSourceViewerLine::setSizes()
{
  QFontMetrics fm(displayFont);
  char_space=fm.ascent();
  bool has_comment = ! commented_lines.isEmpty();

  int c_space=static_cast<int>(char_space)+1;
  int c_digit=static_cast<int>(fm.width('M'))+1;
  QSize sz(0,minimumSize().height());
  int n_space=2;
  if (has_comment)
    n_space++;
  int n_digit=1;
  if (number_of_lines>0)
  {
     int l=number_of_lines;
     while (l>0)
     {
        n_digit++;
        l=l/10;
     }
  }
  sz.setWidth(c_digit*n_digit+c_space*n_space);

  if (minimumSize().width()!=sz.width())
    setMinimumSize(QSize(sz.width(),minimumSize().height()));
  if (maximumSize().width()!=sz.width())
    setMaximumSize(QSize(sz.width(),maximumSize().height()));
}

void WTextSourceViewerLine::mousePressEvent ( QMouseEvent * e )
{
  QRectF lineRect;
  QPoint pos=e->pos();
  if (e->button() != Qt::LeftButton)
     return;
  pos.setY(pos.y()+translation_y);
  int i;
  QBitArray hidden_lines;
  hidden_lines.resize(number_of_lines);
  for (i=0;i<number_of_lines;i++) hidden_lines[i]=true;
  int current_line=-1;
  int block_end=-1;
  int block_start=-1;
  bool hide_current_line=true;
  bool found=false;
  for (QTextBlock textBlock=text_document_p->begin();
      textBlock!=text_document_p->end();
      textBlock=textBlock.next())
  {
    TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
    if (instrumentationData)
    {
      lineRect=text_document_p->documentLayout()->blockBoundingRect(textBlock);
      QRectF hitRect(0,lineRect.y(),width(),lineRect.height());
      hidden_lines[instrumentationData->current_line-1]=false;
      if (hitRect.contains(pos))
      {
        current_line = instrumentationData->current_line;
        hide_current_line = (instrumentationData->instrumentations.isEmpty()) ;
        if (hide_current_line)
          found=true;
      }
      if (instrumentationData->instrumentations.isEmpty()) 
      {
        block_end=instrumentationData->current_line;
        if (block_start==-1)
          block_start=instrumentationData->current_line;
      }
      else
      {
        if (!hide_current_line)
          found=false;
        if (found)
        {
          found=false;
          for (i=block_start-1;i<=block_end-1;i++)
            hidden_lines[i]=hide_current_line;
        }
        block_end=-1;
        block_start=-1;
      }
    }
  }
  if (found)
  {
    found=false;
    for (i=block_start-1;i<=block_end-1;i++)
      hidden_lines[i]=hide_current_line;
  }
  if (current_line<0)
  {
    e->accept();
    return;
  }
  
  int scroll_to_line=current_line;
  if (!hide_current_line)
  {
    int i;
    for (i=current_line;i<number_of_lines && hidden_lines[i];i++)
      hidden_lines[i]=false;
    for (i=current_line-2;i>=0 && hidden_lines[i];i--)
      hidden_lines[i]=false;
  }
  else
  {
    for (int i=current_line;i<number_of_lines && hidden_lines[i];i++)
       scroll_to_line=i+2;
  }

  emit hideLines(scroll_to_line,hidden_lines);
  e->accept();
}

void WTextSourceViewerLine::contextMenuEvent( QContextMenuEvent * e)
{
  QMenu * popup_p=new QMenu(this);
  popup_p->addAction(showAllAction);
  popup_p->addAction(hideAllAction);
  popup_p->exec(QCursor::pos());
  delete popup_p;
  e->accept();
  update();
}


void WTextSourceViewerLine::showAll()
{
  QBitArray hidden_lines;
  hidden_lines.resize(number_of_lines);
  for (int i=0;i<number_of_lines;i++) 
    hidden_lines[i]=false;
  emit hideLines(1,hidden_lines);
}

void WTextSourceViewerLine::hideAll()
{
  QBitArray hidden_lines;
  hidden_lines.resize(number_of_lines);

  for (int i=0;i<number_of_lines;i++) 
    hidden_lines[i]=true;

  for (QTextBlock textBlock=text_document_p->begin();
      textBlock!=text_document_p->end();
      textBlock=textBlock.next())
  {
    TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
    if (instrumentationData && !instrumentationData->instrumentations.isEmpty())
      hidden_lines[instrumentationData->current_line-1]=false;
  }

  emit hideLines(1,hidden_lines);
}

void WTextSourceViewerLine::setCommentLine(const QList <int> &v)
{
  commented_lines=v;
}
