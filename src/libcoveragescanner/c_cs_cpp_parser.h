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

#ifndef C_CPP_CPARSE_INCLUDE_H
#define C_CPP_CPARSE_INCLUDE_H

#include "parser.h"

#if !defined(LOG) 
#include "c_cs_cpp_parser_yacc_release.hxx"
#else
#include "c_cs_cpp_parser_yacc_debug.hxx"
#endif

/* debug function for bison */
int c_cs_cpp_yyprint(FILE *f,int type,YYSTYPE value);
#define  YYPRINT(file,type,value) c_cs_cpp_yyprint(file,type,value);

extern FILE *test_input_file;
void init_c_cs_cpp_parserlex(CompilerInterface::extension_t _parse_language, int (*yy_input_func)(char *,int),const char *filename_orig);
int c_cs_cpp_parsererror(const  char *s);
extern int c_cs_cpp_parserlex (void);
extern int bracket_level;
void c_cs_cpp_parser_enter_set_get();
void c_cs_cpp_parser_exit_set_get();
extern bool in_template_definition;
#endif


