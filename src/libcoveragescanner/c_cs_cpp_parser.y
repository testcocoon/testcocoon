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
#include "c_cs_cpp_parser.h"
#define yyerror c_cs_cpp_parsererror
static Scope *kr_function_def_scope_p=NULL;
static YYLTYPE kr_function_def_pos;
static int level=0;
#define YYMAXDEPTH 100000
#define YYINITDEPTH 1000
#ifndef NO_DEBUG
#define DUMP_SCOPE(sc) DEBUG2("Scoped Name:%s\n",sc?sc->fullName().c_str():"(null)")
#else
#define DUMP_SCOPE(X)
#endif
#if OS_WIN32
#pragma warning(disable : 4065)
#endif
%}
%name-prefix="c_cs_cpp_parser"
%union
{
    token_t                         token                     ; 
    compound_t                      compound                  ; 
    exit_function_t                 exit_function             ; 
    qualified_identifier_t          qualified_identifier      ; 
    qualified_identifier_func_t     qualified_identifier_func ; 
    definition_decl_t               definition_decl           ; 
    logical_expr_t                  logical_expr              ; 
    function_statement_expr_t     function_statement_expr ; 
}
%locations
%verbose
%expect 2
/*%error-verbose*/
/*%debug*/

%token  _IDENTIFIER_  _STRUCT_ _EXTERN_ _PRIVATE_ 
%token  _AND_OP_ _OR_OP_ 
%token  _CASE_ _DEFAULT_ _IF_ _ELSE_ _SWITCH_ _WHILE_ _DO_ _FOR_ _GOTO_ _CONTINUE_ _BREAK_ _RETURN_ _THROW_
%token  _SOMETHING_ _SOME_BINARY_OPERATOR_ _QUALIF_ _STATIC_
%token  _TRY_ _CATCH_
%token  _ASM_ _TOKEN_NOT_FOUND_ _TOKEN_TO_IGNORE_ _ASM_MS_LINE_

/* C# tokens */
%token _UNSAFE_ _FINALLY_ _FOREACH_ _LOCK_ _USING_ _SET_GET_ _DELEGATE_ _YIELD_

%type <logical_expr> logical_expr logical_not_expr logical_binary_operator_high_prio_expr logical_expr_list logical_assignment_expr logical_and_expr logical_or_expr logical_cmp_expr logical_expr_first logical_expr_items

%type <compound>    compound_statement statements statement statements_opt compound_statement_record
                    selection_statement iteration_statement foreach_statement statement_record substatement
                    lock_statement unsafe_statement expression_or_function_statement jump_statement return_statement

%type <definition_decl>  definition_decl 
%destructor { if ($$.scoped_name_p) delete $$.scoped_name_p; } definition_decl

%type <qualified_identifier>  qualified_identifier_declation
                              _qualified_identifier_declaration
                              qualified_identifier_statement
%destructor { if ($$.scoped_name_p) delete $$.scoped_name_p; } qualified_identifier_statement
                                                               _qualified_identifier_declaration
                                                               qualified_identifier_declation

%type <qualified_identifier_func>  expression_statement_no_equal_no_parentesis
                                   expression_statement_no_equal_no_parentesis_first_item 
                                   expression_statement_no_equal_no_parentesis_second_item
                                   _expression_statement_no_equal_no_parentesis_ 
                                   _expression_statement_no_equal_no_parentesis_item_ 
%destructor { if ($$.scoped_name_p) delete $$.scoped_name_p; } expression_statement_no_equal_no_parentesis
                                                               expression_statement_no_equal_no_parentesis_first_item
                                                               expression_statement_no_equal_no_parentesis_second_item
                                                               _expression_statement_no_equal_no_parentesis_ 
                                                               _expression_statement_no_equal_no_parentesis_item_ 

%type <qualified_identifier_func>   definition_expr_no_parenthesis 
                                    _definition_expr_no_parenthesis_no_double_quote_
                                    _definition_expr_no_parenthesis_no_double_quote_first
                                    _definition_expr_no_parenthesis_no_double_quote_rest
                                    _definition_item_no_parenthesis_no_double_quote_
%destructor { if ($$.scoped_name_p) delete $$.scoped_name_p; } definition_expr_no_parenthesis 
                                                               _definition_expr_no_parenthesis_no_double_quote_
                                                               _definition_item_no_parenthesis_no_double_quote_
                                                               _definition_expr_no_parenthesis_no_double_quote_first
                                                               _definition_expr_no_parenthesis_no_double_quote_rest

%type <exit_function> expression_statement function_statement expression_statement_expr
%type <function_statement_expr> function_statement_expr
%destructor { if ($$.scoped_name_p) delete $$.scoped_name_p; } function_statement_expr 

%type <token>  identifier 
%type <token> _IDENTIFIER_ '=' '?' _AND_OP_ _OR_OP_ _SOMETHING_ _SOME_BINARY_OPERATOR_ _SET_GET_ 

%start file
%%

/* declarations */
file : definitions_opt
     ;

definitions_opt: /* empty */
               | definitions 
               | definition_decl
               | definitions definition_decl
               ;

definitions: definition
            | definitions  definition
            ;


definition_decl: definition_expr_no_parenthesis definition_parentesis definition_expr_opt 
                {
                  if ($1.scoped_name_p)
                  {
                    YYLTYPE proto;
                    proto.first_line=@1.first_line;
                    proto.first_column=@1.first_column;
                    proto.last_line=@2.last_line;
                    proto.last_column=@2.last_column;
                    $$.pos=proto;
                    $$.scoped_name_p=$1.scoped_name_p;
                    $1.scoped_name_p=NULL; // ownner is now definition_decl
                  }
                }
              | private_or_brakets_defs definition_expr_no_parenthesis definition_parentesis definition_expr_opt 
                {
                  if ($2.scoped_name_p)
                  {
                    YYLTYPE proto;
                    proto.first_line=@2.first_line;
                    proto.first_column=@2.first_column;
                    proto.last_line=@3.last_line;
                    proto.last_column=@3.last_column;
                    $$.pos=proto;
                    $$.scoped_name_p=$2.scoped_name_p;
                    $2.scoped_name_p=NULL; // ownner is now definition_decl
                  }
                }
               | private_or_brakets_defs definition_parentesis definition_expr_opt 
                { $$.scoped_name_p=NULL; }
               | private_or_brakets_defs definition_expr_no_parenthesis 
                { $$.scoped_name_p=NULL; }
               | private_or_brakets_defs definition_expr_no_parenthesis '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               | private_or_brakets_defs definition_parentesis definition_expr_opt '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               | private_or_brakets_defs definition_expr_no_parenthesis definition_parentesis definition_expr_opt '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               | definition_parentesis definition_expr_opt 
                { $$.scoped_name_p=NULL; }
               | private_or_brakets_defs 
                { $$.scoped_name_p=NULL; }
               | definition_expr_no_parenthesis 
                { $$.scoped_name_p=NULL; }
               | definition_expr_no_parenthesis '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               | definition_parentesis definition_expr_opt '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               | definition_expr_no_parenthesis definition_parentesis definition_expr_opt '=' definition_decl_any
                { $$.scoped_name_p=NULL; }
               ;

