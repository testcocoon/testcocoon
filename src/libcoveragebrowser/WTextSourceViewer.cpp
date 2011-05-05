/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
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

#include "WTextSourceViewer.h"
#include "WTextSourceViewerLine.h"
#include "WTextSourceViewerCount.h"
#include "WTextSourceViewerContain.h" 
#include "rtf.h"
#include "options.h"
#include <QApplication>
#include <QSplitter>
#include <QtDebug>
#include <QLabel>
#include <QScrollBar>
#include <QLayout>
#include <QTextEdit>
#include <QList>
#include <QVBoxLayout>
#include <QTextDocument>
#include "TextSourceInstrumentationData.h"
#include "WTextSourceViewerSearch.h"


WTextSourceViewer::WTextSourceViewer(QWidget *w,Qt::WindowFlags f) : QFrame(w,f)
{
  text_document_p=NULL;
  tab_stop_width=0;
  QFont displayFont;
  ignore_selectionChanged=false;
  displayFont.fromString(Options::get_opt_str(QString(),"FONT_SOURCE_CODE",DEF_FONT_SOURCE_CODE));
  nb_selected_regions=0;
  layout_p=new QVBoxLayout(this);

  splitter_horizontal_p=new QSplitter(this);
  splitter_horizontal_p->setOrientation(Qt::Horizontal);
  line_p=new WTextSourceViewerLine (splitter_horizontal_p);
  text_p=new WTextSourceViewerContain (splitter_horizontal_p);
  count_p=new WTextSourceViewerCount (splitter_horizontal_p);

  line_p->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
  count_p->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
  text_p->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding));
  setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
  search_p=new WTextSourceViewerSearch(this);
  layout_p->addWidget(splitter_horizontal_p);
  layout_p->addWidget(search_p);
  text_p->setFocusProxy(search_p);

  connect (search_p, SIGNAL( findText ( const QString & , QTextDocument::FindFlags )), this, SLOT( findText_slot ( const QString & , QTextDocument::FindFlags )));
  connect (text_p,SIGNAL(instrumentationsSelected()),this,SLOT(selectionChanged_event()),Qt::DirectConnection);
  connect (text_p,SIGNAL(contentsMoving(int,int)) ,this,SLOT(contentsMoving(int,int)));
  connect (text_p,SIGNAL(fontChanged ( const QFont & )) ,this,SLOT(fontTextChanged ( const QFont & )));
  connect(line_p,SIGNAL(hideLines(int,const QBitArray &)),
        this,SLOT(hideLines(int,const QBitArray &)));
  text_p->setWTextSourceViewer(this);
  text_p->setTabChangesFocus(true);
  setCurrentFont(displayFont);
}

WTextSourceViewer::~WTextSourceViewer()
{
  delete line_p;
  delete count_p;
  delete text_p;
  delete search_p;
  delete splitter_horizontal_p;
  delete layout_p;
}

void WTextSourceViewer::selectionChanged_event()
{
  if (ignore_selectionChanged)
    return;
  int start_para,end_para,start_index,end_index;
  text_p->getSelection(start_para,start_index,end_para,end_index);
  if (start_para<0 || end_para<0 || (start_para==end_para && start_index==end_index) )
  {
    if (hasFocus() || testAttribute(Qt::WA_UnderMouse))
    {
      emit clicked();
    }
  }
  else
  {
     if (QApplication::mouseButtons()==Qt::NoButton)
        emit selectionChanged();
  }
}

void WTextSourceViewer::setCoverageCount(const QVector<int> &min,const QVector<int> &max)
{
  count_p->setCoverageCount(min,max);
}

