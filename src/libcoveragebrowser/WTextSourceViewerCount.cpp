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

#include "WTextSourceViewerCount.h"
#include "rtf.h"
#include <QPainter>
#include <qsplitter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QPaintEvent>
#include <QList>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <math.h>
#include <qcursor.h>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include "TextSourceInstrumentationData.h"

WTextSourceViewerCount::WTextSourceViewerCount( 
    QWidget * parent , Qt::WindowFlags f )  : QWidget(parent,f)
{
  translation_y=0;
  show_all=true;
  text_document_p=NULL;
}

WTextSourceViewerCount::~WTextSourceViewerCount( )
{
}

void WTextSourceViewerCount::setCoverageCount(const QVector<int> &min,const QVector<int> &max)
{
  ASSERT(coverage_min.size()==coverage_max.size());
  coverage_min=min;
  coverage_max=max;

  setSizes();
  update();
}

void WTextSourceViewerCount::setDocument(const QTextDocument *doc)
{
  text_document_p=doc;
  setSizes();
}

void WTextSourceViewerCount::setContentsPos(int /*x*/,int y)
{
  translation_y=y;
  update();
}

void WTextSourceViewerCount::setFont ( const QFont & f ) 
{
  QFont curfont=f;
  curfont.setBold(false);
  curfont.setItalic(false);
  curfont.setUnderline(false);
  if (curfont!=font())
  {
    displayFont=curfont;
    setSizes();
  }
}

void WTextSourceViewerCount::paintEvent ( QPaintEvent * ev_p )
{
  QPainter p(this);
  p.setRenderHint(QPainter::SmoothPixmapTransform );
  p.setRenderHint( QPainter::Antialiasing);

  p.setClipRect(ev_p->rect());
  p.translate(0,-translation_y);
  p.setFont(font());
  QAbstractTextDocumentLayout::PaintContext context;
  QRect r=ev_p->rect();
  if (text_document_p)
  {
    int i=1;
    QRectF lineRect;
    for (QTextBlock textBlock=text_document_p->begin();
        textBlock!=text_document_p->end();
        textBlock=textBlock.next())
    {
      TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
      if (instrumentationData)
      {
        if (!instrumentationData->instrumentations.isEmpty())
        {
          i = instrumentationData->current_line;

          lineRect=text_document_p->documentLayout()->blockBoundingRect(textBlock);
          QRectF line_text_area(0,lineRect.top(),width(),lineRect.height());
          if (i>=coverage_min.count() || coverage_min.at(i)<0 || coverage_max.at(i)<0)
            p.drawText(line_text_area,Qt::AlignVCenter|Qt::AlignLeft,"-");
          else if (coverage_min.at(i) == coverage_max.at(i))
            p.drawText(line_text_area,Qt::AlignVCenter|Qt::AlignLeft,QString::number(coverage_min.at(i)));
          else
            p.drawText(line_text_area,Qt::AlignVCenter|Qt::AlignLeft,QString::number(coverage_min.at(i))+"-"+QString::number(coverage_max.at(i)));
        }
      }

      i++;
    }
  }
  r.translate(0,+translation_y);
  context.clip=r;
}


void WTextSourceViewerCount::setSizes()
{
  int number_of_lines=coverage_min.size();
  if (number_of_lines<=0)
    return ;
  QFontMetrics fm(font());
  QSize sz=fm.size(Qt::TextSingleLine,"-");
  for (int i=1;i<number_of_lines;i++)
  {
    QString text;
    if (i>=coverage_min.count() || coverage_min.at(i)<0 || coverage_max.at(i)<0)
      ;
    else if (coverage_min.at(i) == coverage_max.at(i))
    {
      text= QString::number(coverage_min.at(i));
    }
    else
    {
      text=QString::number(coverage_min.at(i))+"-"+QString::number(coverage_max.at(i));
    }
    if (!text.isEmpty())
    {
      QSize sz_text=fm.size(Qt::TextSingleLine,text);
      if (sz_text.width()>sz.width())
        sz.setWidth(sz_text.width());
    }
  }

  if (minimumSize().width()!=sz.width())
    setMinimumSize(QSize(sz.width(),minimumSize().height()));
  if (maximumSize().width()!=sz.width())
    setMaximumSize(QSize(sz.width(),maximumSize().height()));
}
