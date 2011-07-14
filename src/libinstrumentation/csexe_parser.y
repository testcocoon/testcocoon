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
%}
%skeleton "lalr1.cc"                          /*  -*- C++ -*- */
%require "2.4.1"
%defines
%define parser_class_name "CSExeParser"
// The parsing context.
%code requires {
#include <string>
class CSExeParserDriver;
class CSExeParser;
}

%parse-param { CSExeParserDriver& driver }
%lex-param   { CSExeParserDriver& driver }


%union
{
 char *        str;
 unsigned long ul_value;
 unsigned int  ui_value;
 long          l_value;
}
%locations
/*%verbose*/
%error-verbose
/*%debug*/
%expect 0
%pure-parser
%initial-action
{
 // Initialize the initial location.
 @$.begin.filename = @$.end.filename = &driver.file;
};


%token __END__ 0 "End of file"
%token __ULONG__
%token __LONG__
%token __STRING__
%token __UINT__
%token __SEPARATOR__
%token __CSEXE_MEASUREMENT__
%token __CSEXE_STATUS__
%token __STATUS_PASSED__
%token __STATUS_FAILED__
%token __STATUS_CHECK_MANUALLY__
%token __CSEXE_TITLE__
%token __CSEXE_INSTRUMENTATION_SOURCE__
%token __CSEXE_INSTRUMENTATION_VALUES__

%type <ui_value> __UINT__ instrumentation 
%type <ul_value> __ULONG__ signature
%type <l_value> __LONG__ nb_mes 

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

csexe_measurement : {
                      if (! driver.begin_csexe_measurement())
                      { // Interrupt
                        YYERROR;
                      }
                    }
                  csexe_measurement_
                    {
                      driver.end_csexe_measurement();
                    }
                  ;
                  
csexe_measurement_: csexe_start_banner csexe_instrumentations_opt csexe_status_opt
                  | csexe_title
                  ;

csexe_start_banner: __CSEXE_MEASUREMENT__ 
                  {
                    driver.begin_measurement();
                  }
                  ;

csexe_instrumentations_opt: /*empty */
                          | csexe_instrumentations
                          ;

csexe_instrumentations: csexe_instrumentation
                      | csexe_instrumentation csexe_instrumentations
                      ;

csexe_instrumentation: __CSEXE_INSTRUMENTATION_SOURCE__ nb_mes __SEPARATOR__ signature __SEPARATOR__ module_name 
                     {
                        long nb_mes=$2;
                        unsigned long signature=$4;
                        QString module=QString($6);
                        $6=NULL;
                        driver.init_add_instrumentation(@1.begin.line,module,nb_mes,signature);
                     }
                     module_instrumentation
                     {
                        driver.endup_add_instrumentation(@1.begin.line);
                     }
                     ;

nb_mes: __LONG__ 
      ;

signature: __ULONG__ 
      ;

module_name: str
		   { $$=$1; $1=NULL; }
           ;

module_instrumentation: __CSEXE_INSTRUMENTATION_VALUES__ instrumentations 
                      | __CSEXE_INSTRUMENTATION_VALUES__ 
                      ;

instrumentations: instrumentation 
                | instrumentation instrumentations
                ;
instrumentation: __UINT__
               {
                 driver.add_instrumentation(@1.begin.line,static_cast<Instrumentation::execution_state_t>($1));
               }
               ;
csexe_status_opt: /*empty */
                | csexe_status
                ;

csexe_status: csexe_one_status
            | csexe_one_status csexe_status
            ;

csexe_one_status : __CSEXE_STATUS__ __STATUS_PASSED__ 
                   { driver.set_status(Executions::EXECUTION_STATUS_PASSED); }
                  | __CSEXE_STATUS__ __STATUS_FAILED__ 
                   { driver.set_status(Executions::EXECUTION_STATUS_FAILED); }
                  | __CSEXE_STATUS__ __STATUS_CHECK_MANUALLY__ 
                   { driver.set_status(Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY); }
                  ;

csexe_title : __CSEXE_TITLE__ str 
            {
              driver.set_title(QString::fromUtf8($2).trimmed());
            }
            | __CSEXE_TITLE__ 
            {
              driver.set_title(QString());
            }
            ;

str:  __STRING__ 
   { $$=$1; }
   ;

%%

#include <stdio.h>


bool CSExeParser::csexe_parse(const QString &filename,QIODevice &file,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
{
  info.clear();
  _file_p=&file;

  if ( file.open( QIODevice::ReadOnly ) )
  {
    bool ret;
    init_csexe_parserlex();
    DEBUG2("Start parsing:#%s\n",text_line);
    CSExeParserDriver driver(_csmes,*this);
    ret = driver.parse(filename,new_executions,info,short_status,errmsgs,undo_backup_p,( (!file.isSequential()) && (progress_p!=NULL) )?progress_p:NULL);
    DEBUG3("End parsing(ret=%i):#%s\n",driver.result,text_line);
    file.close();
    return ret;
  }

  short_status=QObject::tr("Error opening I/O device");
  info=short_status;
  return false;
}


void yy::CSExeParser::error (const yy::CSExeParser::location_type& l, const std::string& m)
{
 driver.error (l, m);
}
