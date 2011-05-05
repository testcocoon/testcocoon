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

#include "WTextSourceViewerSearch.h"
#include <QProgressBar>
#include "coveragebrowserpdef.h"
#include "options.h"
#include "Service.h"
#include "csmesundoredoframework.h"
#include <QStyle>
#include <QKeyEvent>
#include <QtDebug>
#include <QStyleFactory>
#if QT_COMPLETOR
#include <QStringListModel>
#include <QCompleter>
#endif

WTextSourceViewerSearch::WTextSourceViewerSearch(QWidget* parent,  Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
  setupUi(this);
  _force_visible=false;
  setFocusProxy(search_text_p);
  backward=false;
#if QT_COMPLETOR
  QStringList lru=Options::get_opt_strlst(QString(),"/SOURCE_SEARCH_COMPLETION");
  completor_p = new QCompleter(search_text_p);
  completor_model_p=new QStringListModel(lru,completor_p);
  completor_p->setModel(completor_model_p);
  completor_p->setModelSorting(QCompleter::UnsortedModel);
  completor_p->setCompletionMode(QCompleter::PopupCompletion);
  search_text_p->setCompleter( completor_p );
#endif

   setSizes();
}

WTextSourceViewerSearch::~WTextSourceViewerSearch()
{
#if QT_COMPLETOR
   appendLRU();
  delete completor_p;
#endif
}

void WTextSourceViewerSearch::on_previous_p_clicked()
{
  backward=true;
  search();
}

void WTextSourceViewerSearch::on_next_p_clicked()
{
  backward=false;
  search();
}

void WTextSourceViewerSearch::on_search_text_p_returnPressed()
{
  search();
}

void WTextSourceViewerSearch::on_search_text_p_textChanged(const QString &/*text*/)
{
   setSizes();
}

void WTextSourceViewerSearch::search()
{
   appendLRU();
  QString text=search_text_p->text();
  QTextDocument::FindFlags options ;
  if (backward)
    options |= QTextDocument::FindBackward;
  if (whole_word_p->isChecked())
    options |= QTextDocument::FindWholeWords;
  if (case_sensitive_p->isChecked())
    options |= QTextDocument::FindCaseSensitively;
  emit findText(text,options);
}

void WTextSourceViewerSearch::setSizes()
{
   QSize sz=sizeHint();
   if (search_text_p->text().isEmpty() && !_force_visible)
      sz.setHeight(0);

  if (minimumSize().height()!=sz.height())
    setMinimumSize(QSize(minimumSize().width(),sz.height()));
  if (maximumSize().height()!=sz.height())
     setMaximumSize(QSize(maximumSize().width(),sz.height()));
  if (size().height()==0)
     hide();
  else
     show();
}

#if QT_COMPLETOR
void WTextSourceViewerSearch::appendLRU()
{
  QStringList old_lru=Options::get_opt_strlst(QString(),"/SOURCE_SEARCH_COMPLETION");
  QStringList new_lru=appendLRU(search_text_p->text(),old_lru);
  Options::set_opt(QString(),"/SOURCE_SEARCH_COMPLETION",new_lru);
  completor_model_p->setStringList(new_lru);
}

QStringList WTextSourceViewerSearch::appendLRU(const QString &value,QStringList &old_lru)
{
   if (value.isEmpty())
      return old_lru;
  QStringList new_lru;
  old_lru.removeAll(value);
  new_lru += value ;
  new_lru += old_lru ;
  while (new_lru.count()>LRU_MAX_ITEM)
    new_lru.removeLast();
  return new_lru;
}
#endif


void WTextSourceViewerSearch::display(bool visible)
{
   _force_visible=visible;
   if (!visible)
   {
      hide();
   }
   else
   {
      show();
      setSizes();
   }
}

void WTextSourceViewerSearch::toggleVisible()
{
   display(!isVisible());
}
