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
#include "parsemisc.h"
#include "preprocessor_parser.h"
#include "defines.h"
#include <list>
#define yyerror preprocessor_parsererror
#if OS_WIN32
#pragma warning(disable : 4065)
#endif
extern int preprocessor_parserlex (void);
void init_preprocessor_parser_lex();


extern Defines preprocessor_defines;
struct _preprocessor_area_t
{
  bool not_ignore;
  bool evaluated;
};
extern std::list<_preprocessor_area_t> _preprocessor_area;
%}
%name-prefix="preprocessor_parser"
%union
{
 token_t        token;
 preprocessor_t preprocessor;
}
%locations
%verbose
/*%error-verbose*/
/*%debug*/

%token __IDENTIFIER__
%token __DEFINE__
%token __UNDEF__
%token __IF__
%token __ELIF__
%token __ELSE__
%token __ENDIF__
%token __TRUE__
%token __FALSE__
%token __NOT__
%token __EQUAL__
%token __NOT_EQUAL__
%token __OR__
%token __AND__
%token __NUMBER__
%token __STRING__

%type <preprocessor> preprocessor_primary_item preprocessor_conditional_and_expression preprocessor_expression preprocessor_primary_expression
%type <preprocessor> preprocessor_conditional_or_expression preprocessor_equality_expression preprocessor_unary_expression

%start preprocessor 


%%

preprocessor : preprocessor_define
             | preprocessor_undef
             | preprocessor_if_expr
             | preprocessor_else_expr
             | preprocessor_elif_expr
             | preprocessor_endif_expr
             | preprocessor_other_expr
            ;

preprocessor_define : __DEFINE__ __IDENTIFIER__
        { preprocessor_defines.define($<token>2.text); }
       ;

preprocessor_undef : __UNDEF__ __IDENTIFIER__
        { preprocessor_defines.undef($<token>2.text); }
       ;

preprocessor_endif_expr : __ENDIF__ 
          { _preprocessor_area.pop_back(); }
           ;

preprocessor_if_expr : __IF__ preprocessor_expression
        { 
          _preprocessor_area_t a;
          a.not_ignore=$2.value;
          a.evaluated=$2.value;
          _preprocessor_area.push_back(a) ; }
        ;

preprocessor_elif_expr : __ELIF__ preprocessor_expression
        { 
          if (!_preprocessor_area.back().evaluated)
          {
            if ($2.value)
            {
              _preprocessor_area.back().not_ignore=true;
              _preprocessor_area.back().evaluated=true;
            }
            else
            {
              _preprocessor_area.back().not_ignore=false;
              _preprocessor_area.back().evaluated=false;
            }
          }
          else
          {
            _preprocessor_area.back().not_ignore=false ; 
            _preprocessor_area.back().evaluated=true;
          }
        }
        ;

preprocessor_else_expr : __ELSE__ 
        { 
          if (!_preprocessor_area.back().evaluated)
            _preprocessor_area.back().not_ignore=true;
          else
            _preprocessor_area.back().not_ignore=false ; 
        }
        ;

preprocessor_primary_expression: '(' preprocessor_expression ')'
                  { $$=$2; }
                  | preprocessor_primary_item
                  { $$=$1; }
                  ;
preprocessor_primary_item:  __IDENTIFIER__
               { $$.value=preprocessor_defines.isDefined($<token>1.text); }
            | __TRUE__
               { $$.value=true; }
            | __FALSE__
               { $$.value=false; }
            ;

preprocessor_unary_expression: preprocessor_primary_expression
                  { $$=$1; }
                | '!' preprocessor_unary_expression
                  { $$.value =$2.value; }
                ;

preprocessor_equality_expression : preprocessor_unary_expression
                  { $$=$1; }
                    | preprocessor_equality_expression __EQUAL__ preprocessor_unary_expression
                  { $$.value =($1.value==$3.value); }
                    | preprocessor_equality_expression __NOT_EQUAL__ preprocessor_unary_expression
                  { $$.value =($1.value!=$3.value); }
                    ;

preprocessor_conditional_and_expression : preprocessor_equality_expression
                  { $$=$1; }
                           | preprocessor_conditional_and_expression __AND__ preprocessor_equality_expression
                  { $$.value =($1.value && $3.value); }
                           ;

preprocessor_conditional_or_expression : preprocessor_conditional_and_expression
                  { $$=$1; }
                          | preprocessor_conditional_or_expression __OR__ preprocessor_conditional_and_expression
                  { $$.value =($1.value || $3.value); }
                          ;
preprocessor_expression: preprocessor_conditional_or_expression
                  { $$=$1; }
          ;
preprocessor_other_expr : preprocessor_primary_expression preprocessor_any_exprs_opt;
           ;
preprocessor_any_exprs_opt : /* empty */
              | preprocessor_any_exprs
              ;

preprocessor_any_exprs : preprocessor_any_expr
          | preprocessor_any_exprs preprocessor_any_expr
          ;

preprocessor_any_expr: __IDENTIFIER__
        | __DEFINE__
        | __UNDEF__
        | __IF__
        | __ELIF__
        | __ELSE__
        | __ENDIF__
        | __TRUE__
        | __FALSE__
        | __NOT__
        | __EQUAL__
        | __NOT_EQUAL__
        | __OR__
        | __AND__
        | '('
        | ')'
        ;
%%

#include <stdio.h>

Defines preprocessor_defines;
std::list<_preprocessor_area_t> _preprocessor_area;
static const char*error_format=NULL;
int preprocessor_parsererror(const  char *s)
{
  std::string m=parser_error( error_format, filename, line, column, s);
  fprintf(stderr,"%s\n",m.c_str());
  return 0;
}

int yyparse();
int preprocessor_init(const Defines &defines,const char *err_format)
{
  int ret=0;
  _preprocessor_area.clear();
  preprocessor_defines=defines;
  error_format=err_format;
  init_preprocessor_parserlex();
#if YYDEBUG
  yydebug=1;
#endif
  return ret;
}

int preprocessor_parse(const char *text_line)
{
  int ret;
  int old_lg=preprocessor_input_buffer_sz;
  preprocessor_input_buffer_sz+=strlen(text_line);
  preprocessor_input_buffer=(char*)REALLOC(preprocessor_input_buffer,preprocessor_input_buffer_sz+1);
  strcpy(&preprocessor_input_buffer[old_lg],text_line);
  DEBUG2("Start parsing:#%s\n",text_line);
  ret=yyparse();
  DEBUG3("End parsing(ret=%i):#%s\n",ret,text_line);
  return ret;
}

int preprocessor_yyprint(FILE *f,int /*type*/,YYSTYPE value)
{
  yyprint_token(f,value.token.text);
  return 0;
}

bool preprocessor_ignore_area()
{
  bool no_ignore=true;
  for (std::list<_preprocessor_area_t>::const_iterator it=_preprocessor_area.begin();it!=_preprocessor_area.end();++it)
    no_ignore= no_ignore && it->not_ignore;
  DEBUG2("preprocessor_ignore_area=%s\n",(no_ignore?"false":"true"));
  return !no_ignore;
}
