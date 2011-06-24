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

#ifndef CSEXE_PARSE_INCLUDE_H
#define CSEXE_PARSE_INCLUDE_H

#include "csexe_parser_yacc.hxx"
#include <stdio.h>
#include <QHash>
#include <QIODevice>
#include "csmesio.h"

long csexe_parse(CSMesIO &csmes,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p);

/* debug function for bison */
int csexe_yyprint(FILE *f,int type,YYSTYPE value);
#define  YYPRINT(file,type,value) csexe_yyprint(file,type,value);

void init_csexe_parserlex(CSMesIO &csmes,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p);
int csexe_parsererror(const char *s);
#endif