definition : private_or_brakets_defs definition_expr_no_parenthesis definition_parentesis definition_expr_opt compound_statement_record
            {
               if ($2.scoped_name_p)
               {
                 YYLTYPE proto;
                 proto.first_line=@2.first_line;
                 proto.first_column=@2.first_column;
                 proto.last_line=@3.last_line;
                 proto.last_column=@3.last_column;
                 record_function($2.scoped_name_p,&proto,&@5);
                if (is_function_name_no_scope($2.scoped_name_p,"Main"))
                {
                  YYLTYPE header;
                  header.first_line=@1.first_line;
                  header.first_column=@1.first_column;
                  header.last_line=@4.last_line;
                  header.last_column=@4.last_column;
                  record_main_function(&@$,&header,&@5);
                }
               }
            }
            | definition_expr_no_parenthesis definition_parentesis definition_expr_opt compound_statement_record
            {
               if ($1.scoped_name_p)
               {
                 YYLTYPE proto;
                 proto.first_line=@1.first_line;
                 proto.first_column=@1.first_column;
                 proto.last_line=@2.last_line;
                 proto.last_column=@2.last_column;
                 record_function($1.scoped_name_p,&proto,&@4);
                if (is_function_name_no_scope($1.scoped_name_p,"Main"))
                {
                  YYLTYPE header;
                  header.first_line=@1.first_line;
                  header.first_column=@1.first_column;
                  header.last_line=@3.last_line;
                  header.last_column=@3.last_column;
                  record_main_function(&@$,&header,&@4);
                }
               }
            }
           | private_or_brakets_defs definition_expr_no_parenthesis set_get
           | definition_expr_no_parenthesis set_get
           | private_or_brakets_defs definition_expr_no_parenthesis delegate_decl ';'
           | definition_expr_no_parenthesis delegate_decl ';'
           | private_or_brakets_defs delegate_decl ';'
           | delegate_decl ';'
           | set_get
           | private_or_brakets_defs set_get
           | definition_decl ';'
            {
              if ($1.scoped_name_p && (bracket_level==0))
              {
                if (kr_function_def_scope_p)
                  delete kr_function_def_scope_p;
                kr_function_def_scope_p=$1.scoped_name_p;
                kr_function_def_pos=$1.pos;
                $1.scoped_name_p=NULL; // changing owner
              }
            }
           | private_or_brakets_defs definition_parentesis definition_expr_opt compound_statement_record
            {
            }
           | definition_parentesis definition_expr_opt compound_statement_record
            {
            }
           | ';'
           | compound_statement_record
            {
              if (kr_function_def_scope_p)
              {
                  record_function(kr_function_def_scope_p,&kr_function_def_pos,&@1);
                  delete kr_function_def_scope_p;
                  kr_function_def_scope_p=NULL;
              }
            }
           | private_or_brakets_defs definition_expr_no_parenthesis '{' 
            { 
              if ($2.scoped_name_p)
                pushCurrentScope($2.scoped_name_p)
            }
             definitions_opt '}'
            {
              if ($2.scoped_name_p)
                popCurrentScope();
            }
           | definition_expr_no_parenthesis '{' 
            { 
              if ($1.scoped_name_p)
                pushCurrentScope($1.scoped_name_p)
            }
             definitions_opt '}'
            {
              if ($1.scoped_name_p)
                popCurrentScope();
            }

           ;


private_or_brakets_defs: private_or_brakets_defs_item
                       | private_or_brakets_defs private_or_brakets_defs_item
                       ;

private_or_brakets_defs_item: private_def
                            | brackets_expr
                            ;

private_def: _PRIVATE_
                 | _PRIVATE_ ':'
                 ;

set_get: _SET_GET_ definition_expr_opt compound_statement_record
         {
            Scope sc;
            sc.addName($1.text);
            record_function(&sc,&@1,&@3);
         }
       | _SET_GET_ definition_expr_opt ';'
       ;

definition_parentesis: '(' definition_parentesis_items_opt ')'
                     ;

definition_decl_any_opt: /* empty */
                   | definition_decl_any
                   ;

definition_decl_any: definition_decl_any_item
                   | definition_decl_any definition_decl_any_item
                   ;

definition_decl_any_item: definition_parentesis
                          | '{' definition_decl_any '}'
                          | '<' 
                          | '>'
                          | brackets_expr
                          | '{' '}'
                          | _PRIVATE_
                          | _AND_OP_
                          | _OR_OP_
                          | '!'
                          | _CASE_
                          | _DEFAULT_
                          | _IF_
                          | _ELSE_
                          | _STRUCT_
                          | _EXTERN_
                          | _SWITCH_
                          | _WHILE_
                          | _UNSAFE_
                          | _DO_
                          | _FOR_
                          | _GOTO_
                          | _CONTINUE_
                          | _BREAK_
                          | _RETURN_
                          | _THROW_
                          | _SOMETHING_
                          | _STATIC_
                          | _SOME_BINARY_OPERATOR_
                          | _TRY_
                          | _USING_
                          | _IDENTIFIER_
                          | _SET_GET_
                          | _QUALIF_
                          | _CATCH_
                          | _ASM_
                          | _TOKEN_NOT_FOUND_
                          | _TOKEN_TO_IGNORE_
                          | delegate_expr
                          | ':'
                          | '='
                          | ','
                          | inline_cmd_part
                          ;

definition_parentesis_items_opt: /* empty */
                               | definition_parentesis_items
                               ;

definition_parentesis_items: definition_parentesis_item
                           | definition_parentesis_items definition_parentesis_item
                           ;

definition_parentesis_item: definition_parentesis
                          | '{' definition_parentesis_items_opt '}'
                          | '<' 
                          | '>'
                          | brackets_expr
                          | delegate_expr
                          | ';'
                          | _PRIVATE_
                          | _AND_OP_
                          | _OR_OP_
                          | '!'
                          | _CASE_
                          | _DEFAULT_
                          | _IF_
                          | _ELSE_
                          | _STRUCT_
                          | _EXTERN_
                          | _SWITCH_
                          | _WHILE_
                          | _DO_
                          | _FOR_
                          | _GOTO_
                          | _CONTINUE_
                          | _BREAK_
                          | _RETURN_
                          | _THROW_
                          | _SOMETHING_
                          | _STATIC_
                          | _SOME_BINARY_OPERATOR_
                          | _TRY_
                          | _USING_
                          | _IDENTIFIER_
                          | _SET_GET_
                          | _QUALIF_
                          | _CATCH_
                          | _ASM_
                          | _UNSAFE_
                          | _TOKEN_NOT_FOUND_
                          | _TOKEN_TO_IGNORE_
                          | ':'
                          | '='
                          | ','
                          | inline_cmd_part
                          ;

definition_expr_opt: /*empty */
                   | definition_expr
                   ;

definition_expr: definition_item
               |  definition_expr definition_item 
               ;

definition_item : qualified_identifier_declation
                | _PRIVATE_
                | definition_parentesis
                | brackets_expr
                | _AND_OP_
                | _OR_OP_
                | _CASE_
                | _DEFAULT_
                | _IF_
                | _ELSE_
                | _STRUCT_
                | _EXTERN_
                | _SWITCH_
                | _WHILE_
                | _DO_
                | _FOR_
                | _GOTO_
                | _CONTINUE_
                | _BREAK_
                | _RETURN_
                | _THROW_
                | _SOMETHING_
                | _STATIC_
                | _SOME_BINARY_OPERATOR_
                | _TRY_
                | _USING_
                | _CATCH_
                | _ASM_
                | _TOKEN_NOT_FOUND_
                | _TOKEN_TO_IGNORE_
                | ':'
                | '!'
                | ','
                | inline_cmd_part
                ;

definition_expr_no_parenthesis: _definition_expr_no_parenthesis_no_double_quote_
                                {
                                   $$.is_exit_function=$1.is_exit_function; 
                                   $$.scoped_name_p=$1.scoped_name_p;
                                   $1.scoped_name_p=NULL;
                                   DUMP_SCOPE($$.scoped_name_p);
                                }
                              | _definition_expr_no_parenthesis_no_double_quote_ ':'
                                {
                                   $$.is_exit_function=$1.is_exit_function; 
                                   $$.scoped_name_p=$1.scoped_name_p;
                                   $1.scoped_name_p=NULL;
                                }
                              | _definition_expr_no_parenthesis_no_double_quote_ ':' _definition_expr_no_parenthesis_
                                {
                                   $$.is_exit_function=$1.is_exit_function; 
                                   $$.scoped_name_p=$1.scoped_name_p;
                                   $1.scoped_name_p=NULL;
                                   DUMP_SCOPE($$.scoped_name_p);
                                }
                              ;

_definition_expr_no_parenthesis_no_double_quote_: _definition_expr_no_parenthesis_no_double_quote_first
                                                      {
                                                         $$.is_exit_function=$1.is_exit_function; 
                                                         $$.scoped_name_p=$1.scoped_name_p;
                                                         $1.scoped_name_p=NULL;
                                                         DUMP_SCOPE($$.scoped_name_p);
                                                      }
                                                | _definition_expr_no_parenthesis_no_double_quote_first _definition_expr_no_parenthesis_no_double_quote_rest
                                                      { 
                                                        $$.is_exit_function=$2.is_exit_function || $1.is_exit_function;
                                                        if ($2.scoped_name_p)
                                                        {
                                                           $$.scoped_name_p=$2.scoped_name_p;
                                                           $2.scoped_name_p=NULL;
                                                        }
                                                        else
                                                        {
                                                           $$.scoped_name_p=$1.scoped_name_p;
                                                           $1.scoped_name_p=NULL;
                                                        }
                                                        DUMP_SCOPE($$.scoped_name_p);
                                                      }
                                                ;

