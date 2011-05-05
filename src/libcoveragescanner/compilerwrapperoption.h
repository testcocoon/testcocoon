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

#ifndef COMPILERWRAPPEROPTION_H
#define COMPILERWRAPPEROPTION_H
#include <stdlib.h>
#include "coveragescannerpdef.h"
#include "debug.h"
#include <string.h>
#include <string>
#include <list>
class CompilerWrapperOptionPool ;
class CompilerWrapperOption
{
  public:
    CompilerWrapperOption(CompilerWrapperOptionPool &,const char *field_name);
    virtual ~CompilerWrapperOption();
    bool parse(const char *field,const char *value, bool override);
    virtual bool defined() const =0;
    const char * name() const { return _field_name; }
    const char * description() const { return _description; }
    virtual std::list<std::string> profileValues() const=0;
    virtual void clear() = 0;

  private:
    void operator=(const CompilerWrapperOption &);
    char *_field_name;
    CompilerWrapperOptionPool &_pool;
  protected:
    virtual bool _parse(const char *field,const char *value)=0;
    char *_description;
};

#endif
