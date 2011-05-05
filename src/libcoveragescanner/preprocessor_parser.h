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

#ifndef PREPROCESSOR_CPARSE_INCLUDE_H
#define PREPROCESSOR_CPARSE_INCLUDE_H

#include "parser.h"
#include "defines.h"

#if !defined(LOG) 
#include "preprocessor_parser_yacc_release.hxx"
#else
#include "preprocessor_parser_yacc_debug.hxx"
#endif

/* debug function for bison */
int preprocessor_yyprint(FILE *f,int type,YYSTYPE value);
#define  YYPRINT(file,type,value) preprocessor_yyprint(file,type,value);

extern char *preprocessor_input_buffer;
extern int   preprocessor_input_buffer_sz;
void init_preprocessor_parserlex();
int preprocessor_parsererror(const char *s);
#endif