_definition_expr_no_parenthesis_no_double_quote_rest: _definition_item_no_parenthesis_no_double_quote_
                                                      {
                                                         $$.is_exit_function=$1.is_exit_function; 
                                                         $$.scoped_name_p=$1.scoped_name_p;
                                                         $1.scoped_name_p=NULL;
                                                         DUMP_SCOPE($$.scoped_name_p);
                                                      }
                                                    | _definition_expr_no_parenthesis_no_double_quote_rest _definition_item_no_parenthesis_no_double_quote_
                                                      { 
                                                        $$.is_exit_function=$2.is_exit_function || $1.is_exit_function;
                                                        if ($2.scoped_name_p)
                                                        {
                                                           $$.scoped_name_p=$2.scoped_name_p;
                                                           $2.scoped_name_p=NULL;
                                                        }
                                                        else
                                                        {
                                                           $$.scoped_name_p=$1.scoped_name_p;
                                                           $1.scoped_name_p=NULL;
                                                        }
                                                        DUMP_SCOPE($$.scoped_name_p);
                                                      }
                                                    ;

_definition_expr_no_parenthesis_: _definition_item_no_parenthesis_
                                |  _definition_expr_no_parenthesis_ _definition_item_no_parenthesis_ 
                                ;

_definition_item_no_parenthesis_: qualified_identifier_declation
                                | _PRIVATE_
                                | brackets_expr
                                | _AND_OP_
                                | _OR_OP_
                                | _CASE_
                                | _DEFAULT_
                                | _IF_
                                | _ELSE_
                                | _STRUCT_
                                | _EXTERN_
                                | _SWITCH_
                                | _WHILE_
                                | _DO_
                                | _FOR_
                                | _GOTO_
                                | _CONTINUE_
                                | _BREAK_
                                | _RETURN_
                                | _THROW_
                                | _SOMETHING_
                                | _STATIC_
                                | _SOME_BINARY_OPERATOR_
                                | _TRY_
                                | _USING_
                                | _UNSAFE_
                                | _CATCH_
                                | _ASM_
                                | _TOKEN_NOT_FOUND_
                                | _TOKEN_TO_IGNORE_
                                | ':'
                                | '!'
                                | ','
                                | inline_cmd_part
                                ;

_definition_item_no_parenthesis_no_double_quote__no_action_first: _UNSAFE_
                                                          | _AND_OP_
                                                          | _OR_OP_
                                                          | _CASE_
                                                          | _DEFAULT_
                                                          | _IF_
                                                          | _ELSE_
                                                          | _STRUCT_
                                                          | _EXTERN_
                                                          | _SWITCH_
                                                          | _WHILE_
                                                          | _DO_
                                                          | _FOR_
                                                          | _GOTO_
                                                          | _CONTINUE_
                                                          | _BREAK_
                                                          | _RETURN_
                                                          | _THROW_
                                                          | _SOMETHING_
                                                          | _STATIC_
                                                          | _SOME_BINARY_OPERATOR_
                                                          | _USING_
                                                          | _TRY_
                                                          | _CATCH_
                                                          | _ASM_
                                                          | _TOKEN_NOT_FOUND_
                                                          | _TOKEN_TO_IGNORE_
                                                          | '!'
                                                          | ','
                                                          | inline_cmd_part
                                                          ;

_definition_expr_no_parenthesis_no_double_quote_first : qualified_identifier_declation
                                                  { 
                                                    $$.is_exit_function=$1.is_exit_function; 
                                                    $$.scoped_name_p=$1.scoped_name_p;
                                                    $1.scoped_name_p=NULL;
                                                  }
                                                | _definition_item_no_parenthesis_no_double_quote__no_action_first
                                                  { $$.scoped_name_p=NULL; $$.is_exit_function=false; }
                                                ;

_definition_item_no_parenthesis_no_double_quote__no_action: _UNSAFE_
                                                          | brackets_expr
                                                          | _AND_OP_
                                                          | _PRIVATE_
                                                          | _OR_OP_
                                                          | _CASE_
                                                          | _DEFAULT_
                                                          | _IF_
                                                          | _ELSE_
                                                          | _STRUCT_
                                                          | _EXTERN_
                                                          | _SWITCH_
                                                          | _WHILE_
                                                          | _DO_
                                                          | _FOR_
                                                          | _GOTO_
                                                          | _CONTINUE_
                                                          | _BREAK_
                                                          | _RETURN_
                                                          | _THROW_
                                                          | _SOMETHING_
                                                          | _STATIC_
                                                          | _SOME_BINARY_OPERATOR_
                                                          | _USING_
                                                          | _TRY_
                                                          | _CATCH_
                                                          | _ASM_
                                                          | _TOKEN_NOT_FOUND_
                                                          | _TOKEN_TO_IGNORE_
                                                          | '!'
                                                          | ','
                                                          | inline_cmd_part
                                                          ;

_definition_item_no_parenthesis_no_double_quote_: qualified_identifier_declation
                                                  { 
                                                    $$.is_exit_function=$1.is_exit_function; 
                                                    $$.scoped_name_p=$1.scoped_name_p;
                                                    $1.scoped_name_p=NULL;
                                                  }
                                                | _definition_item_no_parenthesis_no_double_quote__no_action
                                                  { $$.scoped_name_p=NULL; $$.is_exit_function=false; }
                                                ;

definition_template_opt: /* empty */
                       | definition_template
                       ;

definition_template: definition_template_item
                   | definition_template definition_template_item
                   ;

definition_template_item: definition_parentesis
                          | '{' definition_decl_any_opt '}'
                          | '<' definition_template_opt '>'
                          | brackets_expr
                          | ';'
                          | _PRIVATE_
                          | _AND_OP_
                          | _OR_OP_
                          | '!'
                          | _CASE_
                          | _DEFAULT_
                          | _IF_
                          | _ELSE_
                          | _STRUCT_
                          | _EXTERN_
                          | _SWITCH_
                          | _WHILE_
                          | _DO_
                          | _FOR_
                          | _GOTO_
                          | _CONTINUE_
                          | _BREAK_
                          | _RETURN_
                          | _THROW_
                          | _SOMETHING_
                          | _STATIC_
                          | _SOME_BINARY_OPERATOR_
                          | _TRY_
                          | _USING_
                          | _IDENTIFIER_
                          | _SET_GET_
                          | _QUALIF_
                          | _CATCH_
                          | _ASM_
                          | _TOKEN_NOT_FOUND_
                          | _TOKEN_TO_IGNORE_
                          | ':'
                          | '='
                          | ','
                          | inline_cmd_part
                          ;

qualified_identifier_declation: _qualified_identifier_declaration
                      { 
                          $$.is_exit_function = $1.is_exit_function;
                          $$.scoped_name_p=$1.scoped_name_p;
                          $1.scoped_name_p=NULL;
                          DUMP_SCOPE($$.scoped_name_p);
                      }
                    | _QUALIF_ qualified_identifier_something_opt _qualified_identifier_declaration
                        {
                          $$.scoped_name_p=$3.scoped_name_p;
                          $3.scoped_name_p=NULL;
                          $$.is_exit_function = $3.is_exit_function;
                          DUMP_SCOPE($$.scoped_name_p);
                        }
                    ;

_qualified_identifier_declaration: identifier template_definitions_opt
                        { 
                          $$.scoped_name_p=new Scope;
                          $$.scoped_name_p->addName($1.text);
                          $$.is_exit_function = parser_compiler_p->isExitFunction($1.text);
                        }
                     | identifier template_definitions_opt  _QUALIF_ qualified_identifier_something_opt _qualified_identifier_declaration
                        {
                          $$.scoped_name_p=new Scope;
                          $$.scoped_name_p->addName($1.text);
                          if ($5.scoped_name_p)
                          {
                            $$.scoped_name_p->pushScope(*$5.scoped_name_p);
                            Scope *sc=new Scope;
                            sc->addName($$.scoped_name_p->fullName().c_str());
                            delete $$.scoped_name_p;
                            $$.scoped_name_p = sc ;
                          }
                          $$.is_exit_function = false;
                        }
                     ;

