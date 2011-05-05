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

#ifndef XML_WRITER_H
#define XML_WRITER_H
#include "writer.h"
#include <QMultiMap>
#include <QSet>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QColor>

class XMLWriter : public Writer
{
  public:
    XMLWriter (QIODevice *f) : Writer(f) {setDevice(f); initTags();}
    void writeLine();
    void beginBody();
    void endBody();
    void beginHeader();
    void endHeader();
    void insertHtml(const QString &);
  protected:
    void setDevice(QIODevice *d) { stream.setDevice(d); }
    QXmlStreamWriter stream;
    void _begin(section_type_t);
    void _end(section_type_t);
    void _write(int);
    void _write(const QString &);
    void _write(double);

  private:
    void initTags();
    bool ignoreTag(section_type_t) const ;
} ;

#endif
