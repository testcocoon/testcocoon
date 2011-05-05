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

#ifndef WSOURCEVIEW_H
#define WSOURCEVIEW_H

#include "coveragebrowserpdef.h"
#include <QList>
#include <QWheelEvent>
#include <QTimer>
#include <QTextDocument> 
#include <QCache>
#include "csmesundoredoframework.h"
#include "WTextSourceViewer.h"

#include <QWorkspace>


class WSourceView: public WTextSourceViewer
{
    Q_OBJECT

public:
    WSourceView( QWidget *parent = 0);
    virtual ~WSourceView( );
    void emitAllSignals();
    QString getModule() const { return module; }
    QString getSource() const { return source; }
    CSMesUndoRedoFramework::source_type_t getViewType() { return source_type; }
public slots:
    void refreshView();
    void setCSMes(CSMesUndoRedoFramework*);
    void setSource(const QString &mod,const QString &src,CSMesUndoRedoFramework::source_type_t t,int start_line,int start_column,int end_line,int end_column);
    void previous(Instrumentation::filter_t status);
    void next(Instrumentation::filter_t status);
    void setSearchFilter(Instrumentation::filter_t);
    void firstInstrumentation(Instrumentation::filter_t t);
    void lastInstrumentation(Instrumentation::filter_t t);
    void zoomIn();
    void zoomOut();
signals:
    void updateViewType(CSMesUndoRedoFramework::source_type_t);
    void setExplanationOriginal(const QString &module,const QString &source,const QList<int>& line,int coverage_level,Instrumentation::coverage_method_t ,int );
    void setExplanationPreprocessed(const QString &module,const QString &source,const QList<int>& index,int coverage_level,Instrumentation::coverage_method_t,int  );
    void editPossibilities(bool set_comment,bool clear_comment, bool set_manually_validated, bool clear_manually_validated, bool preprocessor_view);
    void requestNextModule(Instrumentation::filter_t);
    void requestPreviousModule(Instrumentation::filter_t);
    void requestFindTextModule ( const QString & exp, QTextDocument::FindFlags options  );
public slots:
    void setComment();
    void clearComment();
    void setManuallyValidated();
    void clearManuallyValidated();
    void saveAs();
    void showFindText();
protected slots:
    void textClicked();
    void selectionChanged_delayed();
    void selectionChanged_received();
    void viewChanged_received();
    void updateView();
    void findText_slot ( const QString & exp, QTextDocument::FindFlags options  );
public:
    QMenu *popupMenu(int line, int column);
private:
    bool _line_mode;
    bool lineMode() const;
    void insertInstrumentationData(QTextDocument &) const;
    void differenceWithReference(const QString &module,const QString &source,QTextDocument &) const;
    int number_of_lines;
    bool hasComments() const;
    bool hasValidated() const;
    bool hasNoValidated() const;
    bool set_comment();
    void emitPossibilities();
    CSMesUndoRedoFramework *csmes_p;
    CSMesUndoRedoFramework::source_type_t source_type;
    QString module,source;
    void gotoNextInstrumentation(int direction,Instrumentation::filter_t status);
    int findInstrumentation(int line, int column,int direction ,Instrumentation::filter_t filter);
    QList<int> findInstrumentationList(int s_line, int s_column,int e_line, int e_column) const;
    void markInstrumentation(int index);
    const Instrumentation* instrumentation(int index);
    Instrumentation::filter_t filter;
    void set_validated_flag(bool v);
    void setExplanation (const QList<int> v );
    QList<int> explanation_item;
protected:
    virtual void wheelEvent ( QWheelEvent * e ) ;
private:
    QTimer updateselection_timer;
    QAction *setCommentAction;
    QAction *clearCommentAction;
    QAction *markAsValidatedAction;
    QAction *clearValidationFlagAction;
    QAction *saveAsAction;
private:
    QTextDocument text_document;
    bool update_under_process;
    int selection_start_line;
    int selection_end_line;
    int selection_start_column;
    int selection_end_column;
};

#endif
