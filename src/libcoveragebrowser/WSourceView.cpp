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

#include "WSourceView.h"
#include "CoverageSettings.h"
#include "TextSourceInstrumentationData.h"
#include "options.h"
#include "diffalgo.h"
#include <qpixmap.h>
#include <QFontMetrics>
#include <QTextCharFormat>
#include <qaction.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <QList>
#include <QMenu>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include "csmesundoredoframework.h"
#include "rtf.h"
#include "Service.h"
#include "WComment.h"
#include "csmesbackgroundcomputations.h"
#include "helpclient.h"
#include <QTimer>
#include <QTime>

WSourceView::WSourceView( QWidget *parent ) : WTextSourceViewer( parent )
{
  csmes_p=NULL;
  _line_mode=true;
  selection_start_line=-1;
  selection_start_column=-1;
  selection_end_line=-1;
  selection_end_column=-1;
  connect (this, SIGNAL( findText ( const QString & , QTextDocument::FindFlags )), this, SLOT( findText_slot ( const QString & , QTextDocument::FindFlags )));

  update_under_process=false;
  setCommentAction = new QAction(tr("Add/Set Comment"),this);
  setCommentAction->setShortcut(Qt::CTRL+Qt::Key_K);
  setCommentAction->setIcon(QIcon(":/images/comments.svg"));
  connect (setCommentAction,SIGNAL(triggered()),this,SLOT(setComment()));

  clearCommentAction = new QAction(tr("Clear Comments"),this);
  clearCommentAction->setShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_K);
  clearCommentAction->setIcon(QIcon(":/images/no_comments.svg"));
  connect (clearCommentAction,SIGNAL(triggered()),this,SLOT(clearComment()));

  markAsValidatedAction = new QAction(tr("Mark as validated"),this);
  markAsValidatedAction->setShortcut(Qt::CTRL+Qt::Key_W);
  markAsValidatedAction->setIcon(QIcon(":/images/validation.svg"));
  connect (markAsValidatedAction,SIGNAL(triggered()),this,SLOT(setManuallyValidated()));

  clearValidationFlagAction = new QAction(tr("Clear validation flag"),this);
  clearValidationFlagAction->setShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_W);
  clearValidationFlagAction->setIcon(QIcon(":/images/no_validation.svg"));
  connect (clearValidationFlagAction,SIGNAL(triggered()),this,SLOT(clearManuallyValidated()));

  saveAsAction = new QAction(tr("Save as..."),this);
  connect (saveAsAction,SIGNAL(triggered()),this,SLOT(saveAs()));

  number_of_lines=0;
  module.clear();
  source.clear();
  updateselection_timer.setSingleShot(true);
  connect(&updateselection_timer,SIGNAL(timeout()),this,SLOT(selectionChanged_received()));
  emitPossibilities();
  setWindowIcon( QPixmap(":/images/testcocoon.svg" ) );
  QPalette palette;
  palette.setColor(backgroundRole(), COL_WHITE);
  setPalette(palette);
  filter=Instrumentation::FILTER_NONE;
  setWindowTitle( tr("<no file>")  );
  setAttribute(Qt::WA_DeleteOnClose);
  setReadOnly(true);
  source_type = CSMesUndoRedoFramework::SOURCE_UNDEF;
  connect (this,SIGNAL(clicked()),this,SLOT(textClicked()));
  connect (this,SIGNAL(selectionChanged()),this,SLOT(selectionChanged_delayed()));
  connect (this,SIGNAL(viewChanged()),this,SLOT(viewChanged_received()));
  QFontMetrics fm(font());
  setTabStopWidth(tabSize()*fm.width(' '));
  setMinimumSize(QSize(20*fm.width('M'),4*fm.lineSpacing()));
  connect(&CoverageSettings::object(),SIGNAL(modified()),
          this,SLOT(refreshView()));
}

WSourceView::~WSourceView()
{
  delete saveAsAction;
  delete setCommentAction;
  delete clearCommentAction;
  delete markAsValidatedAction;
  delete clearValidationFlagAction;
}

