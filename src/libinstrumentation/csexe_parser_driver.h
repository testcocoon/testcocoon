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

#ifndef _CSEXE_PARSER_DDRIVER_H_
#define _CSEXE_PARSER_DDRIVER_H_
#include "csexe_parser_yacc.hxx"
#include <QString>
#include <QHash>
#include "csmesio.h"
#include "executions.h"

class CSExeParser; 

// Announce to Flex the prototype we want for lexing function, ...
# define YY_DECL                                      \
 yy::CSExeParser::token_type                         \
 yylex (yy::CSExeParser::semantic_type* yylval,      \
        CSExeParserDriver& driver)                                       
// ... and declare it for the parser's sake.       
YY_DECL;

class CSExeParserDriver
{
  public:
    CSExeParserDriver(CSMesIO &, const CSExeParser &);
    virtual ~CSExeParserDriver();

    // Handling the parser.
    bool parse (const QString & f,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p);
    const QString &file() const { return _file ; }


    void end_csexe_measurement();
    bool begin_csexe_measurement();

    void add_instrumentation(Instrumentation::execution_state_t instrumentation_item)
    {
      if (_mes_p)
      {
        if (_mes_p_index>static_cast<unsigned int>(_mes_p->size()))
        {
          _mes_p=NULL;
          mark_execution_as_wrong(QObject::tr("Invalid number of executions (too many instrumentation per file)"));
          return;
        }
        (*_mes_p)[_mes_p_index]=Instrumentation::combineExecution( (*_mes_p)[_mes_p_index] , instrumentation_item ) ;
        _mes_p_index++;
      }
    }
    void set_status(Executions::execution_status_t s) ;
    void set_title(const QString &);
    void init_add_instrumentation(const QString &module,long nb_mes, unsigned long signature);
    void endup_add_instrumentation();
    void begin_measurement();
    ExecutionName executionName(const ExecutionName &default_name,const ExecutionName &execution_name,CSMesIO::csexe_import_policy_t policy) ;

    // Error handling.
    void error (const QString & m);
    int line() const {return _line;}
    void incrementLine() { _line++; }

  private:
    void mark_execution_as_wrong(const QString &e) ;
    CSMesIO &_csmes;
    int _line;
    QString _file;
    const CSExeParser &_csexe_parser;
    Executions::modules_executions_t _mts;
    Executions::execution_status_t _execution_status;
    ExecutionName _execution_title;
    ExecutionName _execution_title_file;
    QString _errmsg;
    QString _short_status;
    QString _errmsgs;
    QString _info;
    ExecutionNames _mes_new,_mes_modif,_new_executions;
    bool _skip_module;
    bool _wrong_executions;
    Executions::executions_t* _mes_p ;
    int _mes_p_index;
    QString _detailled_info;
    int _nb_mes_duplicates;
    int _execution_nr;
    int _nb_mes_invalid;
    QHash<ExecutionName,Executions::modules_executions_private_t> *_undo_backup_p;
    CSMesIO::progress_function_t _progress_p;
    float _max_file_size;
    qint64 _flushpos;
};

#endif
