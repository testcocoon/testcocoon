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

#include "WCommentWidget.h"
#include <QVariant>
#include <QImage>
#include <Qt>
#include <QPixmap>
#include <QFontDialog>
#include <QColorDialog>
#include <QStringList>
#include <QRegExp>
#include "coveragebrowserpdef.h"
#include "options.h"
#include "csmesbackgroundcomputations.h"
static QStringList old_comments;
static QRegExp rx1("</p>|<br>|<p />|<p/>");
static QRegExp rx2("<[^>]*>");

WCommentWidget::WCommentWidget(QWidget* parent) : QWidget(parent)
{
  setupUi(this);
  _comment_length_check=true;
  QFont f;
  f.fromString(Options::get_opt_str(QString(),"FONT_COMMENT",DEF_FONT_COMMENT));
  comment_p->setCurrentFont(f);
  comment_p->setFont(f);
  old_comments_p->setSelectionMode( QAbstractItemView::SingleSelection );


  // signals and slots connections
  connect(comment_p, SIGNAL(cursorPositionChanged()), this, SLOT(update_format_information()));
  connect(comment_p, SIGNAL(selectionChanged()), this, SLOT(update_format_information()));
  connect(old_comments_p, SIGNAL(itemSelectionChanged()), this, SLOT(old_list_selection()));
  connect(comment_p, SIGNAL(textChanged()), this, SLOT(on_comment_p_textChanged()));
  init();
}

WCommentWidget::~WCommentWidget()
{
  // no need to delete child widgets, Qt does it all for us
}

void WCommentWidget::languageChange()
{
  retranslateUi(this);
}

void WCommentWidget::on_textcolor_p_clicked(bool)
{
  QColor c = comment_p->textColor();
  c=QColorDialog::getColor(c,this);
  comment_p->setTextColor(c);
}

void WCommentWidget::on_strike_p_toggled(bool b)
{
  QFont font = comment_p->currentFont();
  font.setStrikeOut(b);
  comment_p->setCurrentFont(font);
}

void WCommentWidget::on_italic_p_toggled(bool b)
{
  comment_p->setFontItalic(b);
}

void WCommentWidget::on_underline_p_toggled(bool b)
{
  comment_p->setFontUnderline(b);
}

void WCommentWidget::on_bold_p_toggled(bool b)
{
  if (b)
    comment_p->setFontWeight(QFont::Bold);
  else
    comment_p->setFontWeight(QFont::Normal);
}


void WCommentWidget::init()
{
  minimum_comment_size=Options::get_opt_long(QString(),"MINIMUM_COMMENT_SIZE",DEF_MINIMUM_COMMENT_SIZE);
  old_alignment = -1;
  QPalette palette;
  palette.setColor(comment_p->backgroundRole(), COL_YELLOW_LIGHT);
  comment_p->setPalette(palette);

  QStringList old_comments_plain;
  int nb=old_comments.count();
  for (int i=0;i<nb;i++)
  {
    QTextDocument td;
    td.setHtml(old_comments[i]);
    old_comments_plain+=td.toPlainText().replace("\n", "|");
  }
  old_comments_p->clear();
  old_comments_p->insertItems(0,old_comments_plain);
  old_comments_p->setEnabled(true);
}

void WCommentWidget::update_format_information()
{
  if (comment_p->textCursor().hasSelection())
    return ;
  if ( (comment_p->currentFont().strikeOut()) != strike_p->isChecked())
    strike_p->setChecked(comment_p->currentFont().strikeOut());
  if ( (comment_p->fontWeight()>=QFont::Bold) != bold_p->isChecked())
    bold_p->setChecked(comment_p->fontWeight()>=QFont::Bold);
  if (comment_p->fontItalic()!=italic_p->isChecked())
    italic_p->setChecked(comment_p->fontItalic());
  if (comment_p->fontUnderline()!=underline_p->isChecked())
    underline_p->setChecked(comment_p->fontUnderline());
  if (old_alignment!=comment_p->alignment())
  {
    old_alignment=comment_p->alignment();
    switch (comment_p->alignment())
    {
      case Qt::AlignLeft:
        right_p->setChecked(false);
        center_p->setChecked(false);
        justify_p->setChecked(false);
        if (!left_p->isChecked())
          left_p->setChecked(true);
        break;
      case Qt::AlignRight:
        left_p->setChecked(false);
        center_p->setChecked(false);
        justify_p->setChecked(false);
        if (!right_p->isChecked())
          right_p->setChecked(true);
        break;
      case Qt::AlignCenter:
        left_p->setChecked(false);
        right_p->setChecked(false);
        justify_p->setChecked(false);
        if (!center_p->isChecked())
          center_p->setChecked(true);
        break;
      case Qt::AlignJustify:
        left_p->setChecked(false);
        right_p->setChecked(false);
        center_p->setChecked(false);
        if (!justify_p->isChecked())
          justify_p->setChecked(true);
        break;
    }
  }
}


