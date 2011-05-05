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

#ifndef _FUNCTION_INFO_H
#define _FUNCTION_INFO_H
#include "libinstrumentationpdef.h"
#include <QString>

class LIBINSTRUMENTATION_API FunctionInfo
{
  public:
    FunctionInfo() : _ScopedName(), _Prototype()
    {
      _end_column_org    = -1;
      _end_column_pre    = -1;
      _end_line_org      = -1;
      _end_line_pre      = -1;
      _start_column_org  = -1;
      _start_column_pre  = -1;
      _start_line_org    = -1;
      _start_line_pre    = -1;
    }
    FunctionInfo(const QString &scoped_name,const QString &proto,long sline_org,long scol_org,long eline_org,long ecol_org,long sline_pre,long scol_pre,long eline_pre,long ecol_pre)
    {
      _ScopedName        = scoped_name;
      _Prototype         = proto;
      _end_line_pre      = eline_pre;
      _start_line_pre    = sline_pre;
      _end_column_pre    = ecol_pre;
      _start_column_pre  = scol_pre;
      _end_line_org      = eline_org;
      _start_line_org    = sline_org;
      _end_column_org    = ecol_org;
      _start_column_org  = scol_org;
      ASSERT(!valid() || _end_line_org>=0);
      ASSERT(!valid() || _end_line_pre>=0);
      ASSERT(!valid() || _start_line_pre>=0);
      ASSERT(!valid() || _end_column_pre>=0);
      ASSERT(!valid() || _start_column_pre>=0);
      ASSERT(!valid() || _start_line_org>=0);
    }
    void clear();
    bool valid() const ;
    long endLineOrg() const              { ASSERT(!valid() || _end_line_org>=0); return _end_line_org; }
    long endColumnOrg() const            { return _end_column_org; }
    long endLinePre() const              { ASSERT(!valid() || _end_line_pre>=0);return _end_line_pre; }
    long startLinePre() const            { ASSERT(!valid() || _start_line_pre>=0);return _start_line_pre; }
    long endColumnPre() const            { ASSERT(!valid() || _end_column_pre>=0);return _end_column_pre; }
    long startColumnPre() const          { ASSERT(!valid() || _start_column_pre>=0);return _start_column_pre; }
    long startLineOrg() const            { ASSERT(!valid() || _start_line_org>=0);return _start_line_org; }
    long startColumnOrg() const          { return _start_column_org; }
    const QString &getScopedName() const { return _ScopedName; }
    const QString &getPrototype() const  { return _Prototype; }
    bool operator== ( const FunctionInfo & x ) const
    {
       return _ScopedName == x._ScopedName 
          &&
          _Prototype == x._Prototype 
          &&
          _end_line_org == x._end_line_org
          &&
          _end_column_org == x._end_column_org
          &&
          _end_line_pre == x._end_line_pre
          &&
          _start_line_org == x._start_line_org
          &&
          _start_line_pre == x._start_line_pre
          &&
          _end_column_pre == x._end_column_pre
          &&
          _start_column_pre == x._start_column_pre
          ;
    }
  private:
    QString _ScopedName;
    QString _Prototype;
    long _start_line_pre,_end_line_pre,_start_column_pre,_end_column_pre;
    long _start_line_org,_end_line_org,_end_column_org, _start_column_org;
};

#endif
