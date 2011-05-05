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

#ifndef COMPILERWRAPPERREGEXPOPTION_H
#define COMPILERWRAPPERREGEXPOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include "regexpr.h"
#include <string.h>

class TestCompilerWrapperRegExpOption;

class CompilerWrapperRegExpOption : public CompilerWrapperOption
{
  public:
    CompilerWrapperRegExpOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperRegExpOption();
    const char *value() const { return _value; }
    bool defined() const { return value()!=NULL; }
    bool match(const char*) const ;
    void setValue(const char *v) ;
    virtual std::list<std::string> profileValues() const;
    void clear();

  protected:
    bool _parse(const char *field,const char *value);
    friend class TestCompilerWrapperRegExpOption;
  private:
    char *_value;
    regexp *_regexp ;
};

#endif
