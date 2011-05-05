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

#ifndef CPARSE_INCLUDE_H
#define CPARSE_INCLUDE_H
#include <stdio.h>
#include <string.h>
#if  defined(OS_LINUX) || defined(OS_MACX)
#include <unistd.h>
#endif
#if  defined(OS_WIN32)
#include <io.h>
#endif
#include "coveragescannerpdef.h"
#include "expression.h"
#include "functions.h"
#include "source.h"
#include "compilerinterface.h"
#include "option.h"
#include "linepos.h"
#include "filename.h"
#include "scope.h"
#include "defines.h"

#define CLEAR_FIELDS(X) { memset(&X,0,sizeof(X)); }

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)

typedef struct YYLTYPE
{
  long first_line;
  long first_column;
  long last_line;
  long last_column;
}
YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

struct token_t
{
  const char *text;
} ;

struct logical_expr_t
{
  Expression* expression_id;
} ;

struct compound_t
{
  bool statement_completely_instrumented;
  bool return_or_break_statement;
  bool non_sequential_entry_point;
  bool nop_instruction;
  YYLTYPE pos;
} ;

struct function_statement_expr_t
{
  bool is_exit_function;
  YYLTYPE proto;
  Scope *scoped_name_p;
} ;

struct exit_function_t
{
  bool is_exit_function;
} ;

struct qualified_identifier_t
{
  bool is_exit_function;
  Scope *scoped_name_p;
} ;

struct qualified_identifier_def_t
{
  bool double_quote_found;
  bool is_exit_function;
  Scope *scoped_name_p;
} ;

struct definition_decl_t
{
  YYLTYPE pos;
  Scope *scoped_name_p;
} ;

struct qualified_identifier_func_t
{
  bool is_exit_function;
  Scope *scoped_name_p;
} ;

struct preprocessor_t
{
  bool value;
} ;


int expression_exists(void * p);
extern const char *filename;
extern long byte_pos;
extern long line_absolute;
extern long line;
extern long column;
extern long start_column ;
extern long start_line_absolute ;
void init_parser_lex(int (*input_p)(),int size,const char *filename_orig);
int check_identifier(const char *);
const char* process_token(const char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p);
void extract_name_preprocessor_line(const char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p,bool &exclude_file);
void process_spaces(const char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p);
void process_return(const char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p);

/*! Pragma handled by TestCocoon */
typedef enum { 
  PRAGMA_UNHANDLED,                        /*!< Pragma unhandled */
  PRAGMA_COVERAGE_BRANCH,                  /*!< coverage method:branch */
  PRAGMA_COVERAGE_CONDITION,               /*!< coverage method: condition */
  PRAGMA_COVERAGE_DECISION,                /*!< coverage method: decision */
  PRAGMA_COVERAGE_FULL_INSTRUMENTATION,    /*!< full instrumentation of conditions */
  PRAGMA_COVERAGE_PARTIAL_INSTRUMENTATION, /*!< partial instrumentation of conditions */
  PRAGMA_COVERAGE_ON,                      /*!< Enable code coverage */
  PRAGMA_COVERAGE_OFF,                     /*!< Disable code coverage */
  PRAGMA_COVERAGE_ASSIGNMENT_OFF,          /*!< Disable code coverage in assignment */
  PRAGMA_COVERAGE_ASSIGNMENT_ON,           /*!< Enable code coverage in assignment */
  PRAGMA_COVERAGE_HIT,                     /*!< Enable code coverage Hit */
  PRAGMA_COVERAGE_COUNT,                   /*!< Enable code coverage count */
  PRAGMA_POP,                              /*!< Restore state */
} pragma_t ;
void process_pragma(pragma_t type,bool ignore_text,const char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p);
void process_pragma_coveragescanner(bool ignore_text,const char *yytext,int yyleng,void * yylval_p,YYLTYPE * yylloc_p);
void skip_attribute(char *yytext,int yyleng,void *yylval_p,YYLTYPE *yylloc_p);


void record_function(const Scope *name,const void *pos_definition,const void *pos_corpus);
bool is_function_name_no_scope(const Scope *name,const char *str);
Expression * record_main_function(const void * global_pos,const void *header,const void *body);
Expression * record_expression_computation(const void * instruction,int level);
Expression * record_expression_case(const void * instruction,int level);
Expression * record_expression_label(const void * instruction,int level);
Expression * record_expression_assignment(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_if_then(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1);
Expression * record_expression_while(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1);
Expression * record_expression_for(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1);
Expression * record_expression_condition(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_or_expr(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2);
Expression * record_expression_not_expr(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_parenthesis_expr(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_and_expr(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2);
Expression * record_expression_some_binary_operator(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2);
Expression * record_expression_sub_condition(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_condition_false_only(const void * instruction,int level,Expression* expression_id1);
Expression * record_expression_if_then_else(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1);
void skip_area(const void * area);
void yyprint_token(FILE *f,const char  *); /*!< used to print a token from the bison output */

void pushCurrentScope(const Scope *);
void popCurrentScope();
void clearCurrentScope();
const Scope &currentScope();
instrumentation_option_t instrumentation_option(int line);
void init_instrumentation_option(const instrumentation_option_t &);
void push_instrumentation_option(int line,const instrumentation_option_t &);
void pop_instrumentation_option(int line);
long c_cs_cpp_analyse(const char*filename_orig,const char *finput,Expressions &e,Source &s,LinePos &l,Functions &funct,const Option  &opt,const CompilerInterface &comp,CompilerInterface::extension_t parse_language,const instrumentation_option_t &option, const Defines &defines,int (*yy_input_func)(char *,int),const char *err_format);
int preprocessor_parse(const char *text_line);
int preprocessor_init(const Defines &defines,const char *err_format);
bool preprocessor_ignore_area();

std::string parser_error( const char *error_format, const char *filename,   long line,             long column,          const  char *msg     );
/* debug function for bison */
#if LOG
#define  YYFPRINTF DEBUG_FPRINTF
#else
void dummy_fprintf(FILE *f, ...);
#define  YYFPRINTF dummy_fprintf
#endif

#if !defined(LOG)
#define YYDEBUG 0
#endif
#define YY_LOCATION_PRINT(File, Loc)	

extern const CompilerInterface *parser_compiler_p;
extern const Option *parser_option_p;
extern Source *parser_source_p;
extern Functions *parser_functions_p;
extern Expressions *parser_expressions_p;
extern LinePos *parser_line_pos_p;

#endif