void WSourceView::emitAllSignals()
{
  if (source_type!=CSMesUndoRedoFramework::SOURCE_UNDEF)
    updateViewType(source_type);
  emitPossibilities();
}

void WSourceView::updateView()
{
  if (update_under_process)
    return;
  update_under_process=true;
  number_of_lines=0;
  if (csmes_p!=NULL)
  {
    setUpdatesEnabled(false);
    int display_offset=getDisplayOffset();
    QString txt;
    ModuleFile mod;
    SourceFile src;
    ModuleFile mod_ref;
    SourceFile src_ref;
    csmes_p->equivalentModulesReference(module,source,mod,src,mod_ref,src_ref) ;
    csmes_p->toPLAIN(mod,src,source_type,txt);
    _line_mode=lineMode();
    text_document.setPlainText(txt);
    insertInstrumentationData(text_document);
    differenceWithReference(module,source,text_document);
    RTF::setDefaultFormating(font(),&text_document );
    QFontMetrics fm(font());
    setTabStopWidth(tabSize()*fm.width(' '));
    setDocument(&text_document);
    setSearchFilter(filter);
    QVector <int> coverage_min,coverage_max;
    if (source_type==CSMesUndoRedoFramework::CSMES)
      csmes_p->linesCoverageCountPre(mod,src,coverage_min,coverage_max);
    else
      csmes_p->linesCoverageCountOrg(mod,src,coverage_min,coverage_max);
    setCoverageCount(coverage_min,coverage_max);
    setDisplayOffset(display_offset);
    if (selection_start_line>=0 && selection_end_line>=0)
    {
      setCursorPosition(selection_start_line, 1);
      selectInstrumentations(selection_start_line,selection_start_column,selection_end_line,selection_end_column);
      setExplanation(findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) );
    }

    setUpdatesEnabled(true);
  }
  emitPossibilities();
  update_under_process=false;
}

void WSourceView::setCSMes(CSMesUndoRedoFramework *m)
{
  csmes_p=m;
  if (source_type==CSMesUndoRedoFramework::SOURCE_UNDEF)
    source_type=CoverageSettings::object().getSourceType();
  setSource(module,source,source_type,-1,-1,-1,-1);
}

void WSourceView::setSource(const QString &mod,const QString &src,CSMesUndoRedoFramework::source_type_t t,int start_line,int start_column,int end_line,int end_column)
{
  if ( ! (module==mod && source==src) )
    setDisplayOffset(0);
  bool modified=false;
  if (module!=mod) modified=true;
  module=mod;
  if (source!=src) modified=true;
  source=src;
  if (source_type!=t) modified=true;
  source_type = t;

  if (start_line>=0 && end_line>=0)
  {
    if (selection_start_line!=start_line) modified=true;
    selection_start_line=start_line;
    if (selection_end_line!=end_line ) modified=true;
    selection_end_line=end_line ;
    if (start_column>=0 && end_column>=0)
    {
      if (selection_start_column!=start_column) modified=true;
      selection_start_column=start_column;
      if (selection_end_column!=end_column) modified=true;
      selection_end_column=end_column;
    }
    else
    {
      if (selection_start_column!=0) modified=true;
      selection_start_column=0;
      if (selection_end_column!=65635) modified=true;
      selection_end_column=65635;
    }
  }
  else
  {
    if (modified)
    {
      selection_start_line=-1;
      selection_start_column=-1;
      selection_end_line=-1;
      selection_end_column=-1;
    }
  }

  if (modified)
  {
    if (src!=QString::null && src!="")
      setWindowTitle(src);
    else
      setWindowTitle(mod);

    if (t!=CSMesUndoRedoFramework::SOURCE_UNDEF)
      updateView();
    emitPossibilities();
  }
}


void WSourceView::selectionChanged_delayed ()
{
  updateselection_timer.start(UPDATE_SELECTION_TIMER);
  getSelection(selection_start_line,selection_start_column,selection_end_line,selection_end_column);
}


void WSourceView::selectionChanged_received ()
{
  updateselection_timer.stop();
  selectInstrumentations(selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  setExplanation(findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) );
  emitPossibilities();
}

