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

#ifndef COMPILERWRAPPERTEXTOPTION_H
#define COMPILERWRAPPERTEXTOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include <string.h>
#include <string>
class TestCompilerWrapperTextOption;

class CompilerWrapperTextOption : public CompilerWrapperOption
{
  public:
    CompilerWrapperTextOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperTextOption();
    const char *value() const { return _value; }
    bool defined() const { return value()!=NULL; }
    void setValue(const char *v) { FREE(_value);_value=STRDUP(v); }
    void setValue(const std::string &v) { setValue(v.c_str()); }
    virtual std::list<std::string> profileValues() const ;
    void clear();

  protected:
    bool _parse(const char *field,const char *value);
  private:
    friend class TestCompilerWrapperTextOption;
    char *_value;
};

#endif
