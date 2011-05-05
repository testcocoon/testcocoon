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

#ifndef WTEXTSOURCEVIEWER_H
#define WTEXTSOURCEVIEWER_H
#include "coveragebrowserpdef.h"
#include <qwidget.h>
#include <qstring.h>
#include <qbitarray.h>
#include <QLabel>
#include <QTextDocument>
#include <QList>
#include "instrumentation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextDocument>
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
class WTextSourceViewerLine ;
class WTextSourceViewerLine ;
class WTextSourceViewerCount ;
class WTextSourceViewerContain ;
#include <QMenu>
class WTextSourceViewerSearch;
#include <QHBoxLayout>

class WTextSourceViewer : public QFrame
{
  Q_OBJECT
  public:
    struct background_t { 
      int line;
      QColor color;
    } ;
    void setCoverageCount(const QVector<int> &min,const QVector<int> &max);
    WTextSourceViewer(QWidget *w=NULL,Qt::WindowFlags f=0);
    virtual ~WTextSourceViewer();
    void setReadOnly(bool b) ;
    QFont currentFont () const ;
    virtual void setCurrentFont ( const QFont & f ) ;
    void getCursorPosition ( int & para, int & index ) const ;
    void setCommentLine(const QList <int> &v);
    void getSelection(int &start_para,int &start_index,int &end_para,int &end_index) const;
    QFont font () const { return currentFont(); }
    virtual void setFont ( const QFont & f ) { setCurrentFont(f); }
    void setDocument(QTextDocument *);
    virtual QMenu *popupMenu(int line, int column);
    void setTabStopWidth ( int width );
    void wheelEvent ( QWheelEvent * e ) ;
    void selectInstrumentations(int start_line,int start_column, int end_line,int end_column);
    QByteArray getColumnState() const ;
    void setColumnState (const QByteArray &);
    void setScrollPos(int);
    int getScrollPos() const;
    bool find ( const QString & exp, QTextDocument::FindFlags options  );
  public slots:
    void setCursorPosition ( int para, int index ) ;
    void ensureCursorVisible () ;
    void selectAll();
  private slots:
    void contentsMoving(int,int);
    void fontTextChanged ( const QFont & f );
    void hideLines(int, const QBitArray &);
    void selectionChanged_event();
    void findText_slot ( const QString & exp, QTextDocument::FindFlags options  );
  signals:
    void clicked();
    void selectionChanged();
    void viewChanged();
    void findText ( const QString & exp, QTextDocument::FindFlags options  );
  private:
    bool ignore_selectionChanged;
    int nb_selected_regions;
    void displayed2originalLine(int &para,int &line) const;
    void original2displayedLine(int &para,int &line) const;
    QVBoxLayout *layout_p;
    QSplitter *splitter_horizontal_p;
    WTextSourceViewerLine *line_p;
    WTextSourceViewerCount *count_p;
    WTextSourceViewerContain *text_p;
    QBitArray  hidden_lines;
    QList<background_t> bg_line_color;
    QFont displayFont;
    QTextDocument *text_document_p;
    int tab_stop_width;
    int number_of_line;
    WTextSourceViewerSearch *search_p;

  protected:
     void toggleShowFindText();
    void rehighlight();
    int getDisplayOffset() const ;
    void setDisplayOffset(int);
} ;
#endif