void WSourceView::setExplanation (const QList<int> explanation_item )
{
   int executed_by_limit = Options::get_opt_long(QString(),"EXECUTED_BY_LIMIT",DEF_EXECUTED_BY_LIMIT);
   if (_line_mode)
   {
      QList<int> explanation_line;
      for (QList<int>::const_iterator itInst=explanation_item.begin();itInst!=explanation_item.end();++itInst)
      {
         const QVector<Instrumentation> &inst=csmes_p->instrumentationList(module,source);
         int line=inst.at(*itInst).startLineOrg();
         if (!explanation_line.contains(line))
            explanation_line.append(line);
      }

      emit setExplanationOriginal(module,source,explanation_line,CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),executed_by_limit);
   }
   else
   {
      emit setExplanationPreprocessed(module,source,explanation_item,CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod(),executed_by_limit);
   }
}

void WSourceView::viewChanged_received ()
{
  selectInstrumentations(selection_start_line,selection_start_column,selection_end_line,selection_end_column);
}

void WSourceView::textClicked ()
{
  gotoNextInstrumentation(0,Instrumentation::FILTER_NONE);
}

QMenu *WSourceView::popupMenu(int /*line*/, int /*column*/)
{
  QMenu *popup_p = new QMenu();
  if ( !  findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column).isEmpty() )
  {
    popup_p->addAction(setCommentAction);
    if (hasComments())
      popup_p->addAction(clearCommentAction);
    if (hasNoValidated())
      popup_p->addAction(markAsValidatedAction);
    if (hasValidated())
      popup_p->addAction(clearValidationFlagAction);
  }
  popup_p->addAction(saveAsAction);
  return popup_p;
}

void WSourceView::emitPossibilities()
{
   bool preprocessor_view=false;
   if (csmes_p)
      preprocessor_view=csmes_p->isSourceExisting(module,source,CSMesUndoRedoFramework::CSMES);
   if ( !  findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column).isEmpty() )
      emit editPossibilities(true,hasComments(), hasNoValidated() , hasValidated(),preprocessor_view);
   else
      emit editPossibilities(false,false,false,false,preprocessor_view);
}

void WSourceView::saveAs()
{
   QString selectedFilter=Options::get_opt_str(QString(),"SOURCE_VIEW_SAVE_AS_FORMAT",QString());
   QString html_filter=tr("HTML File (*.html *.htm)");
   QString text_filter=tr("Text File (*.txt)");
   QString file=QFileDialog::getSaveFileName(this,tr("Save as..."),QString(),
      html_filter+";;"+text_filter,
      &selectedFilter,
      FILE_DIALOG_OPTIONS);
   if (!file.isEmpty())
   {
      Options::set_opt(QString(),"SOURCE_VIEW_SAVE_AS_FORMAT",selectedFilter);
      QFile f(file);
      if (!f.open(QIODevice::WriteOnly))
      {
         QMessageBox::warning(this,tr("Error"),tr("Cannot open file '%1'!").arg(file));
         return;
      }
      QTextStream stream(&f);
      if (selectedFilter==html_filter)
         stream << text_document.toHtml();
      else if (selectedFilter==text_filter)
         stream << text_document.toPlainText();
      else
      {
         ASSERT(false);
      }
      f.close();
   }
}

void WSourceView::setManuallyValidated()
{
  csmes_p->beginUndoGroup(tr("mark as validated"));
  set_validated_flag(true);
  csmes_p->endUndoGroup();
}

void WSourceView::set_validated_flag(bool v)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (v)
  {
    if (!Options::get_opt_bool(QString(),"NO_COMMENT_FOR_MANUAL_VALIDATION"))
    {
      if (!set_comment())
        return ;
    }
  }
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  QList<int>::const_iterator selected_it;
  for (selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    csmes_p->setManuallyValidated(module,source,*selected_it,v);
  }
  selectInstrumentations(selection_start_line,selection_start_column,selection_end_line,selection_end_column);
  setExplanation(explanation_item);
  CoverageSettings::object().notifyModification();
}

