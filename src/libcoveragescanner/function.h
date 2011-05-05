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

#ifndef FUNCTION_H
#define FUNCTION_H
#include "coveragescannerpdef.h"

class Source;
class Expression;

class Function
{
  public:
    Function();
    Function(const Function&);
    virtual ~Function();

    void setScopedName(const char*);
    const char * getScopedName() const;
    void setFileName(const char*f) { file_name=f; }
    const char * getFileName() const { return file_name; }
    void setToExclude(bool b) { to_exclude=b; }
    bool getToExclude() const { return to_exclude; }
    const Source * getSource() const;
    void setSource(const Source*);
    void getFunctionCode(long &s_line_abs,long &e_line_abs,long &s_line_orig,long &s_column_orig,long &e_line_orig,long &e_column_orig,long &s_line_pre,long &s_column_pre,long &e_line_pre,long &e_column_pre) const;
    void getFunctionPrototype(long &s_line_abs,long &e_line_abs,long &s_line_orig,long &s_column_orig,long &e_line_orig,long &e_column_orig,long &s_line_pre,long &s_column_pre,long &e_line_pre,long &e_column_pre) const;
    void setFunctionCode(long s_line_abs,long e_line_abs,long s_line_orig,long s_column_orig,long e_line_orig,long e_column_orig,long s_line_pre,long s_column_pre,long e_line_pre,long e_column_pre);
    void setFunctionPrototype(long s_line_abs,long e_line_abs,long s_line_orig,long s_column_orig,long e_line_orig,long e_column_orig,long s_line_pre,long s_column_pre,long e_line_pre,long e_column_pre);
    bool containsExpression(const Expression &e) const;
    const char*prototype() const;
    void clear();
    void pushScopedName(const char *n);
    void clearScopedName();

#if LOG
    void PDEBUG() const;
#endif
    void operator=(const Function &);
    bool operator==(const Function &) const;
  private:
    bool to_exclude;
    bool extractPrototype(char**) const;
    char *scoped_name;
    const char *file_name;
    const Source *source_p;
    long end_code_column_pre;
    long end_code_column_orig;
    long end_code_line_pre;
    long end_code_line_orig;
    long end_code_line_abs;
    long end_prototype_column_pre;
    long end_prototype_column_orig;
    long end_prototype_line_pre;
    long end_prototype_line_orig;
    long end_prototype_line_abs;
    long start_code_column_pre;
    long start_code_column_orig;
    long start_code_line_pre;
    long start_code_line_orig;
    long start_code_line_abs;
    long start_prototype_column_pre;
    long start_prototype_column_orig;
    long start_prototype_line_pre;
    long start_prototype_line_orig;
    long start_prototype_line_abs;
    mutable char *prototype_str;
};

#endif
