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
#include "csexe_parser_driver.h"
#include <stdio.h>
#include <QHash>
#include <QIODevice>
#include "csmesio.h"

class CSExeParser : public virtual CSMesIO
{
  public:
    bool csexe_parse(CSMesIO &csmes,const QString &filename,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p);

  private:
    static void init_csexe_parserlex(CSMesIO &csmes,const QString &filename,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p);

  public:
    static inline ExecutionName executionName(const ExecutionName &default_name,const ExecutionName &execution_name,CSMesIO::csexe_import_policy_t policy) ;
    static inline bool createEmptyExecution(Executions::modules_executions_t &) ;
    static inline bool moduleExists(const ModuleFile &m) ;

    static QIODevice *qiodevice_p;
    static int yy_input_qiodevice(char *buf,int max_size);
};

#endif