void WSourceView::clearManuallyValidated()
{
  csmes_p->beginUndoGroup(tr("clear validation flag"));
  set_validated_flag(false);
  csmes_p->endUndoGroup();
}

void WSourceView::clearComment()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  QList<int>::const_iterator selected_it;

  csmes_p->beginUndoGroup(tr("clear comment"));
  for (selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    csmes_p->setComment(module,source,*selected_it,QString::null);
  }
  csmes_p->endUndoGroup();
  setExplanation(explanation_item);
}

void WSourceView::setComment()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  csmes_p->beginUndoGroup(tr("add/set comment"));
  if (set_comment())
  {
    setExplanation(explanation_item);
  }
  csmes_p->endUndoGroup();
}

bool WSourceView::set_comment()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;

  QString comment=QString::null;
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  QList<int>::const_iterator selected_it;
  QStringList comments;
  for (selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    QString s=csmes_p->getComment(module,source,*selected_it);
    if (s!="" && s!=QString::null)
    {
      if (!comments.contains(s))
      {
        comments+=s;
        if (comment!=QString::null)
          comment+="<HR>"+s;
        else
          comment=s;
      }
    }
  }

  WComment *comment_p = new WComment();
  comment_p->setText(comment);
  if (comment_p->exec())
  {
    comment=comment_p->text();
    for (selected_it=selected.begin();selected_it!=selected.end();++selected_it)
    {
      csmes_p->setComment(module,source,*selected_it,comment);
    }
    return true;
  }
  return false;
}

void WSourceView::next(Instrumentation::filter_t status)
{
    gotoNextInstrumentation(+1,status);
}

void WSourceView::previous(Instrumentation::filter_t status)
{
    gotoNextInstrumentation(-1,status);
}


void WSourceView::gotoNextInstrumentation (int direction ,Instrumentation::filter_t filter)
{
  int line,column;
  getCursorPosition(line,column);
  int found=findInstrumentation(line,column,direction,filter);
  if (found==-1)
  {
    if (direction==1)
    {
      emit requestNextModule(filter);
    }
    if (direction==-1)
    {
      emit requestPreviousModule(filter);
    }
    return;
  }
  markInstrumentation(found);
  emitPossibilities();
}

void WSourceView::markInstrumentation(int index)
{
  if (index!=-1)
  {
    const QVector<Instrumentation> &inst=csmes_p->instrumentationList(module,source);
    const Instrumentation *found_p=&inst.at(index);
    int para=1;
    int pos=1;
    if (_line_mode)
    {
      selection_start_column=0;
      selection_start_line=found_p->startLineOrg();
      selection_end_line=found_p->endLineOrg();
      selection_end_column=32768;
      para=(selection_start_line+selection_end_line)/2;
    }
    else
    {
      selection_start_column=found_p->startColumnPre();
      selection_end_column=found_p->endColumnPre();
      selection_start_line=found_p->startLinePre();
      selection_end_line=found_p->endLinePre();
      para=(selection_start_line+selection_end_line)/2;
      if (selection_start_line==selection_end_line)
        pos=(selection_end_column+selection_start_column)/2;
    }
    selectInstrumentations(selection_start_line,selection_start_column,selection_end_line,selection_end_column);
    setExplanation(findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) );
    setCursorPosition(para,pos);

    ensureCursorVisible();
  }
}

bool WSourceView::lineMode() const
{
  bool line_mode= source_type==CSMesUndoRedoFramework::ORIGINAL;
  if (line_mode && !csmes_p->isSourceExisting(module,source,CSMesUndoRedoFramework::CSMES))
    line_mode=false;
  return line_mode;
}

