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

#ifndef COMPILERWRAPPERENUMOPTION_H
#define COMPILERWRAPPERENUMOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include <string.h>
class TestCompilerWrapperEnumOption;

class CompilerWrapperEnumOption : public CompilerWrapperOption
{
  public:
    enum boolean_option_t { OPT_UNDEFINED=-1, OPT_YES=1, OPT_NO=0, OPT_ALLOW=2, OPT_NONE=3, OPT_GNU=4, OPT_MS=5, OPT_POSIX=6 , OPT_MSCE=7, OPT_CPP=8 } ;
    CompilerWrapperEnumOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperEnumOption();
    boolean_option_t value() const { return _value; }
    bool defined() const { return value()!=OPT_UNDEFINED; }
    void setValue(boolean_option_t v) { _value=v; }
    virtual std::list<std::string> profileValues() const ;
    void clear();

  protected:
    bool _parse(const char *field,const char *value);
  private:
    boolean_option_t _value;
    friend class TestCompilerWrapperEnumOption;
};

#endif
