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
static CSMesIO *_csmes_p=NULL;
static ExecutionName _csexe_parser_execution_title;
static ExecutionName _csexe_parser_execution_title_file;
static Executions::execution_status_t _csexe_parser_execution_status;
static ExecutionName _csexe_parser_execution_title_default;
static Executions::execution_status_t _csexe_parser_execution_status_default;
static CSMesIO::csexe_import_policy_t _csexe_parser_policy;
static Executions::modules_executions_t mts;
static QStringList _errors;
static bool skip_module;
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
                    _csexe_parser_execution_title_file.clear();
                    _csexe_parser_execution_status= Executions::EXECUTION_STATUS_UNKNOWN;
                    //CSExeParser::createEmptyExecution(mts);
                    }
                  csexe_measurement_
                  ;
                  
csexe_measurement_: csexe_start_banner csexe_instrumentations_opt csexe_status_opt
                  | csexe_title
                  ;

csexe_start_banner: __CSEXE_MEASUREMENT__ 
                  {
                    //_csexe_parser_execution_title=CSExeParser::executionName(_csexe_parser_execution_title_default,_csexe_parser_execution_title_file,_csexe_parser_policy);
                    
                  }
                  ;

csexe_instrumentations_opt: /*empty */
                          | csexe_instrumentations
                          ;

csexe_instrumentations: csexe_instrumentation
                      | csexe_instrumentation csexe_instrumentations
                      ;

csexe_instrumentation: __CSEXE_INSTRUMENTATION_SOURCE__ nb_mes __SEPARATOR__ signature __SEPARATOR__ module_name  module_instrumentation
                     {
                        skip_module=false;
                        long nb_mes=$2;
                        unsigned long signature=$4;
                        QString module=QString($6);
                        $6=NULL;

#if 0
                        if (!CSExeParser::moduleExists(module))
                        {
                          skip_module=true;
                        }
                        else
                        {
                          if (instrumentations.modules[module].signature!=signature)
                            wrong_executions=true;
                          else if (nb_mes!=nbInstrumentations(module))
                            wrong_executions=true;
                        }

                        if (!skip_module)
                        {
                          mts.execution_status=execution_status;
                          mes_p=&(mts.executions[module]);
                          int mes_p_size=mes_p->size();
                          if (mes_p_size!=nb_mes)
                          {
                            err=QObject::tr("Invalid file format")
                              +" (" +QObject::tr("Line ")+QString::number(line_nr) +":"+QObject::tr("Wrong instrumentation size")+")";
                            import_error=true;
                          }
                        }
#endif
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
               ;
csexe_status_opt: /*empty */
                | csexe_status
                ;

csexe_status: csexe_one_status
            | csexe_one_status csexe_status
            ;

csexe_one_status : csexe_one_status_
                 {
                    switch (mts.execution_status)
                    {
                      case Executions::EXECUTION_STATUS_UNKNOWN:
                          mts.execution_status=_csexe_parser_execution_status;
                        break;
                      case Executions::EXECUTION_STATUS_FAILED:
                      case Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY:
                      case Executions::EXECUTION_STATUS_PASSED:
                        if (mts.execution_status!=_csexe_parser_execution_status)
                          mts.execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
                        break;
                    }
                 }
                 ;

csexe_one_status_ : __CSEXE_STATUS__ __STATUS_PASSED__ 
                   { _csexe_parser_execution_status=Executions::EXECUTION_STATUS_PASSED; }
                  | __CSEXE_STATUS__ __STATUS_FAILED__ 
                   { _csexe_parser_execution_status=Executions::EXECUTION_STATUS_FAILED; }
                  | __CSEXE_STATUS__ __STATUS_CHECK_MANUALLY__ 
                   { _csexe_parser_execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY; }
                  ;

csexe_title : __CSEXE_TITLE__ str 
            {
              _csexe_parser_execution_title_file = QString::fromUtf8($2).trimmed();
            }
            | __CSEXE_TITLE__ 
            {
              _csexe_parser_execution_title_file.clear();
            }
            ;

str:  __STRING__ 
   { $$=$1; }
   ;

%%

#include <stdio.h>


bool CSExeParser::csexe_parse(CSMesIO &csmes,const QString &filename,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
{
  info.clear();
  short_status.clear();
  if ( file.open( QIODevice::ReadOnly ) )
  {
    bool ret;
    _errors.clear();
    _csmes_p=&csmes;
    skip_module=false;
    _csexe_parser_execution_title_file.clear();
    _csexe_parser_execution_status=default_execution_status;
    _csexe_parser_execution_title_default=name_orig;
    _csexe_parser_execution_status_default=default_execution_status;
    _csexe_parser_policy=policy;
    init_csexe_parserlex(csmes,filename,file,name_orig,policy,default_execution_status,new_executions,info,short_status,errmsgs,undo_backup_p,progress_p);
    DEBUG2("Start parsing:#%s\n",text_line);
    CSExeParserDriver driver;
    driver.parse(filename);
    ret= ( driver.result!=0 );
    DEBUG3("End parsing(ret=%i):#%s\n",driver.result,text_line);
    file.close();
    return ret;
  }

  short_status=QObject::tr("Error opening I/O device");
  info=short_status;
  return false;
}

ExecutionName CSExeParser::executionName(const ExecutionName &default_name,const ExecutionName &execution_name,CSMesIO::csexe_import_policy_t policy) 
{
  return _csmes_p->executionName(default_name,execution_name,policy) ;
}

bool CSExeParser::createEmptyExecution(Executions::modules_executions_t &mts) 
{
  return _csmes_p->createEmptyExecution(mts);
}

bool CSExeParser::moduleExists(const ModuleFile &m) 
{
  return _csmes_p->moduleExists(m);
}

void yy::CSExeParser::error (const yy::CSExeParser::location_type& l, const std::string& m)
{
 driver.error (l, m);
}