qualified_identifier_statement: identifier
                      { 
                          $$.scoped_name_p=new Scope;
                          $$.scoped_name_p->addName($1.text);
                          $$.is_exit_function = parser_compiler_p->isExitFunction($1.text);
                      }
                    | _QUALIF_ qualified_identifier_something_opt identifier
                        {
                          $$.scoped_name_p=new Scope;
                          $$.scoped_name_p->addName($3.text);
                          $$.is_exit_function = parser_compiler_p->isExitFunction($3.text);
                        }
                     ;

template_definitions_opt: /* empty */
                        | template_definitions
                        ;
template_definitions: '<' 
                        { in_template_definition=true; }
                        definition_template_opt '>'
                        { in_template_definition=false; }
                     | template_definitions '<' 
                        { in_template_definition=true; }
                       definition_template_opt '>'
                        { in_template_definition=false; }
                     ;

qualified_identifier_something_opt: qualified_identifier_something
                                 | /* empty */
                                 ;

qualified_identifier_something: qualified_identifier_something_item
                              | qualified_identifier_something qualified_identifier_something_item
                              ;

qualified_identifier_something_item: _SOMETHING_ 
                                   | _STATIC_
                                   ;

identifier : _IDENTIFIER_
             { $$.text=$1.text; }
           ;


/* functions */
compound_statement: '{' 
                    {
                      level++;
                    }
                     statements_opt '}'
                   { 
                      level--;
                      $$=$3;
                      $$.pos=@4; 
                   }
                  ;

statements_opt: /*empty */
                     {
                       $$.statement_completely_instrumented=false;
                       $$.nop_instruction=true;
                       $$.return_or_break_statement=false;
                       $$.non_sequential_entry_point=false;
                     }
                   | statements
                     { $$=$1; }
                   | statements labeled_statements
                     { 
                       $$.statement_completely_instrumented=false;
                       $$.nop_instruction=false;
                       $$.non_sequential_entry_point=true;
                       $$.return_or_break_statement=$1.return_or_break_statement;
                     }
                   | labeled_statements
                     { 
                       $$.statement_completely_instrumented=false;
                       $$.nop_instruction=false;
                       $$.non_sequential_entry_point=true;
                       $$.return_or_break_statement=false;
                     }
                   ;

labeled_statements: labeled_statement
                  | labeled_statement labeled_statements 
                  ;

statements: statement
            { $$=$1; }
          | statements statement
            { 
              $$=$2; 
              $$.nop_instruction=$1.nop_instruction && $2.nop_instruction;
              $$.non_sequential_entry_point=$1.non_sequential_entry_point || $2.non_sequential_entry_point;
              if ($2.non_sequential_entry_point)
              {
                $$.return_or_break_statement=$2.return_or_break_statement;
              }
              else
              {
                $$.return_or_break_statement=$2.return_or_break_statement || $1.return_or_break_statement;
                if ($1.return_or_break_statement)
                  skip_area( & @2);
              }
              if ($2.nop_instruction)
                $$.statement_completely_instrumented=$1.statement_completely_instrumented ;
              else if ($$.return_or_break_statement)
                $$.statement_completely_instrumented=true;
              else
                $$.statement_completely_instrumented=$2.statement_completely_instrumented;
            }
          ;

expression_or_function_statement: 
          function_statement
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | expression_statement
            { 
              $$.pos=@1;
              if ($1.is_exit_function)
              {
                record_expression_computation( & @1,level+1);  
                $$.statement_completely_instrumented=true; 
              }
              else
                $$.statement_completely_instrumented=false; 
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=false;
            }
          ;

statement: compound_statement
            { $$=$1; }
         | expression_or_function_statement
            { 
              $$=$1; 
              $$.nop_instruction=false;
            }
         | _STATIC_ function_statement
            { 
              $$.pos=@$;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | lock_statement
            {
              $$=$1;
              $$.nop_instruction=false;
            }
         | foreach_statement
            { 
              $$=$1;
              $$.nop_instruction=false;
              $$.return_or_break_statement=false;
            }
         | selection_statement
            {
              $$=$1; 
              $$.nop_instruction=false;
            }
         | iteration_statement
            { 
              $$=$1 ; 
              $$.nop_instruction=false;
              $$.statement_completely_instrumented=false; 
            }
          | /* empty */ ';'
            {
              $$.pos=@1;
              $$.statement_completely_instrumented=false;
              $$.nop_instruction=true;
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=false;
            }
         | jump_statement
            { 
              record_expression_computation( & @1,level+1);  
              $$.pos=@1;
              $$.statement_completely_instrumented=true;
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=$1.return_or_break_statement;
            }
         | return_statement
            {  
              record_expression_computation( & @1,level+1);  
              $$.pos=@1;
              $$.statement_completely_instrumented=true; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=$1.return_or_break_statement;
            }
         | unsafe_statement
            { 
              $$=$1; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | finally_statement
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | catch_statement
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | using_statement
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=false;
            }
         | asm_expr
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=false;
              $$.return_or_break_statement=false;
            }
         | try_statement
            { 
              $$.pos=@1;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
              $$.return_or_break_statement=false;
            }
         | labeled_statement statement
            { 
              $$=$2; 
              $$.nop_instruction=false;
              $$.non_sequential_entry_point=true;
            }
         | _STATIC_  expression_statement
            { 
              $$.pos=@2;
              $$.statement_completely_instrumented=false; 
              $$.nop_instruction=false;
              $$.return_or_break_statement=false;
              $$.non_sequential_entry_point=false;
              skip_area( & @2);
            }
         ;


asm_expr: _ASM_ asm_expr_list_opt  '{' asm_expr_block_list_opt '}'
        | _ASM_MS_LINE_
        | _ASM_ asm_expr_list_opt  ';'
        ;

asm_item: _ASM_ asm_expr_list_opt  '(' asm_expr_block_list_opt ')' 
        ;

asm_expr_block_list_opt: /*empty */
                       | asm_expr_block_list
                       ;

asm_expr_block_list: asm_expr_list_block_item
                   | asm_expr_list_block_item asm_expr_block_list 
                   ;

asm_expr_list_block_item: '(' asm_expr_block_list_opt ')'
                  | '(' compound_statement ')'
                  | asm_expr_list_block_item_single
                  | asm_expr_list_block_item_single '{' asm_expr_block_list_opt '}'
                  ;

asm_expr_list_block_item_single:                   _IDENTIFIER_
                   | _STRUCT_
                   | _SET_GET_
                   | _EXTERN_
                   | _PRIVATE_
                   | _AND_OP_
                   | _OR_OP_
                   | _THROW_
                   | _SOMETHING_
                   | _STATIC_
                   | ']'
                   | '['
                   | '<'
                   | '>'
                   | _SOME_BINARY_OPERATOR_
                   | _QUALIF_
                   | _ASM_
                   | _TOKEN_NOT_FOUND_
                   | _TOKEN_TO_IGNORE_
                   | ':'
                   | '='
                   | ','
                   | inline_cmd_part
                   | '!'
                   ;

asm_expr_list_opt: /* empty */
                 | asm_expr_list
                 ;

asm_expr_list: asm_expr_list_item
             | asm_expr_list_item asm_expr_list 
             ;

asm_expr_list_item: _IDENTIFIER_
                   | _STRUCT_
                   | _SET_GET_
                   | _EXTERN_
                   | _PRIVATE_
                   | _AND_OP_
                   | _OR_OP_
                   | _THROW_
                   | _SOMETHING_
                   | _STATIC_
                   | ']'
                   | '['
                   | '<'
                   | '>'
                   | _SOME_BINARY_OPERATOR_
                   | _QUALIF_
                   | _ASM_
                   | _TOKEN_NOT_FOUND_
                   | _TOKEN_TO_IGNORE_
                   | ':'
                   | '='
                   | ','
                   | inline_cmd_part
                   | '!'
                   ;


expression_statement: expression_statement_expr ';'
                     { $$=$1; }
                    ;