int WSourceView::findInstrumentation (int line, int column,int direction ,Instrumentation::filter_t filter)
{
  if (csmes_p==NULL)
    return -1 ;

  const QVector<Instrumentation> &inst=csmes_p->instrumentationList(module,source);
  if (inst.isEmpty())
    return -1;
  if ( source_type==CSMesUndoRedoFramework::SOURCE_UNDEF )
    return -1 ;
  int next_id=-1;
  int previous_id=-1;
  int match_id=-1;
  int sz= static_cast<int>(inst.size());

  for ( int i = 0; i < sz ;  i++ )
  {
    Instrumentation::status_t status=inst.at(i).status(CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod());
    bool include=false;
    switch (filter)
    {
      case Instrumentation::FILTER_NONE:
        include=true;
        break;
      case Instrumentation::FILTER_MANUALLY_VALIDATED:
        if ( status==Instrumentation::STATUS_MANUALLY_VALIDATED )
          include=true;
        break;
      case Instrumentation::FILTER_VALIDATED:
        if ( status==Instrumentation::STATUS_VALIDATED )
          include=true;
        else if ( status==Instrumentation::STATUS_PARTIALLY_VALIDATED )
          include=true;
        break;
      case Instrumentation::FILTER_UNVALIDATED:
        if ( status==Instrumentation::STATUS_UNVALIDATED )
          include=true;
        else if ( status==Instrumentation::STATUS_PARTIALLY_VALIDATED )
          include=true;
        break;
      case Instrumentation::FILTER_COMMENT:
        {
          QString s=csmes_p->getComment(module,source,i);
          if (s!="" && s!=QString::null)
            include=true;
        }
        break;
    }
    if (status==Instrumentation::STATUS_NONE)
      continue;
    if (  include )
    {
      if (_line_mode)
      {
        if ( inst.at(i).endLineOrg() >= line
            && inst.at(i).startLineOrg() <= line)
        {
          match_id=i;
        }
        if ( inst.at(i).endLineOrg() < line)
        {
          if (previous_id==-1)
            previous_id=i;
          if ( inst.at(previous_id).endLineOrg()<inst.at(i).endLineOrg())
            previous_id=i;
        }
        if ( inst.at(i).startLineOrg() > line)
        {
          if (next_id==-1)
            next_id=i;
          if (inst.at(next_id).startLineOrg()>inst.at(i).startLineOrg())
            next_id=i;
        }
      }
      else
      {
        if ( ( inst.at(i).endLinePre() > line
              || ( inst.at(i).endLinePre() == line
                && inst.at(i).endColumnPre() > column ) )
            &&
            ( inst.at(i).startLinePre() < line
              || ( inst.at(i).startLinePre() == line
                && inst.at(i).startColumnPre() <= column ) ) )
        {
          match_id=i;
        }
        if ( inst.at(i).endLinePre() < line
            || ( inst.at(i).endLinePre() == line
              && inst.at(i).endColumnPre() < column ) )
        {
          if (previous_id==-1)
            previous_id=i;
          if ( ( inst.at(previous_id).endLinePre()<inst.at(i).endLinePre())
              || ( inst.at(previous_id).endLinePre()==inst.at(i).endLinePre()
                && inst.at(previous_id).endColumnPre()<inst.at(i).endColumnPre() ) )
            previous_id=i;
        }
        if ( inst.at(i).startLinePre() > line
            || ( inst.at(i).startLinePre() == line
              && inst.at(i).startColumnPre() > column ) )
        {
          if (next_id==-1)
            next_id=i;
          if ( (inst.at(next_id).startLinePre()>inst.at(i).startLinePre())
              || ( inst.at(next_id).startLinePre()==inst.at(i).startLinePre()
                && inst.at(next_id).startLinePre()>inst.at(i).startLinePre() ) )
            next_id=i;
        }
      }
    }
  }
  int found_id=-1;
  if (direction==0 && match_id>=0)
    return match_id;
  if (direction>0)
  {
    if (next_id!=-1)
      return next_id;
    else if (match_id!=-1)
      return -1; // no next item found, no changes
  }
  else if (direction<0)
  {
    if (previous_id!=-1)
      return previous_id;
    else if (match_id!=-1)
      return -1; // no previous item found, no changes
  }

  if (found_id==-1)
  {
    if ( (previous_id==-1) && (next_id!=-1))
      found_id=next_id;
    else if ( (next_id==-1) && (previous_id!=-1))
      found_id=previous_id;
    else if ( (next_id==-1) && (previous_id==-1))
      return -1;
    else
    { // take the nearest
      int dist_previous=0;
      int dist_next=0;
      switch (source_type)
      {
        case CSMesUndoRedoFramework::SOURCE_UNDEF:
          return -1;
        case CSMesUndoRedoFramework::CSMES:
          dist_previous=abs(inst.at(previous_id).endLinePre() - line);
          dist_next= abs( inst.at(next_id).startLinePre() - line);
          break;
        case CSMesUndoRedoFramework::ORIGINAL:
          dist_previous=abs(inst.at(previous_id).endLineOrg() - line);
          dist_next= abs( inst.at(next_id).startLineOrg() - line);
          break;
      }
      if (dist_next>dist_previous)
        found_id=previous_id;
      else
        found_id=next_id;
    }
  }

  return found_id;
}