void WTextSourceViewer::hideLines (int current,const QBitArray &v)
{
  int line=current;
  int column=1;
  hidden_lines=v;
  if (!text_document_p)
    return ;
  
  QTextDocument *text_document_hl_p=NULL;
  RTF::hideLines(hidden_lines,text_document_p,text_document_hl_p);
  text_document_hl_p->setParent(text_p);
  text_p->setDocument(text_document_hl_p);
  text_p->setTabStopWidth(tab_stop_width);
  count_p->setDocument(text_document_hl_p);
  line_p->setDocument(text_document_p->blockCount(),text_document_hl_p);
  if (line>=0)
  {
    original2displayedLine(line,column);
    text_p->setCursorPosition(line,column);
    text_p->ensureCursorVisible();
  }
  emit viewChanged();
}

void WTextSourceViewer::setDocument(QTextDocument *t)
{
  text_document_p=t;
  number_of_line=text_document_p->blockCount();
  text_p->setDocument(text_document_p);
  text_p->setTabStopWidth(tab_stop_width);
  int l=text_document_p->blockCount();
  hidden_lines.resize(l);
  for (int i=0;i<l;i++)
    hidden_lines[i]=false;
  line_p->setDocument(l,text_document_p);
  count_p->setDocument(text_document_p);
}

QMenu *WTextSourceViewer::popupMenu(int /*line*/, int /*index*/)
{
  return NULL;
}

void WTextSourceViewer::setCommentLine(const QList <int> &v)
{
  line_p->setCommentLine(v);
}

void WTextSourceViewer::setCursorPosition ( int para, int index ) 
{
  if (para>0)
    original2displayedLine(para,index);
  text_p->setCursorPosition(para,index);
}

void WTextSourceViewer::ensureCursorVisible () 
{
  int para,pos;
  text_p->getCursorPosition(para,pos);
  text_p->setCursorPosition(para+1,pos);
  text_p->ensureCursorVisible () ;
  text_p->setCursorPosition(para,pos);
}

void WTextSourceViewer::setReadOnly(bool b) 
{
  text_p->setReadOnly(b);
}

QFont WTextSourceViewer::currentFont () const 
{
  return text_p->currentFont();
}

void WTextSourceViewer::setCurrentFont ( const QFont & f ) 
{
  text_p->setCurrentFont(f);
  line_p->setFont(f);
  count_p->setFont(f);
}

void WTextSourceViewer::getCursorPosition ( int &para, int  &index ) const 
{
  text_p->getCursorPosition(para,index);
  displayed2originalLine(para,index);
}

void WTextSourceViewer::selectAll()
{
  text_p->selectAll();
}

void WTextSourceViewer::contentsMoving(int /*x*/,int y)
{
  line_p->setContentsPos(0,y+text_p->frameWidth());
  count_p->setContentsPos(0,y+text_p->frameWidth());
}

void WTextSourceViewer::fontTextChanged ( const QFont & f )
{
  setCurrentFont(f);
}

void WTextSourceViewer::displayed2originalLine(int &para_line,int &index_col) const
{
  int sz=hidden_lines.size();
  int line_show_id=0;
  int orig_line_id;
  for (orig_line_id=0 ;line_show_id<=para_line && orig_line_id<sz;orig_line_id++)
  {
    if (!hidden_lines[orig_line_id])
      line_show_id++;
  }
  para_line=orig_line_id;
  index_col++;
}

void WTextSourceViewer::original2displayedLine(int &para_line,int &index_col) const
{
  ASSERT(para_line>0);
  ASSERT(index_col>0);
  para_line--;
  index_col--;
  if (index_col<=-1) index_col=0;
  int sz=hidden_lines.size();
  if (para_line>=sz || hidden_lines[para_line])
  {
    para_line=-1;
    return ;
  }
  int displayed_line=0;
  for (int i=0;i<para_line && i<sz;i++)
  {
    if (!hidden_lines[i])
      displayed_line++;
  }
  para_line= displayed_line;
}