expression_statement_expr: expression_statement_no_equal_no_parentesis '=' expression_statement_assignment 
                            { 
                              $$.is_exit_function=false; 
                            }
                        | expression_statement_no_equal_no_parentesis  
                                { 
                                  $$.is_exit_function=false; 
                                }
                        | expression_statement_parentesis expression_statement_items_opt 
                                { 
                                  $$.is_exit_function=false; 
                                }
                        | function_statement_expr
                                { $$.is_exit_function = $1.is_exit_function; }
                        | expression_statement_no_equal_no_parentesis expression_statement_parentesis expression_statement_parentesis_rest_opt '=' expression_statement_assignment 
                            { 
                              $$.is_exit_function=false; 
                            }
                        ;

function_statement_expr: expression_statement_no_equal_no_parentesis  expression_statement_parentesis expression_statement_parentesis_rest_opt 
                                { 
                                  $$.is_exit_function=$1.is_exit_function; 
                                  $$.scoped_name_p = $1.scoped_name_p;
                                  $1.scoped_name_p = NULL;
                                   if ($$.scoped_name_p)
                                   {
                                     $$.proto.first_line=@1.first_line;
                                     $$.proto.first_column=@1.first_column;
                                     $$.proto.last_line=@2.last_line;
                                     $$.proto.last_column=@2.last_column;
                                   }
                                }
                        ;

expression_statement_assignment: logical_assignment_expr
                                  { 
                                    if (instrumentation_option(@1.first_line).instrument_assignment)
                                      record_expression_assignment( & @1,level+1,$1.expression_id);  
                                    else
                                      skip_area(&@1);
                                  }
                               | '{' expression_statement_items_opt '}' 
                               |  logical_assignment_expr ',' expression_statement_expr
                                  { 
                                    if (instrumentation_option(@1.first_line).instrument_assignment)
                                      record_expression_assignment( & @1,level+1,$1.expression_id);  
                                    else
                                      skip_area(&@1);
                                  }
                               | '{' expression_statement_items_opt '}' ',' expression_statement_expr
                               ;

function_statement: function_statement_expr compound_statement_record
                     {
                       $$.is_exit_function=false; 
                       if ($1.scoped_name_p)
                         record_function($1.scoped_name_p,&$1.proto,&@2);
                     }
                    ;


expression_statement_parentesis_rest_opt: /* empty */
                                        | expression_statement_parentesis_rest
                                        ;

expression_statement_parentesis_rest: expression_statement_parentesis_rest_item
                          | expression_statement_parentesis_rest expression_statement_parentesis_rest_item 
                          ;

expression_statement_parentesis_rest_item: _IDENTIFIER_
                                               | _STRUCT_
                                               | _SET_GET_
                                               | expression_statement_parentesis
                                               | _EXTERN_
                                               | _PRIVATE_
                                               | _AND_OP_
                                               | _OR_OP_
                                               | _THROW_
                                               | _SOMETHING_
                                               | _STATIC_
                                               | brackets_expr
                                               | '<'
                                               | '>'
                                               | _SOME_BINARY_OPERATOR_
                                               | _QUALIF_
                                               | _TOKEN_NOT_FOUND_
                                               | _TOKEN_TO_IGNORE_
                                               | ':'
                                               | ','
                                               | inline_cmd_part
                                               | '!'
                                               ;

expression_statement_items_opt: /* empty */
                              |expression_statement_items
                              ;

expression_statement_items: expression_statement_item
                          | expression_statement_items expression_statement_item
                          ;

expression_statement_items_block_opt : expression_statement_items_block
                                     | /* empty */
                                     ;

expression_statement_items_block: expression_statement_items_block expression_statement_items_block_item 
                                | expression_statement_items_block_item
                                ;

expression_statement_items_block_item: _IDENTIFIER_
                                     | _STRUCT_
                                     | _SET_GET_
                                     | _EXTERN_
                                     | _PRIVATE_
                                     | _AND_OP_
                                     | _OR_OP_
                                     | _THROW_
                                     | _UNSAFE_
                                     | _SOMETHING_
                                     | _STATIC_
                                     | brackets_expr
                                     | delegate_expr
                                     | asm_item
                                     | '<'
                                     | '>'
                                     | _SOME_BINARY_OPERATOR_
                                     | _QUALIF_
                                     | _TOKEN_NOT_FOUND_
                                     | _TOKEN_TO_IGNORE_
                                     | ':'
                                     | ';'
                                     | '='
                                     | ','
                                     | inline_cmd_part
                                     | '('
                                     | ')'
                                     | '!'
                                     |  '{' expression_statement_items_block_opt '}'
                                     ;
                   
expression_statement_no_equal_no_parentesis: expression_statement_no_equal_no_parentesis_first_item 
                                              {
                                                 $$.is_exit_function=$1.is_exit_function; 
                                                 $$.scoped_name_p=$1.scoped_name_p;
                                                 $1.scoped_name_p=NULL;
                                              }
                                           | expression_statement_no_equal_no_parentesis_first_item expression_statement_no_equal_no_parentesis_second_item 
                                              { 
                                                $$.is_exit_function=false;
                                                if ($2.scoped_name_p)
                                                {
                                                   $$.scoped_name_p=$2.scoped_name_p;
                                                   $2.scoped_name_p=NULL;
                                                }
                                                else
                                                {
                                                   $$.scoped_name_p=$1.scoped_name_p;
                                                   $1.scoped_name_p=NULL;
                                                }
                                              }
                                           | expression_statement_no_equal_no_parentesis_first_item expression_statement_no_equal_no_parentesis_second_item _expression_statement_no_equal_no_parentesis_
                                              { 
                                                $$.is_exit_function=false;
                                                if ($3.scoped_name_p)
                                                {
                                                   $$.scoped_name_p=$3.scoped_name_p;
                                                   $3.scoped_name_p=NULL;
                                                }
                                                else if ($2.scoped_name_p)
                                                {
                                                   $$.scoped_name_p=$2.scoped_name_p;
                                                   $2.scoped_name_p=NULL;
                                                }
                                                else
                                                {
                                                   $$.scoped_name_p=$1.scoped_name_p;
                                                   $1.scoped_name_p=NULL;
                                                }
                                              }
                                           ;

expression_statement_no_equal_no_parentesis_first_item_no_action: _STRUCT_
                                                       | _EXTERN_
                                                       | _PRIVATE_
                                                       | _AND_OP_
                                                       | _OR_OP_
                                                       | _SOMETHING_
                                                       | _SET_GET_
                                                       | brackets_expr
                                                       | asm_item
                                                       | delegate_expr
                                                       | '<'
                                                       | '>'
                                                       | _SOME_BINARY_OPERATOR_
                                                       | _TOKEN_NOT_FOUND_
                                                       | _TOKEN_TO_IGNORE_
                                                       | ','
                                                       | inline_cmd_part
                                                       | '!'
                                                        ;

expression_statement_no_equal_no_parentesis_first_item: expression_statement_no_equal_no_parentesis_first_item_no_action
                                                        { $$.scoped_name_p=NULL; $$.is_exit_function=false; }
                                                       | qualified_identifier_statement
                                                        { 
                                                          $$.is_exit_function=$1.is_exit_function; 
                                                          $$.scoped_name_p=$1.scoped_name_p;
                                                          $1.scoped_name_p=NULL;
                                                        }
                                                       ;

expression_statement_no_equal_no_parentesis_second_item_no_action: _STRUCT_
                                                                 | _EXTERN_
                                                                 | _PRIVATE_
                                                                 | _AND_OP_
                                                                 | _OR_OP_
                                                                 | _THROW_
                                                                 | _SET_GET_
                                                                 | _SOMETHING_
                                                                 | _STATIC_
                                                                 | brackets_expr
                                                                 | delegate_expr
                                                                 | asm_item
                                                                 | '<'
                                                                 | '>'
                                                                 | _SOME_BINARY_OPERATOR_
                                                                 | _TOKEN_NOT_FOUND_
                                                                 | _TOKEN_TO_IGNORE_
                                                                 | ','
                                                                 | inline_cmd_part
                                                                 | '!'
                                                                 |  '{' definitions_opt '}'
                                                                 ;

