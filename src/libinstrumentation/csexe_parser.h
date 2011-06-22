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

#ifndef CSEXE_PARSE_INCLUDE_H
#define CSEXE_PARSE_INCLUDE_H

typedef int csexe_t;

#if !defined(LOG) 
#include "csexe_parser_yacc_release.hxx"
#else
#include "csexe_parser_yacc_debug.hxx"
#endif
#include <stdio.h>

/* debug function for bison */
int csexe_yyprint(FILE *f,int type,YYSTYPE value);
#define  YYPRINT(file,type,value) csexe_yyprint(file,type,value);

extern char *csexe_input_buffer;
extern int   csexe_input_buffer_sz;
void init_csexe_parserlex();
int csexe_parsererror(const char *s);
#endif


