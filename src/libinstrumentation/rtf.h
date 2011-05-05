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

#ifndef RTF_H
#define RTF_H
#include <QString>
#include <QBitArray>
#include <QColor>
#include <QTextDocument>
#include <QFont>
#include "libinstrumentationpdef.h"

class LIBINSTRUMENTATION_API RTF
{
  public:
    static void CPPSyntax(const QString &input,QString &output);
    static int  numberOfLines(const QString &input);
    static void return2paragraph(QString &input);
    static void hideLines(const QBitArray &,const QTextDocument *input,QTextDocument *&output);
    static void toTableFormat(QString &input);
    static QString extractLine(const QString &text,int line);
    static QString toPlain(const QString &text);
    static void setDefaultFormating(const QFont &font,QTextDocument *document);
};
#endif
