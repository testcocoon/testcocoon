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

%{
#include "csexe_parser.h"
#include <string.h>
#include "debug.h"
#define yyerror csexe_parsererror
#if OS_WIN32
#pragma warning(disable : 4065)
#endif
extern int csexe_parserlex (void);
void init_csexe_parser_lex();


%}
%name-prefix="csexe_parser"
%union
{
 csexe_t csexe;
}
%locations
%verbose
/*%error-verbose*/
/*%debug*/

%token __NUMBER__
%token __STRING__


%start csexe_parser 


%%

csexe_parser : csexe_measurements
             | /* empty */
             ;

csexe_measurements : csexe_measurement 
                   | csexe_measurement csexe_measurements
                   ;

csexe_measurement : csexe_titles csexe_start_banner
                  | csexe_start_banner
                  ;

csexe_start_banner: '#' __STRING__ '\n' csexe_instrumentations
                  | '#' __STRING__ '\n' csexe_instrumentations csexe_status
                  | '#' __STRING__ '\n' csexe_status
                  | '#' __STRING__ '\n' 
                  ;

csexe_instrumentations: csexe_instrumentation
                      | csexe_instrumentation csexe_instrumentations
                      ;

csexe_instrumentation: '/' nb_mes ':' signature ':' module_name '\n' module_instrumentation
                     ;

nb_mes: __NUMBER__ 
      ;

signature: __NUMBER__ 
      ;

module_name: title
           ;

module_instrumentation: '\\' instrumentations '\n'
                      | '\\' '\n'
                      ;

instrumentations: instrumentation 
                | instrumentation instrumentations
                ;
instrumentation: __NUMBER__
               | '+'
               | '-'
               | '?'
               ;

csexe_status: csexe_one_status
            | csexe_one_status csexe_status
            ;

csexe_one_status : '!' title '\n'
                  | '!' '\n'
                  ;

csexe_titles: csexe_title
            | csexe_title csexe_titles
            ;

csexe_title : '*' title '\n'
            | '*' '\n'
            ;

title: title_char
     | title_char title
     ;

title_char: '+' | '?' | '-' | '*' | ':' | '!' |  '/' | '\\' | __NUMBER__ | __STRING__ ;

%%

#include <stdio.h>

int csexe_parsererror(const  char *s)
{
  //fprintf(stderr,"file:%s line:%i column:%i %s\n",filename,line,column,s);
  return 0;
}

int yyparse();
int csexe_init()
{
  int ret=0;
  init_csexe_parserlex();
#if YYDEBUG
  yydebug=1;
#endif
  return ret;
}

int csexe_parse(const char *text_line)
{
  int ret;
  int old_lg=csexe_input_buffer_sz;
  csexe_input_buffer_sz+=strlen(text_line);
  csexe_input_buffer=(char*)REALLOC(csexe_input_buffer,csexe_input_buffer_sz+1);
  strcpy(&csexe_input_buffer[old_lg],text_line);
  DEBUG2("Start parsing:#%s\n",text_line);
  ret=yyparse();
  DEBUG3("End parsing(ret=%i):#%s\n",ret,text_line);
  return ret;
}

int csexe_yyprint(FILE *f,int /*type*/,YYSTYPE value)
{
  //yyprint_token(f,value.token.text);
  return 0;
}

