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

#ifndef TEXTSOURCEINSTRUMENTATION_DATA_H
#define TEXTSOURCEINSTRUMENTATION_DATA_H
#include "libinstrumentationpdef.h"
#include "instrumentation.h"
#include <QTextBlockUserData>

class LIBINSTRUMENTATION_API TextSourceInstrumentationData : public QTextBlockUserData
{
  public:
    class InstrumentationData
    {
      private:
        const Instrumentation *_data;
        bool _selected;
        int _start_column_offset;
        int _end_column_offset;
        const Instrumentation *instrumentation() const { return _data; }
      public:
        void setInstrumentation(const Instrumentation *d) {_data=d; }
        void setSelected(bool b)   { _selected=b; }
        bool selected() const      { return _selected; }
        int startColumnPre() const { return instrumentation()->startColumnPre()+startColumnOffset();; }
        int startLinePre() const   { return instrumentation()->startLinePre(); }
        int endColumnPre() const   { return instrumentation()->endColumnPre()+startColumnOffset();; }
        void setStartColumnOffset(int offset) { _start_column_offset=offset; }
        void setEndColumnOffset(int offset) { _end_column_offset=offset; }
        int endColumnOffset() const {return _end_column_offset; }
        int startColumnOffset() const {return _start_column_offset; }
        int endLinePre() const     { return instrumentation()->endLinePre(); }
        int startColumnOrg() const { return instrumentation()->startColumnOrg()+startColumnOffset(); }
        int startLineOrg() const   { return instrumentation()->startLineOrg(); }
        int endColumnOrg() const   { return instrumentation()->endColumnOrg()+startColumnOffset();; }
        int endLineOrg() const     { return instrumentation()->endLineOrg(); }
        Instrumentation::status_t status(int level,Instrumentation::coverage_method_t method) const { return instrumentation()->status(level,method); }
        bool isInRangeOrg(int _start_line,int _start_column, int _end_line,int _end_column) const
        {
          return instrumentation()->isInRangeOrg(_start_line,_start_column, _end_line,_end_column);
        }
        bool isInRangePre(int _start_line,int _start_column, int _end_line,int _end_column) const
        {
          return instrumentation()->isInRangePre(_start_line,_start_column, _end_line,_end_column);
        }

      public:
        InstrumentationData()
        {
          _data=NULL;
          _selected=false;
          _start_column_offset=0;
          _end_column_offset=0;
        }
    };
    TextSourceInstrumentationData() { clear(); }
    void clear();
    QVector<InstrumentationData> instrumentations; 
    bool line_mode;
    int coverage_level;
    int current_line;
    Instrumentation::coverage_method_t coverage_method;
} ;

#endif
