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

#ifndef COMPILER_H
#define COMPILER_H
#include "compilerinterface.h"
#include <time.h>
class Option;
class LibGen ;
class Functions;
class Expressions;
class Identifiers;
class Source;
class LinePos;
class CSMESFile;
class Defines;
class TestLibGen;

class Compiler
{
    public:

        Compiler(Option &);
        virtual ~Compiler();

    public:
        bool generate();
        const char *error();
        void display_temporary_files() const { compiler_p->display_temporary_files() ; }

    private:
        friend class TestLibGen;
        bool generate_one();
        bool compile_and_link_all_files(bool compile_cs_library,bool no_cslib_generation);
        bool compile_file();
        bool add_missing_sources(CSMESFile &) ;
        bool add_instrumentation_to_object_files(CSMESFile &) ;
        bool add_instrumentation_to_source_files(CSMESFile &,bool preprocess_background) ;
        bool analyse(const char *filename_orig,const char *finput,Expressions &expressions,Source &source,LinePos &filename,Functions& function,const Option &opt_p,const Defines &,const CompilerInterface &, int (*yy_input_func)(char *,int));
        time_t modification_time(const char *name);

        CompilerInterface *compiler_p;
        int argc;
        char **argv;
        LibGen *cslib_p ;
        LibGen *cslib() ;
        bool addmeasures(const char  *filename,bool import_symbols,bool import_library,CSMESFile &csmes_file,std::string &errmsg);
        const Option *option_p;
};

#endif