const Instrumentation* WSourceView::instrumentation(int index)
{
  if (index<0)
    return NULL;
  const QVector<Instrumentation> inst=csmes_p->instrumentationList(module,source);
  return &inst.at(index);
}

void WSourceView::setSearchFilter(Instrumentation::filter_t f)
{
  int display_offset=getDisplayOffset();
  filter=f;
  switch (source_type)
  {
    case CSMesUndoRedoFramework::SOURCE_UNDEF:
      break;
    case CSMesUndoRedoFramework::CSMES:
      setCommentLine(csmes_p->commentedLinesPre(module,source));
      break;
    case CSMesUndoRedoFramework::ORIGINAL:
      setCommentLine(csmes_p->commentedLinesOrg(module,source));
      break;
  }
  setDisplayOffset(display_offset);
}

QList<int> WSourceView::findInstrumentationList(int s_line, int s_column,int e_line, int e_column) const
{
  QList<int> ret;
  if (csmes_p==NULL)
    return ret ;
  if ( source_type==CSMesUndoRedoFramework::SOURCE_UNDEF )
    return ret ;

  const QVector<Instrumentation> &inst=csmes_p->instrumentationList(module,source);
  if (inst.isEmpty())
    return ret;
  int sz= static_cast<int>(inst.size());

  for ( int i = 0; i < sz ;  i++ )
  {
    if (inst.at(i).status(CoverageSettings::object().getCoverageLevel(),CoverageSettings::object().getCoverageMethod())==Instrumentation::STATUS_NONE)
      continue; //ignore
    if (_line_mode)
    {
        if ( inst.at(i).endLineOrg() >= s_line
            && inst.at(i).endLineOrg() <= e_line)
          ret.append(i);
        else if ( inst.at(i).startLineOrg() >= s_line
            && inst.at(i).startLineOrg() <= e_line)
          ret.append(i);
        else if ( inst.at(i).endLineOrg() >= e_line
            && inst.at(i).startLineOrg()  <= s_line)
          ret.append(i);
    }
    else
    {
        if ( ( inst.at(i).endLinePre() > s_line
              || ( inst.at(i).endLinePre() == s_line
                && inst.at(i).endColumnPre() >= s_column ) )
            && (inst.at(i).endLinePre() < e_line
              || ( inst.at(i).endLinePre() == e_line
                && inst.at(i).endColumnPre() <= e_column ) ) )
          ret.append(i);
        else if ( ( inst.at(i).startLinePre() > s_line
              || ( inst.at(i).startLinePre() == s_line
                && inst.at(i).startColumnPre() >= s_column ) )
            && (inst.at(i).startLinePre() < e_line
              || ( inst.at(i).startLinePre() == e_line
                && inst.at(i).startColumnPre() <= e_column ) ) )
          ret.append(i);
        else if ( ( inst.at(i).endLinePre() > e_line
              || ( e_line == inst.at(i).endLinePre()
                &&  inst.at(i).endColumnPre() >= e_column ) )
            && ( inst.at(i).startLinePre()  < s_line
              || ( s_line == inst.at(i).startLinePre()
                && inst.at(i).startColumnPre() <= s_column ) ) )
          ret.append(i);
    }
  }

  return ret;
}


