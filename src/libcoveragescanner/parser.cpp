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

#include "coveragescannerpdef.h"
#include "parser.h"
#include "parsemisc.h"
#include "expression_computation.h"
#include "expression_label.h"
#include "expression_case.h"
#include "expression_condition_false_only.h"
#include "expression_condition.h"
#include "expression_subcondition.h"
#include "expression_assignment.h"
#include "expression_parenthesis_expr.h"
#include "expression_not_expr.h"
#include "expression_some_binary_operator_expr.h"
#include "expression_or_expr.h"
#include "expression_and_expr.h"
#include "expression_main_function.h"
#include "expression_if_then_else.h"
#include "expression_if_then.h"
#include "expression_while.h"
#include "expression_for.h"
#include "function.h"
#include "expressions.h"
#include "source.h"
#include "parseroption.h"
#include <string.h>
#include <list>
#include <set>
#define FIRST_COLUMN_ID 1

std::set<std::string> _excluded_files;
std::set<std::string> _included_files;
const CompilerInterface *parser_compiler_p=NULL;
const Option *parser_option_p=NULL;
Source *parser_source_p=NULL;
Functions *parser_functions_p=NULL;
Expressions *parser_expressions_p=NULL;
LinePos *parser_line_pos_p=NULL;

static Scope current_scope;
static int (*input)()=NULL;
const char *filename;
long byte_pos;
long line_absolute;
long line;
long column;
long start_column = FIRST_COLUMN_ID ;
long start_line_absolute = 1;

void filenames_abs_pos(long line_abs,long &line_orig,long &line_rel,const char *&file)
{
  FUNCTION_TRACE;
  parser_line_pos_p->find(line_abs,line_orig,file,line_rel);
}