void WTextSourceViewer::selectInstrumentations(int start_line,int start_column, int end_line,int end_column)
{
  QVector<TextSourceInstrumentationData*> instrumentationDatas;
  instrumentationDatas.resize(number_of_line+1);
  QTextBlock textBlock;
  for (QVector<TextSourceInstrumentationData*>::Iterator instrumentationDatasIt = instrumentationDatas.begin(); instrumentationDatasIt != instrumentationDatas.end(); ++instrumentationDatasIt )
    *instrumentationDatasIt=NULL;
    
  for (textBlock = text_p->document()->begin(); textBlock!=text_p->document()->end() ;textBlock=textBlock.next())
  {
    TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
    if (instrumentationData)
    {
      ASSERT(instrumentationDatas.size()>instrumentationData->current_line);
      if (instrumentationDatas.size()<=instrumentationData->current_line)
        instrumentationDatas.resize(instrumentationData->current_line+1);
      instrumentationDatas[instrumentationData->current_line]=instrumentationData;
      for (QVector<TextSourceInstrumentationData::InstrumentationData>::Iterator it=instrumentationData->instrumentations.begin(); it!=instrumentationData->instrumentations.end();++it)
      {
        bool selected;
        if (instrumentationData->line_mode)
          selected=(*it).isInRangeOrg(start_line,start_column, end_line,end_column);
        else
          selected=(*it).isInRangePre(start_line,start_column, end_line,end_column);
        if ( (*it).selected()!=selected )
        {
          (*it).setSelected(selected); 
          Instrumentation::status_t status= (*it).status(instrumentationData->coverage_level,instrumentationData->coverage_method);
          if (status!=Instrumentation::STATUS_NONE)
          {
            QBrush background_format=Instrumentation::statusBackgroundColor(status,selected);
            QTextCharFormat f;
            f.setBackground(background_format);

            if (instrumentationData->line_mode)
            {
              QTextCursor cur(textBlock);
              cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
              cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
              cur.mergeCharFormat(f);
            }
            else
            {
              int start_column;
              int end_column;
              int current_line=instrumentationData->current_line;
              if ((*it).startLinePre()!=current_line)
                start_column=0;
              else
                start_column=(*it).startColumnPre()-1;

              if (current_line!=(*it).endLinePre())
                end_column=textBlock.text().length()-start_column;
              else
                end_column=(*it).endColumnPre()-1;

              QTextCursor cur(textBlock);
              cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
              cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,start_column);
              cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::KeepAnchor);
              cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,end_column);
              cur.mergeCharFormat(f);
            }
          }
        }
      }
    }
  }
  
  //rehighlight();
}

void WTextSourceViewer::rehighlight()
{
  text_p->rehighlight();
}

void WTextSourceViewer::getSelection(int &start_para,int &start_index,int &end_para,int &end_index) const
{
   text_p->getSelection(start_para,start_index,end_para,end_index) ;
   displayed2originalLine(start_para,start_index);
   displayed2originalLine(end_para,end_index);
}


void WTextSourceViewer::setTabStopWidth ( int width )
{
  tab_stop_width=width;
  text_p->setTabStopWidth (width);
}


void WTextSourceViewer::wheelEvent ( QWheelEvent * e ) 
{
  text_p->wheelEvent (  e ) ;
}

void WTextSourceViewer::setDisplayOffset(int y) 
{
  text_p->setDisplayOffset(y);
}

int WTextSourceViewer::getDisplayOffset() const
{
  return text_p->getDisplayOffset();
}

QByteArray WTextSourceViewer::getColumnState() const 
{
  return splitter_horizontal_p->saveState();
}

void    WTextSourceViewer::setColumnState (const QByteArray &s)
{
  splitter_horizontal_p->restoreState(s);
}

void     WTextSourceViewer::setScrollPos(int p)
{
  text_p->verticalScrollBar()->setValue(p);
}

int WTextSourceViewer::getScrollPos() const
{
  return text_p->verticalScrollBar()->value();
}

bool WTextSourceViewer::find ( const QString & exp, QTextDocument::FindFlags options  )
{
  return  text_p->find(exp,options);
}

void WTextSourceViewer::findText_slot ( const QString & exp, QTextDocument::FindFlags options  )
{
   emit findText(exp,options);
}

void WTextSourceViewer::toggleShowFindText()
{
   search_p->toggleVisible();
}
