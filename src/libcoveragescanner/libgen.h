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

#ifndef LIBGEN_H
#define LIBGEN_H
#include "option.h"
#include <stdio.h>
class FileName;
class CompilerInterface;
class Option ;
class LibGen
{
public:

    LibGen();
    virtual ~LibGen();

    void append(const char*,bool import_symbols,unsigned long signature);
    void operator=(const LibGen &src);
    void clear();
    void generate(CompilerInterface &i,const char *default_csexe,bool lock_csexe,bool compile);
    virtual std::string csmes_library_source_name() const=0;
    bool isAppend(const char *text) const ;
    static void escape(const char*text,char *text_escape);
#if LOG
    void PDEBUG() const;
#endif
protected:
    virtual void save_source(const char *,const CompilerInterface &,const char *default_csexe,bool lock_csexe)=0;
    struct data_t { 
      unsigned long signature;
      char *filename; 
      bool import_symbols;
    };
    data_t *datas ;
    long nb_data;
    void fputs_trace(const char *str,FILE *f) const;
};
#endif
