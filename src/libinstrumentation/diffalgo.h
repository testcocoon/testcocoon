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

#ifndef DIFFALGO_H
#define DIFFALGO_H
#include "libinstrumentationpdef.h"
#include <string>
#include <QString>

class DiffItem
{
  public:
    enum operation_t
    {
      DELETED,
      INSERTED,
      SAME
    };
  private:
    QString _text;
    operation_t _operation;
    int _lg;
  public:
    const QString & text() const { return _text; }
    void setText(const QString &t) { _text=t; }
    operation_t operation() const { return _operation; }
    void setOperation(operation_t o) { _operation=o; }
    int length() const { return _lg; }
    void setLength(int lg) { _lg=lg; }
};

enum DiffPolicy
{
  DiffChar,
  DiffWord,
  DiffWordIgnoreSpaces,
  DiffWordIgnoreSpacesAndComments
};

enum differences_t
{
  DIFF_SAME,
  DIFF_DIFFERENCE_IN_BLANCS,
  DIFF_DIFFERENCE_IN_COMMENTS,
  DIFF_DIFFERENT
};

void calcDiff(QList<DiffItem> &diff,const QString &str,const QString &reference,DiffPolicy);
QString printDiff(const QString &str,const QList<DiffItem> &diff);
differences_t differences(const QString &,const QString &);
#endif