void WSourceView::wheelEvent ( QWheelEvent * e )
{
  if (e->modifiers() & Qt::ShiftModifier)
  {
    if (e->delta()>0)
      gotoNextInstrumentation(+1,filter);
    else
      gotoNextInstrumentation(-1,filter);
    e->accept();
  }
  if (e->modifiers() & Qt::ControlModifier)
  {
    if (e->delta()>0)
      zoomIn();
    else
      zoomOut();
    e->accept();
  }
  else
    WTextSourceViewer::wheelEvent (e);
}

bool WSourceView::hasComments() const
{
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  for (QList<int>::const_iterator selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    QString s=csmes_p->getComment(module,source,*selected_it);
    if (s!="" && s!=QString::null)
      return true;
  }
  return false;
}

bool WSourceView::hasValidated() const
{
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  for (QList<int>::const_iterator selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    if (csmes_p->getManuallyValidated(module,source,*selected_it))
      return true;
  }
  return false;
}

bool WSourceView::hasNoValidated() const
{
  QList<int> selected = findInstrumentationList( selection_start_line,selection_start_column,selection_end_line,selection_end_column) ;
  for (QList<int>::const_iterator selected_it=selected.begin();selected_it!=selected.end();++selected_it)
  {
    if ( ! csmes_p->getManuallyValidated(module,source,*selected_it))
      return true;
  }
  return false;
}

void WSourceView::firstInstrumentation(Instrumentation::filter_t t)
{
  setCursorPosition(1,1);
  gotoNextInstrumentation(1,t);
}

void WSourceView::lastInstrumentation(Instrumentation::filter_t t)
{
  setCursorPosition(-1,-1);
  gotoNextInstrumentation(-1,t);
}

void WSourceView::zoomIn()
{
  QFont f=font();

  int s=f.pointSize();
  f.setPointSize(s+1);
  setCurrentFont(f);
  updateView();
}

void WSourceView::zoomOut()
{
  QFont f=font();
  int s=f.pointSize();
  if (s>1)
  {
    f.setPointSize(s-1);
    setCurrentFont(f);
    updateView();
  }
}

void WSourceView::differenceWithReference(const ModuleFile &module,const SourceFile &source,QTextDocument &doc) const
{
  if (csmes_p->hasReference())
  {

    CSMes::modifications_t modif=csmes_p->compareCSMesSource(module,source)  ;
    int pos=0;
    QTextCursor cur(&doc);
    QTextCharFormat insert_format;
    insert_format.setFontWeight(QFont::Bold);
    insert_format.setFontUnderline(true);
    insert_format.setFontStrikeOut(false);
    QTextCharFormat delete_format;
    delete_format.setFontWeight(QFont::Bold);
    delete_format.setFontStrikeOut(true);
    delete_format.setFontUnderline(false);

    switch (modif)
    {
    case CSMes::SAME:
    case CSMes::SAME_EXCEPT_WHITE_SPACES:
    case CSMes::SAME_EXCEPT_COMMENT:
       return;
    case CSMes::DIFFERENT:
       break;
    case CSMes::NOT_EXISTING:
       {
         ModuleFile mod,mod_ref;
         SourceFile src,src_ref;
         csmes_p->equivalentModulesReference(module,source,mod,src,mod_ref,src_ref);
         QString reference;
         csmes_p->toPLAINReference(mod_ref,src_ref,source_type,reference);
         cur.insertText(reference);
         cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
         cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
         cur.mergeCharFormat(delete_format);
         int current_block=0;
         for (QTextBlock textBlock = doc.begin(); textBlock!=doc.end() ;textBlock=textBlock.next())
         {
           TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
           if (instrumentationData)
             instrumentationData->clear();
           else
           {
             instrumentationData=new TextSourceInstrumentationData;
             textBlock.setUserData(instrumentationData);
           }
           instrumentationData->line_mode=_line_mode;
           instrumentationData->coverage_level=CoverageSettings::object().getCoverageLevel();
           instrumentationData->coverage_method=CoverageSettings::object().getCoverageMethod();
           instrumentationData->current_line=current_block+1;
           current_block++;
         }
       }
       return;
    case CSMes::NEW:
       cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
       cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
       cur.mergeCharFormat(insert_format);
       return;
    case CSMes::NOTHING_TO_COMPARE:
       return;
    case CSMes::MODIFICATIONS_UNDEFINED:
       break;
    }

    QList<DiffItem> diff = csmes_p->differencesWithReference(module,source,source_type);
    for (QList<DiffItem>::const_iterator it=diff.begin();it!=diff.end();++it)
    {
      int lg=(*it).length();
      switch ( (*it).operation() )
      {
        case DiffItem::DELETED:
          {
            int offset=lg;
            QTextBlock textBlock=doc.findBlock(pos+lg);
            int column=pos+lg-textBlock.position()+1;
            TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
            if (instrumentationData)
            {
              for (QVector<TextSourceInstrumentationData::InstrumentationData>::Iterator itInst=instrumentationData->instrumentations.begin();
                  itInst!=instrumentationData->instrumentations.end();++itInst)
              {
                if ( (*itInst).startColumnPre()>column )
                  (*itInst).setStartColumnOffset(offset+(*itInst).startColumnOffset());;
                if ( (*itInst).endColumnPre()>column )
                  (*itInst).setEndColumnOffset(offset+(*itInst).endColumnOffset());;
              }
            }
            cur.setPosition(pos,QTextCursor::MoveAnchor);
            cur.insertText((*it).text());
            cur.setPosition(pos,QTextCursor::MoveAnchor);
            cur.setPosition(pos+lg,QTextCursor::KeepAnchor);
            cur.mergeCharFormat(delete_format);
          }
          break;
        case DiffItem::INSERTED:
          cur.setPosition(pos,QTextCursor::MoveAnchor);
          cur.setPosition(pos+lg,QTextCursor::KeepAnchor);
          cur.mergeCharFormat(insert_format);
          break;
        case DiffItem::SAME:
          break;
      }
      pos+=lg;

    }
  }
}