void WCommentWidget::on_right_p_toggled( bool b)
{
  if (b)
  {
    comment_p->setAlignment(Qt::AlignRight);
    left_p->setChecked(false);
    center_p->setChecked(false);
    justify_p->setChecked(false);
  }
}


void WCommentWidget::on_center_p_toggled( bool b)
{

  if (b)
  {
    comment_p->setAlignment(Qt::AlignCenter);
    right_p->setChecked(false);
    left_p->setChecked(false);
    justify_p->setChecked(false);
  }
}


void WCommentWidget::on_left_p_toggled( bool b)
{

  if (b)
  {
    comment_p->setAlignment(Qt::AlignLeft);
    right_p->setChecked(false);
    center_p->setChecked(false);
    justify_p->setChecked(false);
  }
}


void WCommentWidget::on_justify_p_toggled( bool b)
{

  if (b)
  {
    comment_p->setAlignment(Qt::AlignJustify);
    right_p->setChecked(false);
    center_p->setChecked(false);
    left_p->setChecked(false);
  }
}


void WCommentWidget::on_ok_p_clicked()
{
  if (!old_comments.contains(text()))
    old_comments += text();
  CSMesBackgroundComputations::Pauser statistic_pauser;
  accept();
}

void WCommentWidget::on_cancel_p_clicked()
{
  comment_p->setHtml(_comment);
  comment_p->document()->setModified(false);
  reject();
}


QString WCommentWidget::text()
{
  QString s= comment_p->toHtml();
  QRegExp start_body("^.*<body[^<]*>[ \r\n]*");
  QRegExp end_body("[ \r\n]*</body[^<]*>.*$");
  start_body.setCaseSensitivity(Qt::CaseInsensitive);
  end_body.setCaseSensitivity(Qt::CaseInsensitive);
  s=s.replace(start_body,"");
  s=s.replace(end_body,"");
  return s;
}

void WCommentWidget::old_list_selection()
{
  int max=old_comments_p->count();
  const QList<QListWidgetItem *> selItems=old_comments_p->selectedItems();
  if (selItems.count()<=0)
    return ;
  const QListWidgetItem *selectedWidget=selItems.at(0);
  for (int i=0;i<max;i++)
  {
    if (selectedWidget==old_comments_p->item(i))
    {
      comment_p->setHtml(old_comments[i]);
      old_comments_p->clearSelection();
      return ;
    }
  }
}

void WCommentWidget::setText(const QString &c)
{
  _comment=c;
  comment_p->setHtml(c);
  comment_p->document()->setModified(false);
  QFont f;
  f.fromString(Options::get_opt_str(QString(),"FONT_COMMENT",DEF_FONT_COMMENT));
  comment_p->setCurrentFont(f);
  comment_p->setFont(f);
}


void WCommentWidget::on_comment_p_textChanged()
{
  if (_comment_length_check)
  {
    QString t=comment_p->toPlainText();
    int s=t.length()-1;
    if (s<0)
      s=0;
    ok_p->setEnabled(s>=minimum_comment_size);
    count_p->display((int)s);
  }
  else
    ok_p->setEnabled(true);
}

bool WCommentWidget::isModified() const 
{
  if (comment_p->document())
    return comment_p->document()->isModified();
  return false;
}

void WCommentWidget::setCommentLengthCheck(bool b)
{
  _comment_length_check=b;
  count_p->setVisible(_comment_length_check);
}