expression_statement_no_equal_no_parentesis_second_item: expression_statement_no_equal_no_parentesis_second_item_no_action
                                                        { $$.scoped_name_p=NULL; $$.is_exit_function=false; }
                                                       | qualified_identifier_statement
                                                        { 
                                                          $$.is_exit_function=$1.is_exit_function; 
                                                          $$.scoped_name_p=$1.scoped_name_p;
                                                          $1.scoped_name_p=NULL;
                                                        }
                                                       ;

_expression_statement_no_equal_no_parentesis_: _expression_statement_no_equal_no_parentesis_item_
                                                 { 
                                                    $$.is_exit_function=$1.is_exit_function;
                                                    $$.scoped_name_p=$1.scoped_name_p;
                                                    $1.scoped_name_p=NULL;
                                                 }
                                              | _expression_statement_no_equal_no_parentesis_ _expression_statement_no_equal_no_parentesis_item_ 
                                                 { 
                                                   $$.is_exit_function=$1.is_exit_function || $2.is_exit_function; 
                                                   if ($2.scoped_name_p)
                                                   {
                                                      $$.scoped_name_p=$2.scoped_name_p;
                                                      $2.scoped_name_p=NULL;
                                                   }
                                                   else
                                                   {
                                                      $$.scoped_name_p=$1.scoped_name_p;
                                                      $1.scoped_name_p=NULL;
                                                   }
                                                 }
                                              ;

_expression_statement_no_equal_no_parentesis_item_no_action_: _STRUCT_
                                                   | _EXTERN_
                                                   | _PRIVATE_
                                                   | _AND_OP_
                                                   | _SET_GET_
                                                   | _OR_OP_
                                                   | _THROW_
                                                   | _SOMETHING_
                                                   | _STATIC_
                                                   | brackets_expr
                                                   | delegate_expr
                                                   | asm_item
                                                   | '<'
                                                   | '>'
                                                   | _SOME_BINARY_OPERATOR_
                                                   | _TOKEN_NOT_FOUND_
                                                   | _TOKEN_TO_IGNORE_
                                                   | ','
                                                   | ':'
                                                   | inline_cmd_part
                                                   | '!'
                                                   |  '{' definitions_opt '}'
                                                   ;

_expression_statement_no_equal_no_parentesis_item_: _expression_statement_no_equal_no_parentesis_item_no_action_
                                                    { $$.scoped_name_p=NULL; $$.is_exit_function=false; }
                                                   | qualified_identifier_statement
                                                    { 
                                                      $$.is_exit_function=$1.is_exit_function; 
                                                      $$.scoped_name_p=$1.scoped_name_p;
                                                      $1.scoped_name_p=NULL;
                                                    }
                                                   ;

expression_statement_parentesis: '(' ')'
                               | '(' expression_statement_parentesis_first_item expression_statement_parentesis_items ')'
                               | '(' expression_statement_parentesis_first_item ')'
                               | '(' compound_statement ')'
                                    {
                                    }
                               ;

expression_statement_parentesis_items: expression_statement_parentesis_item
                                     | expression_statement_parentesis_items expression_statement_parentesis_item 
                                     ;

expression_statement_parentesis_first_item : expression_statement_parentesis
                                           | _IDENTIFIER_
                                           | _SET_GET_
                                           | _STRUCT_
                                           | _EXTERN_
                                           | _PRIVATE_
                                           | _UNSAFE_
                                           | _AND_OP_
                                           | _OR_OP_
                                           | _THROW_
                                           | _SOMETHING_
                                           | _STATIC_
                                           | brackets_expr
                                           | delegate_expr
                                           | asm_item
                                           | '<'
                                           | '>'
                                           | _SOME_BINARY_OPERATOR_
                                           | _QUALIF_
                                           | _TOKEN_NOT_FOUND_
                                           | _TOKEN_TO_IGNORE_
                                           | ':'
                                           | '='
                                           | ','
                                           | inline_cmd_part
                                           | '!'
                                           ;
                           
expression_statement_parentesis_item : expression_statement_parentesis
                                     | _IDENTIFIER_
                                     | _SET_GET_
                                     | _STRUCT_
                                     | _EXTERN_
                                     | _PRIVATE_
                                     | _AND_OP_
                                     | _OR_OP_
                                     | _THROW_
                                     | _SOMETHING_
                                     | _STATIC_
                                     | brackets_expr
                                     | delegate_expr
                                     | asm_item
                                     | '<'
                                     | '>'
                                     | _SOME_BINARY_OPERATOR_
                                     | _QUALIF_
                                     | _TOKEN_NOT_FOUND_
                                     | _TOKEN_TO_IGNORE_
                                     | _UNSAFE_
                                     | ':'
                                     | '='
                                     | ','
                                     | inline_cmd_part
                                     | '!'
                                     |  '{' expression_statement_items_block_opt '}'
                                     ;
                           
expression_statement_item: _IDENTIFIER_
                           | _STRUCT_
                           | '{' expression_statement_items_block_opt '}'
                           | _EXTERN_
                           | _PRIVATE_
                           | _SET_GET_
                           | _AND_OP_
                           | _OR_OP_
                           | _THROW_
                           | _UNSAFE_
                           | _SOMETHING_
                           | _STATIC_
                           | brackets_expr
                           | delegate_expr
                           | asm_item
                           | '<'
                           | '>'
                           | _SOME_BINARY_OPERATOR_
                           | _QUALIF_
                           | _TOKEN_NOT_FOUND_
                           | _TOKEN_TO_IGNORE_
                           | ':'
                           | '='
                           | ','
                           | inline_cmd_part
                           | expression_statement_parentesis
                           | '!'
                           ;

substatement:  { level++; }
                statement
                { 
                  level--;
                  $$=$2;
                }
                ;

statement_record: substatement
                { 
                  if (!$1.statement_completely_instrumented)
                    record_expression_computation( & $1.pos,level+1);  
                  $$=$1; 
                  $$.statement_completely_instrumented=true;
                }
                ;

compound_statement_record: compound_statement
                { 
                  if (!$1.statement_completely_instrumented)
                    record_expression_computation( & $1.pos,level+1);  
                  $$=$1; 
                  $$.statement_completely_instrumented=true;
                }
                ;

selection_statement : _IF_ '(' logical_expr ')' statement_record
                      { 
                        record_expression_if_then( & @3,& @5 ,& @$ ,level,$3.expression_id); 
                        $$.statement_completely_instrumented=false;
                        $$.non_sequential_entry_point=$5.non_sequential_entry_point;
                        $$.return_or_break_statement=false;
                      }
                    | _IF_ '(' logical_expr ')' statement_record _ELSE_ statement_record
                      { 
                        record_expression_if_then_else( & @3,& @5 ,& @7,level,$3.expression_id);
                        $$.statement_completely_instrumented=true;
                        $$.non_sequential_entry_point=$5.non_sequential_entry_point || $7.non_sequential_entry_point;
                        $$.return_or_break_statement=$5.return_or_break_statement && $7.return_or_break_statement;
                      }
                    | _SWITCH_ '(' logical_expr ')' substatement
                      { 
                        $$=$5; 
                        $$.statement_completely_instrumented=false;
                        $$.return_or_break_statement=false;
                      }
                    ;

expr: expr_first_item
    | expr_first_item _expr_
    ;

_expr_: expr_item
       | _expr_ expr_item
       ;


expr_first_item: _IDENTIFIER_
               | _STRUCT_
               | _SET_GET_
               | _EXTERN_
               | _PRIVATE_
               | _AND_OP_
               | _OR_OP_
               | _CASE_
               | _DEFAULT_
               | _IF_
               | _ELSE_
               | _SWITCH_
               | _WHILE_
               | _DO_
               | _FOR_
               | _GOTO_
               | _CONTINUE_
               | _BREAK_
               | _UNSAFE_
               | _THROW_
               | _SOMETHING_
               | _STATIC_
               | '<'
               | '>'
               | _SOME_BINARY_OPERATOR_
               | _QUALIF_
               | _TRY_
               | _USING_
               | _CATCH_
               | _TOKEN_NOT_FOUND_
               | _TOKEN_TO_IGNORE_
               | '(' ')'
               | '(' expr ')'
               | brackets_expr
               | delegate_expr
               | '(' compound_statement ')'
                  {
                  }
               | '!'
               | '='
               | ','
               | inline_cmd_part
               ;