void WSourceView::insertInstrumentationData(QTextDocument &doc) const
{
  int current_block=0;
  const QVector<Instrumentation> &instrumentation_list=csmes_p->instrumentationList(module,source);
  QVector<TextSourceInstrumentationData*> instrumentationDatas;
  instrumentationDatas.resize(doc.blockCount());
  QTextBlock textBlock;
  for (textBlock = doc.begin(); textBlock!=doc.end() ;textBlock=textBlock.next())
  {
    TextSourceInstrumentationData *instrumentationData=dynamic_cast<TextSourceInstrumentationData*> (textBlock.userData());
    if (instrumentationData)
      instrumentationData->clear();
    else
    {
      instrumentationData=new TextSourceInstrumentationData;
      textBlock.setUserData(instrumentationData);
    }
    instrumentationData->line_mode=_line_mode;
    instrumentationData->coverage_level=CoverageSettings::object().getCoverageLevel();
    instrumentationData->coverage_method=CoverageSettings::object().getCoverageMethod();
    instrumentationData->current_line=current_block+1;

    instrumentationDatas[current_block]=instrumentationData;
    current_block++;
  }
  int max_blocks=current_block;
  for (QVector<Instrumentation>::const_iterator inst_it=instrumentation_list.begin();inst_it!=instrumentation_list.end();++inst_it)
  {
    long start_line,end_line;
    if (_line_mode)
    {
      start_line=(*inst_it).startLineOrg();
      end_line=(*inst_it).endLineOrg();
    }
    else
    {
      start_line=(*inst_it).startLinePre();
      end_line=(*inst_it).endLinePre();
    }
    for (int line=start_line;line<=end_line;++line)
    {
      if (line<=max_blocks)
      {
        TextSourceInstrumentationData *instrumentationData=instrumentationDatas[line-1];
        TextSourceInstrumentationData::InstrumentationData inst;
        inst.setInstrumentation( & (*inst_it) );
        instrumentationData->instrumentations.append(inst);
      }
    }
  }
}

void WSourceView::refreshView()
{
   updateView();
   repaint();
}

void WSourceView::findText_slot ( const QString & exp, QTextDocument::FindFlags options  )
{
  if (!find(exp,options))
  {
     emit requestFindTextModule(exp,options);
  }
}

void WSourceView::showFindText()
{
   toggleShowFindText();
}
