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

#ifndef HTML_WRITER_H
#define HTML_WRITER_H
#include "writer.h"
#include <QMultiMap>
#include <QSet>
#include <QStringList>
#include <QColor>

class HTMLWriter : public Writer
{
  public:
    HTMLWriter (QIODevice *f) : Writer(f) {setDevice(f);initTags();}
    void writeLineFormating() { stream<<endl ; }
    void writeLine();
    void beginBody();
    void endBody();
    void beginHeader();
    void endHeader();
    void insertHtml(const QString &);
  protected:
    void setDevice(QIODevice *d) { stream.setDevice(d); }
    QTextStream stream;
    void _begin(section_type_t);
    void _end(section_type_t);
    void _write(int);
    void _write(const QString &);
    void _write(double);

  private:
    void initTags();
    inline QList<QStringList> tag(section_type_t) const;
    QMultiMap<section_type_t,QStringList > tags;
    QMap<section_type_t,QStringList > styles;
    QSet<section_type_t> no_ident_tags;
    QSet<section_type_t> no_class;
    void indent_end(section_type_t);
    void indent_begin(section_type_t);
    int section_count;
    void bargraph(QColor,double);
    bool no_ident_this_section;
} ;

#endif
