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

#ifndef CSMES_IO_H
#define CSMES_IO_H
#include <qstring.h>
#include <qstringlist.h>
#include <QHash>
#include "csmescomment.h"
#include "csmesfilethread.h"
class CSMesIO : public CSMesComment
{
  protected:
    CSMesIO();
  public:
    typedef bool (*progress_function_t)(float,bool);
    enum csexe_import_policy_t 
    {
      CSEXE_POLICY_IMPORT_DUPLICATES=0,
      CSEXE_POLICY_IGNORE_DUPLICATES=1,
      CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY=2,
      CSEXE_POLICY_MERGE_ALL_TOGETHER=3,
      CSEXE_POLICY_INVALID
    } ;
    bool hasConditionInstrumentation() const { return has_condition_instrumentation; }
    virtual ~CSMesIO();
  protected:
    LIBINSTRUMENTATION_API bool lock_csexe(QIODevice &file);
    LIBINSTRUMENTATION_API bool unlock_csexe(QIODevice &file);
    bool loadCSExe(QIODevice &data,ExecutionName name,csexe_import_policy_t,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsg,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,progress_function_t);
    bool loadCSMes(const QString &file);
    bool mergeCSMes(const QString &s,CSMESFile::merge_policy_t policy);
    bool saveCSMes();
    bool flushCSMes();
    void closeCSMes();
    void clear();
    bool isSourceExisting(ModuleFile module,SourceFile source, CSMESFile::type_t t) const;
    bool saveBlackBoxConfiguration(const QString &s) ;
    bool isBlackBoxConfiguration() const;
    bool isCoverageBranchOnly() ;
    const QString &getFilename() const { return filename ; }
    const QString &error() const { return err; }
    void read_source(const QString &module,const QString &source, CSMESFile::type_t t,QString &qbuf) const;
    unsigned long checksum(const ModuleFile &module,const SourceFile &source) const;
  private:
    CSMESFileThread csmes;
    QString filename;
  protected:
    QString err;
  private:
    bool load_csexe_one(Executions::modules_executions_t &execution, ExecutionName &name,QString &err,QIODevice &data,QString &last_line,int &line_nr,const ExecutionName &name_orig,csexe_import_policy_t policy,Executions::execution_status_t default_execution_status) const;
    bool isCSExeDirectory(const ExecutionName &) const;
    ExecutionName executionName(const ExecutionName &default_name,const ExecutionName &execution_name,csexe_import_policy_t policy) const;
    bool has_condition_instrumentation;
};

#endif
