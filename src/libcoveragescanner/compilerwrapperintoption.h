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

#ifndef COMPILERWRAPPERINTOPTION_H
#define COMPILERWRAPPERINTOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include <string.h>
class TestCompilerWrapperIntOption;

class CompilerWrapperIntOption : public CompilerWrapperOption
{
  public:
    CompilerWrapperIntOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperIntOption();
    int value() const { return _value; }
    bool defined() const { return _defined; }
    void setValue(int v) { _value=v; }
    virtual std::list<std::string> profileValues() const ;
    void clear();

  protected:
    bool _parse(const char *field,const char *value);
  private:
    int _value;
    bool _defined;
    friend class TestCompilerWrapperIntOption;
};

#endif
