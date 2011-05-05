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

#ifndef COMPILERWRAPPER_OPTION_POOL_H
#define COMPILERWRAPPER_OPTION_POOL_H
#include "compilerwrapperlistoption.h"
#include "compilerwrapperenumoption.h"
#include "expression.h"
#include "argv.h"
#include <list>
class CompilerWrapperOption;

class CompilerWrapperOptionPool
{
  std::list<CompilerWrapperOption*> options;

  public:
    CompilerWrapperOptionPool();
    virtual ~CompilerWrapperOptionPool();
    void add(CompilerWrapperOption *);
    void remove(CompilerWrapperOption *);
    bool parseprofile(const std::string & file,const char *architecture_option,const Argv &param_args) ;
#if LOG
    void PDEBUG() const;
#endif
    void clear();
  private:
    bool parse(const char *field,const char *value,bool override);
    CompilerWrapperListOption *table_architecture_arg_p; /*!< architecture */
    CompilerWrapperEnumOption *architecture_arg_append_p; //!< Architecture specifier (for embedded C++)

};

#endif
