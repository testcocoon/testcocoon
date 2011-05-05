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

#ifndef COMPILERWRAPPERLANGUAGEOPTION_H
#define COMPILERWRAPPERLANGUAGEOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include "expression.h"
#include <string.h>

class TestCompilerWrapperLanguageOption;

class CompilerWrapperLanguageOption : public CompilerWrapperOption
{
  public:
    CompilerWrapperLanguageOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperLanguageOption();
    Expression::code_style_t value() const { return _value; }
    bool defined() const { return _value!=Expression::CODESTYLE_UNKNOWN; }
    void setValue(Expression::code_style_t v) { _value=v; }
    virtual std::list<std::string> profileValues() const ;
    void clear();

  protected:
    bool _parse(const char *field,const char *value);
    friend class TestCompilerWrapperLanguageOption;
  private:
    Expression::code_style_t _value;
};

#endif
