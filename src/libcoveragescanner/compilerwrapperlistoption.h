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

#ifndef COMPILERWRAPPERLISTOPTION_H
#define COMPILERWRAPPERLISTOPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "compilerwrapperoption.h"
#include "debug.h"
#include <string.h>
#include <list>
class CompilerWrapperEnumOption ;
class TestCompilerWrapperListOption ;

class CompilerWrapperListOption : public CompilerWrapperOption
{
  public:
    CompilerWrapperListOption(CompilerWrapperOptionPool&, const char *field_name);
    virtual ~CompilerWrapperListOption();
    char*const * value() const { return _values; }
    bool defined() const { return _values!=NULL; }
    virtual std::list<std::string> profileValues() const;
    const char *operator[](int pos) const
    {
      if (_values)
        return _values[pos];
      else
        return NULL;
    }
    void clear();
    const char *  findOptionTable (const char *param,bool append)  const;
    const char * matchBooleanOption(const char *arg0,const char *arg1,const CompilerWrapperEnumOption &behaviour) const;

  protected:
    bool _parse(const char *field,const char *value);
  private:
    char **_values;
    static bool build_table(char **&table,const char *val);
    static void delete_table(char **table);
    static const char *  findOptionTable (char **const table,const char *param,bool append)  ;
    static const char * matchBooleanOption_item(const char *arg0,const char *arg1,const char *option,const CompilerWrapperEnumOption &behaviour) ;
    static const char * matchBooleanOption_table(const char *arg0,const char *arg1,char ** const option_table,const CompilerWrapperEnumOption &behaviour) ;
    friend class TestCompilerWrapperListOption ;
};

#endif
