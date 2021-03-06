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
class CSExeParserDriver;
class CSExeParser;
#if OS_WIN32
#undef max
#endif
}

%parse-param { CSExeParserDriver& driver }
%lex-param   { CSExeParserDriver& driver }


%union
{
 char *        str;
 unsigned long ul_value;
 long          l_value;
}
/*%verbose*/
%error-verbose
/*%debug*/
%expect 0
%pure-parser
%initial-action
{
 // Initialize the initial location.
};


%token __END__ 0 "EOF"
%token __ULONG__ "unsigned value"
%token __UINT__
%token __LONG__ "value"
%token __STRING__ "string"
%token __SEPARATOR__ ":"
%token __CSEXE_MEASUREMENT__ "execution report"
%token __CSEXE_STATUS__ "execution status (! character)"
%token __STATUS_PASSED__ "status PASSED"
%token __STATUS_FAILED__ "status FAILED"
%token __STATUS_CHECK_MANUALLY__ "status CHECKED_MANUALLY"
%token __STATUS_UNKNOWN__ "status UNKNOWN"
%token __CSEXE_TITLE__ "title"
%token __CSEXE_INSTRUMENTATION_SOURCE__ "execution report source file"
%token __CSEXE_INSTRUMENTATION_VALUES__ "execution report values"
%token __EOL__ "end of line"

%type <ul_value> __ULONG__ signature
%type <l_value> __LONG__ nb_mes 

%type <str>   __STRING__ str module_name
%destructor { if ($$) FREE($$); } str module_name

%start csexe_parser 


%%

csexe_parser : csexe_status_opt csexe_measurements  
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
                  
csexe_measurement_: csexe_titles_opt csexe_start_banner csexe_instrumentations_opt csexe_status_opt
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
                        driver.init_add_instrumentation(module,nb_mes,signature);
                     }
                     module_instrumentation
                     {
                        driver.endup_add_instrumentation();
                     }
                     ;

nb_mes: __LONG__ 
      ;

signature: __ULONG__ 
      ;

module_name: str
		   { $$=$1; $1=NULL; }
           ;

module_instrumentation: __CSEXE_INSTRUMENTATION_VALUES__ __EOL__
                      | __CSEXE_INSTRUMENTATION_VALUES__ __END__
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
                  | __CSEXE_STATUS__ __STATUS_UNKNOWN__ 
                   { driver.set_status(Executions::EXECUTION_STATUS_UNKNOWN); }
                  ;
csexe_titles_opt : /* empty */
                 | csexe_titles
                 ;

csexe_titles: csexe_title
            | csexe_title csexe_titles
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
#include <QString>


bool CSExeParser::parse(const QString &filename,QIODevice &file,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
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
 driver.error (QString::fromStdString(m));
}
