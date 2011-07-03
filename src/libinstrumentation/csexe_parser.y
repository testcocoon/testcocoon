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
#include "executionname.h"
#include <QString>
#define yyerror csexe_parsererror
#if OS_WIN32
#pragma warning(disable : 4065)
#endif
extern int csexe_parserlex (YYSTYPE * yylval_param,YYLTYPE * yylloc_param );
extern ExecutionName _csexe_parser_execution_title;
%}
%name-prefix="csexe_parser"
%union
{
 char *        str;
 unsigned long value;
}
%locations
%verbose
%error-verbose
/*%debug*/
%pure-parser
%parse-param {QString *randomness}

%token __NUMBER__
%token __STRING__
%token __CSEXE_MEASUREMENT__
%token __CSEXE_STATUS__
%token __STATUS_PASSED__
%token __STATUS_FAILED__
%token __STATUS_CHECK_MANUALLY__
%token __CSEXE_TITLE__
%token __CSEXE_INSTRUMENTATION_SOURCE__
%token __CSEXE_INSTRUMENTATION_VALUES__

%type <value> __NUMBER__ instrumentation

%type <str>   __STRING__ str module_name
%destructor { if ($$) FREE($$); } str module_name

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

csexe_start_banner: __CSEXE_MEASUREMENT__ '\n' csexe_instrumentations
                  | __CSEXE_MEASUREMENT__ '\n' csexe_instrumentations csexe_status
                  | __CSEXE_MEASUREMENT__ '\n' csexe_status
                  | __CSEXE_MEASUREMENT__ '\n' 
                  ;

csexe_instrumentations: csexe_instrumentation
                      | csexe_instrumentation csexe_instrumentations
                      ;

csexe_instrumentation: __CSEXE_INSTRUMENTATION_SOURCE__ nb_mes ':' signature ':' module_name '\n' module_instrumentation
                     ;

nb_mes: __NUMBER__ 
      ;

signature: __NUMBER__ 
      ;

module_name: str
		   { $$=$1; $1=NULL; }
           ;

module_instrumentation: __CSEXE_INSTRUMENTATION_VALUES__ instrumentations '\n'
                      | __CSEXE_INSTRUMENTATION_VALUES__ '\n'
                      ;

instrumentations: instrumentation 
                | instrumentation instrumentations
                ;
instrumentation: __NUMBER__
               ;

csexe_status: csexe_one_status
            | csexe_one_status csexe_status
            ;

csexe_one_status : __CSEXE_STATUS__ __STATUS_PASSED__ '\n'
                  | __CSEXE_STATUS__ __STATUS_FAILED__ '\n'
                  | __CSEXE_STATUS__ __STATUS_CHECK_MANUALLY__ '\n'
                  ;

csexe_titles: csexe_title
            | csexe_title csexe_titles
            ;

csexe_title : __CSEXE_TITLE__ str '\n'
            {
              QString title = QString::fromUtf8($2).trimmed();
              if (!title.isEmpty())
                _csexe_parser_execution_title = title;
            }
            | __CSEXE_TITLE__ '\n'
            ;

str:  __STRING__ 
   { $$=$1; }
   ;

%%

#include <stdio.h>

int csexe_parsererror(YYLTYPE *yylloc, QString *errormsg, const  char *s)
{
  *errormsg = "Line "+QString::number(yylloc->first_line);
  *errormsg += ", Column "+QString::number(yylloc->first_column);
  *errormsg +=":" + QString::fromAscii(s);
  fprintf(stderr,"Error:%s\n",errormsg->toAscii().data());
  return 0;
}

int yyparse(int *randomness);

long csexe_parse(CSMesIO &csmes,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
{
  info.clear();
  short_status.clear();
  if ( file.open( QIODevice::ReadOnly ) )
  {
    int ret;
    _csexe_parser_execution_title=name_orig;
    init_csexe_parserlex(csmes,file,name_orig,policy,default_execution_status,new_executions,info,short_status,errmsgs,undo_backup_p,progress_p);
    DEBUG2("Start parsing:#%s\n",text_line);
    QString errormsg;
    ret=yyparse(&errormsg);
    DEBUG3("End parsing(ret=%i):#%s\n",ret,text_line);
    file.close();
    return ret;
  }

  short_status=QObject::tr("Error opening I/O device");
  info=short_status;
  return -1;
}

int csexe_yyprint(FILE *f,int /*type*/,YYSTYPE value)
{
  //yyprint_token(f,value.token.text);
  return 0;
}

ExecutionName _csexe_parser_execution_title;