expr_item: _IDENTIFIER_
         | _STRUCT_
         | _SET_GET_
         | _EXTERN_
         | _PRIVATE_
         | _AND_OP_
         | _OR_OP_
         | _CASE_
         | _DEFAULT_
         | _IF_
         | _ELSE_
         | _SWITCH_
         | _WHILE_
         | _DO_
         | _FOR_
         | _GOTO_
         | _CONTINUE_
         | _UNSAFE_
         | _BREAK_
         | _THROW_
         | _SOMETHING_
         | _STATIC_
         | brackets_expr
         | delegate_expr
         | '<'
         | '>'
         | _SOME_BINARY_OPERATOR_
         | _QUALIF_
         | _TRY_
         | _USING_
         | _CATCH_
         | _TOKEN_NOT_FOUND_
         | _TOKEN_TO_IGNORE_
         | '(' ')'
         | '(' expr ')'
         | '(' compound_statement ')'
            {
            }
         | '!'
         | '='
         | ','
         | inline_cmd_part
         |  '{' expression_statement_items_block_opt '}'
         ;

iteration_statement : _WHILE_ '(' logical_expr ')' statement_record
                      {
                        record_expression_while( & @3,&@1,&@5,level,$3.expression_id);  
                        $$.non_sequential_entry_point=$5.non_sequential_entry_point;
                        $$.return_or_break_statement=false;
                      }
                    | _DO_ statement_record _WHILE_ '(' logical_expr ')' ';'
                      {
                        record_expression_condition( & @5,level,$5.expression_id);  
                        $$.non_sequential_entry_point=$2.non_sequential_entry_point;
                        $$.return_or_break_statement=false;
                      }
                    | _FOR_ '(' expr_opt ';' ';' expr_opt ')' statement_record
                      {
                        $$.non_sequential_entry_point=$8.non_sequential_entry_point;
                        $$.return_or_break_statement=false;
                      }
                    | _FOR_ '(' expr_opt ';' logical_expr ';' expr_opt ')' statement_record
                      {
                        record_expression_for( & @5,&@5, &@9, level,$5.expression_id);
                        $$.non_sequential_entry_point=$9.non_sequential_entry_point;
                        $$.return_or_break_statement=false;
                      }
                    ;

expr_opt: /*empty */
        | expr
        ;

jump_statement : _GOTO_ expr ';'
                {
                  $$.return_or_break_statement=true;
                }
               | _CONTINUE_ ';'
                {
                  $$.return_or_break_statement=true;
                }
               | _BREAK_ ';'
                {
                  $$.return_or_break_statement=true;
                }
               | _YIELD_ _BREAK_ ';'
                {
                  $$.return_or_break_statement=false;
                }
               ;

return_statement: _RETURN_ ';'
                  {
                    $$.return_or_break_statement=true;
                  }
                | _RETURN_ expr ';'
                  {
                    $$.return_or_break_statement=true;
                  }
                | _YIELD_ _RETURN_ expr ';'
                  {
                    $$.return_or_break_statement=false;
                  }
                | _YIELD_ _RETURN_ ';'
                  {
                    $$.return_or_break_statement=false;
                  }
                | _THROW_ expr ';'
                  {
                    $$.return_or_break_statement=true;
                  }
                | _THROW_ ';'
                  {
                    $$.return_or_break_statement=true;
                  }
                ;

unsafe_statement : _UNSAFE_ statement 
                    { $$=$2; }
                  ; 

try_statement : _TRY_ statement_record
              ; 


using_statement : _USING_ '(' expr ')' statement_record
                ;

lock_statement : _LOCK_ '(' expr ')' statement_record
                      {
                        $$=$5;
                        $$.statement_completely_instrumented=false; 
                      }
                  ;

foreach_statement : _FOREACH_ '(' expr ')' statement_record
                      {
                        $$=$5;
                        $$.statement_completely_instrumented=false; 
                      }
                  ;

finally_statement :  _FINALLY_  statement_record
                      {
                        if (!instrumentation_option($2.pos.first_line).instrument_exception)
                        {
                          skip_area( & $2.pos);
                        }
                      }
                ; 
catch_statement :  _CATCH_ '(' expr ')' statement_record
                      {
                        if (!instrumentation_option($5.pos.first_line).instrument_exception)
                        {
                          skip_area( & $5.pos);
                        }
                      }
                |  _CATCH_  compound_statement_record
                      {
                        if (!instrumentation_option($2.pos.first_line).instrument_exception)
                        {
                          skip_area( & $2.pos);
                        }
                      }
                ; 

labeled_statement : _CASE_ expr_labeled_statement ':' 
                      {
                        //if (language_to_parse()!=CompilerInterface::EXT_SOURCE_CS)
                          //record_expression_case( & @$,level)
                      }
                  | _DEFAULT_ ':'
                      {
                        //if (language_to_parse()!=CompilerInterface::EXT_SOURCE_CS)
                          //record_expression_case( & @$,level)
                      }
                  | _IDENTIFIER_ ':' 
                      {
                          //record_expression_label( & @$,level)
                      }
                  ;

expr_labeled_statement: expr_item_labeled_statement
                      | expr_labeled_statement expr_item_labeled_statement 
                      ;


expr_item_labeled_statement: _IDENTIFIER_
                           | _STRUCT_
                           | _SET_GET_
                           | _EXTERN_
                           | _PRIVATE_
                           | _AND_OP_
                           | _OR_OP_
                           | _CASE_
                           | _DEFAULT_
                           | _IF_
                           | _ELSE_
                           | _SWITCH_
                           | _WHILE_
                           | _DO_
                           | _FOR_
                           | _GOTO_
                           | _CONTINUE_
                           | _BREAK_
                           | _SOMETHING_
                           | _STATIC_
                           | brackets_expr
                           | '<'
                           | '>'
                           | _SOME_BINARY_OPERATOR_
                           | _QUALIF_
                           | _TRY_
                           | _USING_
                           | _CATCH_
                           | _TOKEN_NOT_FOUND_
                           | _TOKEN_TO_IGNORE_
                           | '='
                           | ','
                           | inline_cmd_part
                           | '!'
                           | '(' expr_labeled_statement ')'
                           | '(' ')'
                           ;

logical_expr: logical_expr_list
              { $$=$1; }
            | logical_expr_list ','
              { $$=$1; }
            ;

logical_expr_list: logical_assignment_expr
                  { $$=$1; }
                 | logical_expr_list ',' logical_assignment_expr
                  { $$=$3; }
                 ;

logical_assignment_expr : logical_cmp_expr
                  { $$=$1; }
                | logical_assignment_expr '=' logical_cmp_expr
                  { $$=$3; }
                ;

logical_cmp_expr: logical_or_expr
                  { $$=$1; }
                | logical_or_expr '?' logical_assignment_expr_opt ':' logical_cmp_expr
                  {
                    $$=$1; 
                    Expression* expr_id = record_expression_sub_condition( & @1,level,$1.expression_id);
                    $$.expression_id=expr_id;
                  }
                ; 

logical_assignment_expr_opt : logical_assignment_expr
                    | /* empty */
                    ;

logical_or_expr : logical_and_expr
                  { $$=$1; }
                | logical_or_expr _OR_OP_ logical_and_expr
                  {
                    Expression* expr_id1 = record_expression_sub_condition( & @1,level,$1.expression_id);
                    Expression* expr_id2 = record_expression_sub_condition( & @3,level,$3.expression_id);
                    Expression* expr_id  = record_expression_or_expr( & @$,level,expr_id1,expr_id2);
                    $$.expression_id=expr_id;
                  }
                ;

logical_and_expr : logical_binary_operator_high_prio_expr
                  { $$=$1; }
                 | logical_and_expr _AND_OP_ logical_binary_operator_high_prio_expr
                  {
                    Expression* expr_id1 = record_expression_sub_condition( & @1,level,$1.expression_id);   
                    Expression* expr_id2 = record_expression_sub_condition( & @3,level,$3.expression_id);   
                    Expression* expr_id  = record_expression_and_expr( & @$,level,expr_id1,expr_id2);
                    $$.expression_id=expr_id;
                  }
                 ;

