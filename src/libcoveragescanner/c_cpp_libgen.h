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

#ifndef CPPLIBGEN_H
#define CPPLIBGEN_H
#include "option.h"
#include "libgen.h"
#include <stdio.h>
class FileName;
class CompilerInterface;
class Option ;
class CppLibGen : public LibGen
{
public:

    CppLibGen();
    virtual ~CppLibGen();

    void generate(CompilerInterface &i,const char *default_csexe,bool lock_csexe);
    std::string csmes_library_source_name() const;
#if LOG
    void PDEBUG() const;
#endif
private:
    void save_source(const char *,const CompilerInterface &,const char *default_csexe,bool lock_csexe);
};
#endif
