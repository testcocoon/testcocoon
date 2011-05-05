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

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "coveragescannerpdef.h"
#include "csmesfile.h"
#include "patch.h"
#include "expressioninterface.h"
#include <string>
#include <list>

class StringPool;
class Source;
class TestExpressions;
class TestExpression;
class TestFilterFunction;
class Expression: public ExpressionInterface
{
public:
    typedef std::vector<Expression*> _childrens_t;
    Expression();
    virtual ~Expression();
    enum code_style_t { CODESTYLE_UNKNOWN, CODESTYLE_CPP, CODESTYLE_CS } ;

    bool undefined() const
    {
        return type==INSTRUMENTATION_UNDEFINED;
    }
    CSMESFile::instrumentation_childrens_t childrensCSMESFile() const ;
    bool read(instrumentation_type_t &t, const char *&f,instrumentation_option_t &options,long &s_line_absolute, long &s_line_org, long &s_column_org, long &s_line_pre, long &s_column_pre, long &e_line_absolute,  long &e_line_org, long &e_column_org,  long &e_line_pre, long &e_column_pre) const ;
    bool is_filename(const char *) const ; /*!< Expression applying of the file given as parameter? */
    virtual std::list<const Patch *> generatePatchCode(code_style_t t,const char *table_name,long index) = 0;

    virtual instrumentation_type_t instrumentation_type() const { return type ; }
    virtual long startIndex() const { return _index; }

    void clear();
    inline bool operator==(const Expression &) const;
    virtual long indexes() const=0;
    bool valid() const;
public:
    long startDepthPatch(int id) const { return _patch_location[id].start_depth; }
    long endDepthPatch(int id) const { return _patch_location[id].end_depth; }
    long startLineAbsolutePatch(int id) const { return _patch_location[id].start_line_absolute; }
    long startColumnPatch(int id) const { return _patch_location[id].start_column; }
    long endLineAbsolutePatch(int id) const { return _patch_location[id].end_line_absolute; }
    long endColumnPatch(int id) const { return _patch_location[id].end_column; }
    int nbPatches() const { return _patch_location.size(); }
public:
    long startLineAbsolute() const { return _location_pre.start_line_absolute; }
    long startColumnOrg() const { return _location_org.start_column; }
    long startColumnPre() const { return _location_pre.start_column; }
    long startLinePre() const { return _location_pre.start_line; }
    long startLineOrg() const { return _location_org.start_line; }
    long endLineAbsolute() const { return _location_pre.end_line_absolute; }
    long endColumnPre() const { return _location_pre.end_column; }
    long endLinePre() const { return _location_pre.end_line; }
    long endColumnOrg() const { return _location_org.end_column; }
    long endLineOrg() const { return _location_org.end_line; } 
    inline bool start_lt(long line_abs,long column) const;
    bool end_le(long line_abs,long column) const
    {
        return ! end_gt(line_abs,column);
    }
    inline bool end_gt(long line_abs,long column) const;
    bool start_ge(long line_abs,long column) const
    {
        return ! start_lt(line_abs,column);
    }

    friend inline bool start_eq(const Expression &,const Expression &) ;
    friend inline bool start_gt(const Expression &,const Expression &) ;
    friend inline bool end_gt(const Expression &,const Expression &) ;


#if LOG
    virtual void PDEBUG() const ;
#endif
    virtual CSMESFile::instrumentation_t instrumentationType() const =0;
    void writePatchArea(int index,long s_line_absolute, long s_column, long e_line_absolute, long e_column,long depth);
    const instrumentation_option_t &getOption() const { return instrumentation_option; }
    const char *file() const { return _file; }
    const _childrens_t & children() const { return _childrens; }
    Expression * parent() const { return _parent; }
    void setParent(Expression *);
private:
    void operator=(const Expression &);
    Expression(const Expression &);
    friend class TestExpressions;
    friend class TestExpression;
    friend class TestFilterFunction;
protected:
    bool write(instrumentation_type_t t, const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth);
    void addChildren(Expression *);
    void clearTree();
    code_style_t _code_style;
    long _index;