Expression * record_expression_assignment(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionAssignment *e=new ExpressionAssignment(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_label(const void * instruction,int level)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionLabel *e=new ExpressionLabel(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_case(const void * instruction,int level)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionCase *e=new ExpressionCase(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_computation(const void * instruction,int level)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionComputation *e=new ExpressionComputation(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_if_then_else(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const YYLTYPE *patch1_p=static_cast<const YYLTYPE *>(patch1);
  const YYLTYPE *patch2_p=static_cast<const YYLTYPE *>(patch2);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionIfThenElse *e=new ExpressionIfThenElse(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      patch1_p->first_line,
      patch1_p->first_column,
      patch1_p->last_line,
      patch1_p->last_column,
      patch2_p->first_line,
      patch2_p->first_column,
      patch2_p->last_line,
      patch2_p->last_column,
        expression_id1
        );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_condition_false_only(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionConditionFalseOnly *e=new ExpressionConditionFalseOnly(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1
      );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_parenthesis_expr(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionParenthesisExpr *e=new ExpressionParenthesisExpr(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_not_expr(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionNotExpr *e=new ExpressionNotExpr(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_some_binary_operator(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionSomeBinaryOperatorExpr *e=new ExpressionSomeBinaryOperatorExpr(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1,
      expression_id2
      );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_and_expr(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionAndExpr *e=new ExpressionAndExpr(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1,
      expression_id2
      );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_or_expr(const void * instruction,int level,Expression* expression_id1,Expression* expression_id2)
{
  FUNCTION_TRACE;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionOrExpr *e=new ExpressionOrExpr(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1,
      expression_id2
      );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_sub_condition(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionSubCondition *e=new ExpressionSubCondition(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1);

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_condition(const void * instruction,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionCondition *e=new ExpressionCondition(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      expression_id1);

  parser_expressions_p->append(e);
  return e;
}


Expression * record_expression_for(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const YYLTYPE *patch1_p=static_cast<const YYLTYPE *>(patch1);
  const YYLTYPE *patch2_p=static_cast<const YYLTYPE *>(patch2);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionFor *e=new ExpressionFor(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      patch1_p->first_line,
      patch1_p->first_column,
      patch1_p->last_line,
      patch1_p->last_column,
      patch2_p->first_line,
      patch2_p->first_column,
      patch2_p->last_line,
      patch2_p->last_column,
      expression_id1,
      *parser_source_p
        );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_while(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const YYLTYPE *patch1_p=static_cast<const YYLTYPE *>(patch1);
  const YYLTYPE *patch2_p=static_cast<const YYLTYPE *>(patch2);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionWhile *e=new ExpressionWhile(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      patch1_p->first_line,
      patch1_p->first_column,
      patch1_p->last_line,
      patch1_p->last_column,
      patch2_p->first_line,
      patch2_p->first_column,
      patch2_p->last_line,
      patch2_p->last_column,
      expression_id1
        );

  parser_expressions_p->append(e);
  return e;
}

Expression * record_expression_if_then(const void * instruction,const void *patch1,const void *patch2,int level,Expression* expression_id1)
{
  FUNCTION_TRACE;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *instruction_p=static_cast<const YYLTYPE *>(instruction);
  const YYLTYPE *patch1_p=static_cast<const YYLTYPE *>(patch1);
  const YYLTYPE *patch2_p=static_cast<const YYLTYPE *>(patch2);
  instrumentation_option_t options=instrumentation_option(instruction_p->first_line);

  filenames_abs_pos(instruction_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(instruction_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionIfThen *e=new ExpressionIfThen(
      file_name,
      options,
      instruction_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      instruction_p->first_column,
      instruction_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      instruction_p->last_column,
      level,
      patch1_p->first_line,
      patch1_p->first_column,
      patch1_p->last_line,
      patch1_p->last_column,
      patch2_p->first_line,
      patch2_p->first_column,
      patch2_p->last_line,
      patch2_p->last_column,
      expression_id1
        );

  parser_expressions_p->append(e);
  return e;
}

const char *filenames_append(const char*s,long line_orig,long line_abs)
{
  FUNCTION_TRACE;
  return parser_line_pos_p->append(s,line_orig,line_abs);
}

static inline void functions_append(Function &f)
{
  FUNCTION_TRACE;
  f.setSource(parser_source_p);
  parser_functions_p->append(f);
}

static inline const char* source_append(const char*s)
{
  FUNCTION_TRACE;
  const char *result ;
  result =  parser_source_p->append(s);

  ASSERT (strcmp(result,s)==0);
  return result;
}

static inline void source_append_char(char c)
{
  FUNCTION_TRACE;
  parser_source_p->append(c);
}


static ParserOption _instrumentation_options;
instrumentation_option_t instrumentation_option(int line)
{
  return _instrumentation_options.option(line);
}

void init_instrumentation_option(const instrumentation_option_t &o)
{
  _instrumentation_options.clear(o);
}

void push_instrumentation_option(int line,const instrumentation_option_t &o)
{
  _instrumentation_options.push(line,o);
}

void pop_instrumentation_option(int line)
{
  _instrumentation_options.pop(line);
}

static int YYSTYPE_size = 0;
#ifdef LOG
static void printScope(const Scope *scope)
{
  FUNCTION_TRACE;
  if (scope==NULL)
     return ;
  bool first=true;
  for (int i=0;i<scope->count();i++)
  {
    const char *name=(*scope)[i];
    if (name!=NULL)
    {
      if (!first)
      { 
        DEBUG1("::"); 
      }
      DEBUG1(name);
      first=false;
    }
  }
}
#endif

int expression_exists(void * p)
{
  FUNCTION_TRACE;
    YYLTYPE *position_p=static_cast<YYLTYPE *>(p);
    return parser_expressions_p->instrument_exists_abs(
                       position_p->first_line,
                       position_p->first_column,
                       position_p->last_line,
                       position_p->last_column);
}


void record_function(const Scope  *scope_name_p,const void *pos_definition,const void *pos_corpus)
{
  FUNCTION_TRACE;
  if (scope_name_p==NULL)
  {
     ASSERT(false);
     return ;
  }
  if (scope_name_p->count()==0)
  {
     ASSERT(false);
     return;
  }

   Function f;
   const char *corpus_file_name;
   long corpus_start_line_pre,corpus_end_line_pre;
   long corpus_start_line_orig,corpus_end_line_orig;
   const YYLTYPE *pos_corpus_p=static_cast<const YYLTYPE *>(pos_corpus);
   ASSERT(pos_definition);
   ASSERT(pos_corpus);

   filenames_abs_pos(pos_corpus_p->last_line,
         corpus_end_line_orig,
         corpus_end_line_pre,
         corpus_file_name);
   filenames_abs_pos(pos_corpus_p->first_line,
         corpus_start_line_orig,
         corpus_start_line_pre,
         corpus_file_name);

   DEBUG1("Function: Name:");
   f.clearScopedName();
   int nb_scope=0;
   if ((*scope_name_p)[0]!=NULL)
   {
      for (int i=0;i<currentScope().count();i++)
      {
         const char *name=currentScope()[i];
         if (name!=NULL)
         {
            if (nb_scope>0)
            {
               DEBUG1("::");
            }
            DEBUG1(name);
            f.pushScopedName(name);
            nb_scope++;
         }
      }
   }
   for (int i=0;i<scope_name_p->count();i++)
   {
      if ((*scope_name_p)[i]!=NULL)
      {
         if (nb_scope>0)
         {
            DEBUG1("::");
         }
         DEBUG1((*scope_name_p)[i]);
         f.pushScopedName((*scope_name_p)[i]);
         nb_scope++;
      }
   }
   ASSERT(nb_scope>0);

   if (pos_definition)
   {
      const YYLTYPE *pos_definition_p=static_cast<const YYLTYPE *>(pos_definition);
      long definition_start_line_pre,definition_end_line_pre;
      long definition_start_line_orig,definition_end_line_orig;
      const char *definition_file_name;
      filenames_abs_pos(pos_definition_p->last_line,
            definition_end_line_orig,
            definition_end_line_pre,
            definition_file_name);
      filenames_abs_pos(pos_definition_p->first_line,
            definition_start_line_orig,
            definition_start_line_pre,
            definition_file_name);

      f.setFunctionPrototype(pos_definition_p->first_line,pos_definition_p->last_line,
          definition_start_line_orig,-1,definition_end_line_orig,-1,
          definition_start_line_pre, pos_definition_p->first_column, definition_end_line_pre,pos_definition_p->last_column);
   }

   f.setFunctionCode(pos_corpus_p->first_line,pos_corpus_p->last_line,
       corpus_start_line_orig,-1,corpus_end_line_orig,-1,
       corpus_start_line_pre, pos_corpus_p->first_column, corpus_end_line_pre,pos_corpus_p->last_column);

   f.setFileName(corpus_file_name);

   functions_append(f);

}

void skip_area(const void * a)
{
  const YYLTYPE *area=static_cast<const YYLTYPE *>(a);
  parser_expressions_p->ignore_area( area->first_line,area->first_column,area->last_line,area->last_column);
}

Expression * record_main_function(const void * global_pos,const void *header,const void *body)
{
  FUNCTION_TRACE;
  int level=0;
  const char *file_name;
  long start_line_pre,end_line_pre;
  long start_line_orig,end_line_orig;
  const YYLTYPE *global_pos_p=static_cast<const YYLTYPE *>(global_pos);
  const YYLTYPE *header_p=static_cast<const YYLTYPE *>(header);
  const YYLTYPE *body_p=static_cast<const YYLTYPE *>(body);
  instrumentation_option_t options=instrumentation_option(global_pos_p->first_line);

  filenames_abs_pos(global_pos_p->last_line,
      end_line_orig,
      end_line_pre,
      file_name);
  filenames_abs_pos(global_pos_p->first_line,
      start_line_orig,
      start_line_pre,
      file_name);


  ExpressionMainFunction *e=new ExpressionMainFunction(
      file_name,
      options,
      global_pos_p->first_line,
      start_line_orig,
      -1,
      start_line_pre,
      global_pos_p->first_column,
      global_pos_p->last_line,
      end_line_orig,
      -1,
      end_line_pre,
      global_pos_p->last_column,
      level,
      header_p->first_line,
      header_p->first_column,
      header_p->last_line,
      header_p->last_column,
      body_p->first_line,
      body_p->first_column,
      body_p->last_line,
      body_p->last_column
        );

  parser_expressions_p->append(e);
  return e;
}

void init_parser_lex(int (*input_p)(),int size,const char *filename_orig)
{
  FUNCTION_TRACE;
  input=input_p;
  YYSTYPE_size = size;

  column = FIRST_COLUMN_ID ;

  line_absolute = 1;
  line = 1;

  filename = filenames_append(filename_orig,0,0); 
  byte_pos = 0;
  start_column = FIRST_COLUMN_ID ;
  start_line_absolute = 1;
}

/*!
 * \retval Pointer pointer into the source of the token
 */
static const char* adjust_position(const char *yytext, /*!< Test to be appended */
                             bool ignore_text /*!< Test is not added in the source code (for intern CoverageScanner pragma) */)
{
  FUNCTION_TRACE;
  int i;

  start_line_absolute=line_absolute;
  start_column=column;
  for (i = 0; yytext[i] != '\0'; i++)
  {
    byte_pos++;
    if (yytext[i] == '\n')
    {
      column = FIRST_COLUMN_ID ;
      line_absolute++;
      line++;
    }
    else
      column++;

  }

  if (ignore_text)
  { // fill the text with spaces
    char *text=STRDUP(yytext);
    for  (char *c=text;*c;c++)
    {
      if (*c!='\r' && *c!='\n')
        *c=' ';
    }
    const char *text_ret=source_append(text);
    FREE(text);
    return text_ret;
  }
  else
    return source_append(yytext);
}



static
void extract_position(YYLTYPE *yylloc_p)
{
  FUNCTION_TRACE;
  (*yylloc_p).first_line=start_line_absolute;
  (*yylloc_p).first_column=start_column;
  (*yylloc_p).last_line=line_absolute;
  (*yylloc_p).last_column=column;
}



void extract_name_preprocessor_line(const char *yytext,int yyleng,void * /*yylval_p*/,YYLTYPE * /*yylloc_p*/,bool &exclude_file)
{
  FUNCTION_TRACE;
  DEBUG2("extract_name_preprocessor_line:%s\n",yytext);
  int i1,i2,i0;
  int cLin;
  char name[CHAINE_LEN];
  for (i0=0;(i0<yyleng) && !(yytext[i0]>='0' && yytext[i0]<='9');i0++) ;
  for (i1=0;(i1<yyleng) && yytext[i1]!='"';i1++) ;
  for (i2=i1+1;(i2<yyleng) && yytext[i2]!='"';i2++) ;
  
  memcpy(name,&yytext[i0],i1-i0-1);
  name[i1-i0-1]=0;
  cLin=atoi(name);

  memcpy(name,&yytext[i1+1],i2-i1-1);
  name[i2-i1-1]=0;

  Source::removeEscape(name);

  std::string expression_match;
  exclude_file= parser_option_p->getFilterFile().isExclude(name,expression_match);
  if (exclude_file)
  {
    if (parser_option_p->isVerbose())
    {
      if (_excluded_files.find(std::string(name))==_excluded_files.end())
      {
        INFO3("File Excluded:%s (Expression:%s)\n",name,expression_match.c_str());
        _excluded_files.insert(std::string(name));
      }
    }
    DEBUG3("File Ignored:%s (Expression:%s)\n",name,expression_match.c_str());
  }
  else
  {
    if (parser_option_p->isVerbose())
    {
      if (_included_files.find(std::string(name))==_included_files.end())
      {
        INFO3("File Included:%s (Expression:%s)\n",name,expression_match.c_str());
        _included_files.insert(std::string(name));
      }
    }
    DEBUG3("File Included:%s (Expression:%s)\n",name,expression_match.c_str());
  }

  line=cLin-1;
  adjust_position (yytext,false);
  long line_orig=line;
  filename = filenames_append(name,line_orig,line_absolute);
}

void process_pragma_coveragescanner(bool ignore_text,const char *yytext,int yyleng,void * yylval_p,YYLTYPE * yylloc_p)
{
  FUNCTION_TRACE;

  pragma_t type=PRAGMA_UNHANDLED;
  if      (strstr(yytext,"cov-full-instrumentation"))    type = PRAGMA_COVERAGE_FULL_INSTRUMENTATION    ; 
  else if (strstr(yytext,"cov-partial-instrumentation")) type = PRAGMA_COVERAGE_PARTIAL_INSTRUMENTATION ; 
  else if (strstr(yytext,"cov-decision"))                type = PRAGMA_COVERAGE_DECISION                ; 
  else if (strstr(yytext,"cov-branch"))                  type = PRAGMA_COVERAGE_BRANCH                  ; 
  else if (strstr(yytext,"cov-condition"))               type = PRAGMA_COVERAGE_CONDITION               ; 
  else if (strstr(yytext,"cov-hit"))                     type = PRAGMA_COVERAGE_HIT                     ; 
  else if (strstr(yytext,"cov-count"))                   type = PRAGMA_COVERAGE_COUNT                   ; 
  else if (strstr(yytext,"cov-on"))                      type = PRAGMA_COVERAGE_ON                      ; 
  else if (strstr(yytext,"cov-off"))                     type = PRAGMA_COVERAGE_OFF                     ; 
  else if (strstr(yytext,"cov-assignment-off"))          type = PRAGMA_COVERAGE_ASSIGNMENT_OFF          ; 
  else if (strstr(yytext,"cov-assignment-on"))           type = PRAGMA_COVERAGE_ASSIGNMENT_ON           ; 
  else if (strstr(yytext,"pop"))                         type = PRAGMA_POP                              ; 

  process_pragma(type,ignore_text,yytext,yyleng,yylval_p,yylloc_p); 
}

void process_pragma(pragma_t type,bool ignore_text,const char *yytext,int /*yyleng*/,void * /*yylval_p*/,YYLTYPE * yylloc_p)
{
  FUNCTION_TRACE;
  int line=0;
  if (yylloc_p)
    line=yylloc_p->first_line;
  instrumentation_option_t option=instrumentation_option(line);
  switch (type)
  {
    case PRAGMA_POP:
    case PRAGMA_UNHANDLED:
      break;
    case PRAGMA_COVERAGE_ASSIGNMENT_ON:
      option.instrument_assignment=1;
      break;
    case PRAGMA_COVERAGE_ASSIGNMENT_OFF:
      option.instrument_assignment=0;
      break;
    case PRAGMA_COVERAGE_ON:
      option.coverage_enable=1;
      break;
    case PRAGMA_COVERAGE_OFF:
      option.coverage_enable=0;
      break;
    case PRAGMA_COVERAGE_HIT:
      option.coverage_hit=1;
      option.coverage_count=0;
      break;
    case PRAGMA_COVERAGE_COUNT:
      option.coverage_hit=0;
      option.coverage_count=1;
      break;
    case PRAGMA_COVERAGE_FULL_INSTRUMENTATION:
      option.partial_condition=0;
      break;
    case PRAGMA_COVERAGE_PARTIAL_INSTRUMENTATION:
      option.partial_condition=1;
      break;
    case PRAGMA_COVERAGE_DECISION:
      option.coverage_decision=1;
      option.coverage_branch=1;
      option.coverage_condition=0;
      break;
    case PRAGMA_COVERAGE_BRANCH:
      option.coverage_decision=0;
      option.coverage_branch=1;
      option.coverage_condition=0;
      break;
    case PRAGMA_COVERAGE_CONDITION:
      option.coverage_decision=1;
      option.coverage_branch=1;
      option.coverage_condition=1;
      break;
  }

  switch (type)
  {
    case PRAGMA_POP:
      pop_instrumentation_option(line);
      break;
    case PRAGMA_UNHANDLED:
      break;
    case PRAGMA_COVERAGE_ON:
    case PRAGMA_COVERAGE_OFF:
    case PRAGMA_COVERAGE_ASSIGNMENT_OFF:
    case PRAGMA_COVERAGE_ASSIGNMENT_ON:
    case PRAGMA_COVERAGE_HIT:
    case PRAGMA_COVERAGE_COUNT:
    case PRAGMA_COVERAGE_FULL_INSTRUMENTATION:
    case PRAGMA_COVERAGE_PARTIAL_INSTRUMENTATION:
    case PRAGMA_COVERAGE_DECISION:
    case PRAGMA_COVERAGE_BRANCH:
    case PRAGMA_COVERAGE_CONDITION:
      push_instrumentation_option(line,option);
      break;
  }
  adjust_position(yytext,ignore_text);
}


void skip_attribute(char *yytext,int yyleng,void * /*yylval_p*/,YYLTYPE * yylloc_p)
{
  FUNCTION_TRACE;
  int c;
  int level=0;
  bool started=false;
  std::string text=std::string(yytext);


  c = input();

  while (c != EOF)
  {
    DEBUG2("%c",c);
    text+=c;
    if (c=='(')
    {
      level++;
      started=true;
    }
    if (c==')')
      level--;
    if ((level==0) && (started))
    {
      break;
    }
    c = input();
  }

  if (strstr(text.c_str(),"CoverageScanner")) 
    process_pragma_coveragescanner(true,text.c_str(),0,NULL,yylloc_p);
  else
    adjust_position(text.c_str(),false);
}

  
void process_return(const char *yytext,int /*yyleng*/,void * /*yylval_p*/,YYLTYPE * /*yylloc_p*/)
{
  FUNCTION_TRACE;
  column = FIRST_COLUMN_ID ;
  line_absolute++;
  line++;
  source_append(yytext);
}

void process_spaces(const char *yytext,int yyleng,void * /*yylval_p*/,YYLTYPE * /*yylloc_p*/)
{
  FUNCTION_TRACE;
  column += yyleng ;
  source_append(yytext);
}

/*!
 * \retval Pointer pointer into the source of the token
 */
const char* process_token(const char *yytext,int /*yyleng*/,void *yylval_p,YYLTYPE *yylloc_p)
{
  FUNCTION_TRACE;
  const char *yytext_str;
  yytext_str=adjust_position(yytext,false);
  ASSERT(yytext_str);

 
  extract_position(yylloc_p);
  //memset(yylval_p,0,YYSTYPE_size);
  ((token_t*)yylval_p)->text = yytext_str;

  return yytext_str;
}

void dummy_fprintf(FILE * /*f*/,...)
{
}

void popCurrentScope()
{
  FUNCTION_TRACE;
  current_scope.popScope();
}

void pushCurrentScope(const Scope *p)
{
  FUNCTION_TRACE;
#ifdef LOG
  DEBUG1("pushCurrentScope:");
  printScope(p);
  DEBUG1("\n");
#endif
  current_scope.pushScope(*p);
}

bool is_function_name_no_scope(const Scope *scope_p,const char *str)
{
  if (scope_p)
  {
    int c=scope_p->count();
    if (c>0)
      return strcmp((*scope_p)[c-1],str)==0;
  }
  return false;
}

void clearCurrentScope()
{
  FUNCTION_TRACE;
  current_scope.clear();
}

const Scope &currentScope()
{
  FUNCTION_TRACE;
  return current_scope;
}

void initExcludeIncludeWarning()
{
  _excluded_files.clear();
  _included_files.clear();
}

/// Display parser error message using the correct error format
std::string parser_error( 
    const char *error_format, ///< Error format
    const char *filename,     ///< Source filename
    long line,                ///< Source line
    long column,              ///< Source column
    const  char *msg          ///< Error message
    )
{
#ifdef LOG
  DEBUG1("!!! ERRORS !!!\n");
#endif
  std::string formated_message;
  if (error_format)
  {
    char *outstr=NULL;
    const char *f="";
    const char *found=NULL;
    if (filename)
      f=filename;
    outstr=STRDUP(error_format);
    while ( (found=strstr(outstr,"%f")) )
    {
      char *new_outstr;
      int pos=found-outstr;
      new_outstr=(char*)MALLOC(sizeof(char)*(strlen(outstr)+strlen(f)+1));
      strncpy(new_outstr,outstr,pos);
      new_outstr[pos]='\0';
      strcat(new_outstr,f);
      strcat(new_outstr,&outstr[pos+2]);
      FREE(outstr);
      outstr=new_outstr;
    }
    while ( (found=strstr(outstr,"%l")) )
    {
      char *new_outstr;
      char l[CHAINE_LEN];
      int pos=found-outstr;
      sprintf(l,"%li",line);
      new_outstr=(char*)MALLOC(sizeof(char)*(strlen(outstr)+strlen(l)+1));
      strncpy(new_outstr,outstr,pos);
      new_outstr[pos]='\0';
      strcat(new_outstr,l);
      strcat(new_outstr,&outstr[pos+2]);
      FREE(outstr);
      outstr=new_outstr;
    }
    while ( (found=strstr(outstr,"%c")) )
    {
      char *new_outstr;
      char c[CHAINE_LEN];
      int pos=found-outstr;
      sprintf(c,"%li",column);
      new_outstr=(char*)MALLOC(sizeof(char)*(strlen(outstr)+strlen(c)+1));
      strncpy(new_outstr,outstr,pos);
      new_outstr[pos]='\0';
      strcat(new_outstr,c);
      strcat(new_outstr,&outstr[pos+2]);
      FREE(outstr);
      outstr=new_outstr;
    }
    while ( (found=strstr(outstr,"%m")) )
    {
      char *new_outstr;
      int pos=found-outstr;
      new_outstr=(char*)MALLOC(sizeof(char)*(strlen(outstr)+strlen(msg)+1));
      strncpy(new_outstr,outstr,pos);
      new_outstr[pos]='\0';
      strcat(new_outstr,msg);
      strcat(new_outstr,&outstr[pos+2]);
      FREE(outstr);
      outstr=new_outstr;
    }
    formated_message=std::string(outstr);
    FREE(outstr);
  }
#ifdef LOG
  if (filename!=NULL) 
  {
    ERROR5("%s: Line %li, Column %li:%s\n", filename, line,column, msg);
  }
  else
  {
    ERROR4("Line %li, Column %li:%s\n", line,column, msg);
  }
  if (parser_source_p!=NULL)
    parser_source_p->PDEBUG("source code");
  DEBUG1("!!! ERRORS !!!\n");
#endif
  return formated_message;
}

