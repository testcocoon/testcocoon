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

#include "WComment.h"
#include <qvariant.h>
#include <qimage.h>
#include <Qt>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qregexp.h>
#include "coveragebrowserpdef.h"
#include "options.h"

WComment::WComment(QWidget* parent, Qt::WindowFlags fl) : QDialog(parent, fl)
{
  setupUi(this);

  // signals and slots connections
  connect(comment_p, SIGNAL(reject()), this, SLOT(rejectSlot()));
  connect(comment_p, SIGNAL(accept()), this, SLOT(acceptSlot()));
}

WComment::~WComment()
{
  // no need to delete child widgets, Qt does it all for us
}

void WComment::languageChange()
{
  retranslateUi(this);
}

void WComment::acceptSlot()
{
  accept();
}

void WComment::rejectSlot()
{
  reject();
}

int WComment::exec()
{
  return QDialog::exec();
}

QString WComment::text()
{
  return comment_p->text();
}

void WComment::setText(const QString &c)
{
  comment_p->setText(c);
}