    _childrens_t _childrens;
    Expression * _parent;
    const char *_file;
    instrumentation_type_t type;
    instrumentation_option_t instrumentation_option;

    struct _location_area_t
    {
      long end_column;
      long end_depth;
      long end_line;
      long end_line_absolute;
      long start_column;
      long start_depth;
      long start_line;
      long start_line_absolute;
    };

    std::vector<_location_area_t> _patch_location;
    _location_area_t _location_pre,_location_org;

private:
    static StringPool filenames_pool; //!< Use only to save the string name
};


bool Expression::operator==(const Expression &b) const
{
    return (startLineAbsolute()==b.startLineAbsolute())
           && (startColumnPre()==b.startColumnPre())
           && (startColumnOrg()==b.startColumnOrg())
           && (endLineAbsolute()==b.endLineAbsolute())
           && (startLinePre()==b.startLinePre())
           && (startLineOrg()==b.startLineOrg())
           && (endLinePre()==b.endLinePre())
           && (endLineOrg()==b.endLineOrg())
           && (endColumnOrg()==b.endColumnOrg())
           && (endColumnPre()==b.endColumnPre())
           && (type==b.type) ;
}


bool Expression::start_lt(long val_b_hi,long val_b_lo) const
{
    long val_a_hi = startLineAbsolute();
    long val_a_lo = startColumnPre();

    if (val_a_hi==val_b_hi)
        return val_a_lo < val_b_lo;
    else
        return val_a_hi < val_b_hi;
}


inline bool start_eq(const Expression &a,const Expression &b)
{
    long val_a_hi = a.startLineAbsolute();
    long val_a_lo = a.startColumnPre();
    long val_b_hi = b.startLineAbsolute();
    long val_b_lo = b.startColumnPre();

    return (val_a_hi==val_b_hi) && (val_a_lo == val_b_lo) && (a.type==b.type) ;
}

inline bool start_gt(const Expression &a,const Expression &b)
{
  long val_a_hi = a.startLineAbsolute();
  long val_a_lo = a.startColumnPre();
  long val_b_hi = b.startLineAbsolute();
  long val_b_lo = b.startColumnPre();

  if (val_a_hi==val_b_hi)
  {
    if (val_a_lo==val_b_lo)
    {
      if (a.endLineAbsolute() == b.endLineAbsolute()) 
      {
        if (a.endColumnPre()==b.endColumnPre())
          return a.type > b.type ;
        else
          return (a.endColumnPre()<b.endColumnPre());
      }
      else
        return (a.endLineAbsolute()<b.endLineAbsolute());
    }   
    else
      return val_a_lo > val_b_lo;
  }
  else
    return val_a_hi > val_b_hi;
}

bool Expression::end_gt(long val_b_hi,long val_b_lo) const
{
    long val_a_hi = endLineAbsolute();
    long val_a_lo = endColumnPre();

    if (val_a_hi==val_b_hi)
        return val_a_lo > val_b_lo;
    else
        return val_a_hi > val_b_hi;
}

inline bool end_gt(const Expression &a,const Expression &b)
{
  long val_a_hi = a.endLineAbsolute();
  long val_a_lo = a.endColumnPre();
  long val_b_hi = b.endLineAbsolute();
  long val_b_lo = b.endColumnPre();

  if (val_a_hi==val_b_hi)
  {
    if (val_a_lo==val_b_lo)
    {
      if (a.startLineAbsolute() == b.startLineAbsolute()) 
      {
        if (a.startColumnPre()==b.startColumnPre())
          return a.type < b.type ;
        else
          return (a.startColumnPre()<b.startColumnPre());
      }
      else
        return (a.startLineAbsolute()<b.startLineAbsolute());
    }  
    else
      return val_a_lo > val_b_lo;
  }
  else
    return val_a_hi > val_b_hi;
}

#endif