logical_binary_operator_high_prio_expr : logical_not_expr
                                { $$=$1; }
                              | logical_binary_operator_high_prio_expr _SOME_BINARY_OPERATOR_ logical_not_expr
                                {
                                  Expression* expr_id  = record_expression_some_binary_operator( & @$,level,$1.expression_id,$3.expression_id);
                                  $$.expression_id=expr_id;
                                }
                              ;

logical_not_expr: logical_expr_items
                  { $$=$1; }
                | '!' logical_not_expr
                  {
                    Expression* expr_id1 = $2.expression_id;
                    Expression* expr_id  = record_expression_not_expr( & @2,level,expr_id1);
                    $$.expression_id=expr_id;
                  }
                | logical_expr_items '!' logical_not_expr
                  {
                    Expression* expr_id1 = $3.expression_id;
                    Expression* expr_id  = record_expression_not_expr( & @3,level,expr_id1);
                    $$.expression_id=expr_id;
                  }
                ;

logical_expr_items: logical_expr_first _logical_expr_items_
                  { $$=$1; }
                  | logical_expr_first
                  { $$=$1; }
                  ;

logical_expr_first: '(' compound_statement ')'
                     {
                  { $$.expression_id=NULL;}
                     }
                 | '(' ')'
                  { $$.expression_id=NULL;}
                 | '(' logical_expr ')'
                  {
                    Expression* expr_id1 = $2.expression_id;
                    Expression* expr_id  = record_expression_parenthesis_expr( & @$,level,expr_id1);
                    $$.expression_id=expr_id;
                  }
                 | qualified_identifier_statement
                  { $$.expression_id=NULL;}
                 | _SET_GET_
                  { $$.expression_id=NULL;}
                 | '<'
                  { $$.expression_id=NULL;}
                 | '>'
                  { $$.expression_id=NULL;}
                 | _STRUCT_
                  { $$.expression_id=NULL;}
                 | _EXTERN_
                  { $$.expression_id=NULL;}
                 | delegate_expr
                  { $$.expression_id=NULL;}
                 | _SOMETHING_
                  { $$.expression_id=NULL;}
                 | _SOME_BINARY_OPERATOR_
                  { $$.expression_id=NULL;}
                 | brackets_expr
                  { $$.expression_id=NULL;}
                 | _UNSAFE_
                  { $$.expression_id=NULL;}
                 | _USING_
                  { $$.expression_id=NULL;}
                 | _TOKEN_NOT_FOUND_
                  { $$.expression_id=NULL;}
                 | _TOKEN_TO_IGNORE_
                  { $$.expression_id=NULL;}
                 | _AND_OP_ 
                  { $$.expression_id=NULL;}
                 ;

_logical_expr_items_: _logical_expr_items_ _logical_expr_item_ 
                   | _logical_expr_item_
                   ;

_logical_expr_item_: '(' expr ')' /* parentesis of a function call */
                   | '(' ')'
                   | '(' compound_statement ')'
                   |  '{' expression_statement_items_block_opt '}'
                   | '<'
                   | '>'
                   | _IDENTIFIER_
                   | _SET_GET_
                   | _STRUCT_
                   | _EXTERN_
                   | _PRIVATE_
                   | _CASE_
                   | _DEFAULT_
                   | _IF_
                   | _ELSE_
                   | _SWITCH_
                   | _WHILE_
                   | _DO_
                   | _FOR_
                   | _GOTO_
                   | _CONTINUE_
                   | _BREAK_
                   | _THROW_
                   | _SOMETHING_
                   | _STATIC_
                   | _UNSAFE_
                   | brackets_expr
                   | delegate_expr
                   | _QUALIF_
                   | _TRY_
                   | _USING_
                   | _CATCH_
                   | _TOKEN_NOT_FOUND_
                   | _TOKEN_TO_IGNORE_
                   ;

brackets_expr: '[' _brackets_expr_items_opt_ ']'
            ;

_brackets_expr_items_opt_: /* empty */
                         | _brackets_expr_items_
                         ;

_brackets_expr_items_ : _brackets_expr_item_
                      | _brackets_expr_items_ _brackets_expr_item_
                      ;


_brackets_expr_item_:  brackets_expr
                    | '(' 
                    | ')' 
                    | '{' 
                    | '}'
                    | '<' 
                    | '>'
                    | _PRIVATE_
                    | _UNSAFE_
                    | _AND_OP_
                    | _OR_OP_
                    | '!'
                    | _CASE_
                    | _DEFAULT_
                    | _IF_
                    | _ELSE_
                    | _STRUCT_
                    | _EXTERN_
                    | _SWITCH_
                    | _WHILE_
                    | _DO_
                    | _FOR_
                    | _GOTO_
                    | _CONTINUE_
                    | _BREAK_
                    | _RETURN_
                    | _THROW_
                    | _SOMETHING_
                    | _STATIC_
                    | _SOME_BINARY_OPERATOR_
                    | _TRY_
                    | _USING_
                    | _IDENTIFIER_
                    | _SET_GET_
                    | _QUALIF_
                    | _CATCH_
                    | _TOKEN_NOT_FOUND_
                    | _TOKEN_TO_IGNORE_
                    | _ASM_
                    | ':'
                    | '='
                    | ';'
                    | ','
                    | inline_cmd_part
                    ;

delegate_decl: _DELEGATE_ definition_expr_no_parenthesis definition_parentesis
             | _DELEGATE_ definition_parentesis 
             ;

delegate_expr: _DELEGATE_ definition_expr_no_parenthesis definition_parentesis compound_statement_record
               {
                  if ($2.scoped_name_p)
                  {
                    YYLTYPE proto;
                    proto.first_line=@1.first_line;
                    proto.first_column=@1.first_column;
                    proto.last_line=@3.last_line;
                    proto.last_column=@3.last_column;
                    record_function($2.scoped_name_p,&proto,&@4);
                  }
               }
             | _DELEGATE_ definition_expr_no_parenthesis compound_statement_record
               {
                  if ($2.scoped_name_p)
                  {
                    YYLTYPE proto;
                    proto.first_line=@1.first_line;
                    proto.first_column=@1.first_column;
                    proto.last_line=@2.last_line;
                    proto.last_column=@2.last_column;
                    record_function($2.scoped_name_p,&proto,&@3);
                  }
               }
             | _DELEGATE_ compound_statement_record
               {
               }
             | _DELEGATE_ definition_parentesis compound_statement_record
               {
               }
             ;

inline_cmd_part: '?' expr ':' 
               | '?' ':'
               ;
%%

#include <stdio.h>


static const char*error_format=NULL;
int c_cs_cpp_parsererror(const char *s)
{
  std::string m=  parser_error( error_format, filename, line, column, s);
  fprintf(stderr,"%s\n",m.c_str());
  return 0;
}

int yyparse();
long c_cs_cpp_analyse(const char*filename_orig,const char *finput,Expressions &e,Source &s,LinePos &l,Functions &funct,const Option  &opt,const CompilerInterface &comp,CompilerInterface::extension_t parse_language,const instrumentation_option_t &option, const Defines &defines,int (*yy_input_func)(char *,int),const char *err_format)
{
  int ret;
  FILE *f=NULL;

  preprocessor_init(defines,err_format);
  error_format=err_format;
  kr_function_def_scope_p=NULL;
  init_instrumentation_option(option);
  level=0;

  parser_compiler_p    = &comp;
  parser_option_p      = &opt;
  parser_source_p      = &s;
  parser_functions_p   = &funct;
  parser_expressions_p = &e;
  parser_line_pos_p    = &l;

  clearCurrentScope();
  init_c_cs_cpp_parserlex(parse_language,yy_input_func,filename_orig);
#if YYDEBUG
  yydebug=1;
#endif
  if (finput!=NULL)
  {
    if (yy_input_func==NULL)
    {
      f=fopen(finput,"r");
      if (f==NULL)
        FATAL2("Could not open file %s",finput);
    }
  }
  else
  {
    FATAL1("File name not defined");
  }
  test_input_file=f;


  ret=yyparse();
  if (f)
    fclose(f);
  return ret;
}

int c_cs_cpp_yyprint(FILE *f,int /*type*/,YYSTYPE value)
{
  yyprint_token(f,value.token.text);
  return 0;
}

