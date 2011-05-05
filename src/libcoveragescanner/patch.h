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

#ifndef PATCH_H
#define PATCH_H
#include "coveragescannerpdef.h"
#include "expressioninterface.h"
#include <sstream>
#include <string>
#include <list>
#include "stringpool.h"

class Patch
{
  protected:
    enum PatchPriority
    {
      PriorityEnd,
      PriorityStart,
    };

  public:
    Patch(const char *__f,long __line_rel,long __line,long __col,long __depth,const instrumentation_option_t &__option,long __offset,const char *__table_name,PatchPriority __priority) ;
    std::string patchCode() const;

    virtual std::string code(const std::string &table_name) const=0;


    friend bool operator < (const Patch & lhs, const Patch & rhs)
    {
      if (lhs.line()<rhs.line())
        return true;
      else if ( lhs.line()==rhs.line() && lhs.column()<rhs.column())
        return true;
      else if ( lhs.line()==rhs.line() && lhs.column()==rhs.column() && lhs.depth()<rhs.depth())
        return true;
      else if ( lhs.line()==rhs.line() && lhs.column()==rhs.column() && lhs.depth()==rhs.depth() && lhs.priority()<rhs.priority())
        return true;
      return false;
    }

    const char *file() const { return _file; }
    long line() const { return _line; }
    long line_rel() const { return _line_rel; }
    long column() const { return _column ;}
    long offset() const { return _offset ;}
    long depth() const { return _depth ;}
    const instrumentation_option_t & option() const { return _option ;}
    virtual std::string name() const { return "<unknown>"; }
    PatchPriority priority() const { return _priority ;}
#if LOG
    void PDEBUG() const;
#endif
  private:
    const char *_file;
    long _line;
    long _line_rel;
    long _column;
    PatchPriority _priority;
    long _depth;
    long _offset;
    instrumentation_option_t _option;
    std::string  _table_name;
  protected:

    static std::string LongToString(long v)
    {
      std::stringstream out;
      out << v;
      return out.str();
    }

    static std::string IntToString(int v)
    {
      std::stringstream out;
      out << v;
      return out.str();
    }
  private:
    static StringPool filenames_pool;

};


#endif
