/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#ifndef __EXECUTIONS_H
#define __EXECUTIONS_H
#include "libinstrumentationpdef.h"
#include <QMap>
#include <QVector>
#include "instrumentation.h"
#include <QString>
#include <QStringList>
#include <QCache>
#include <QMutex>
#include "accesslocker.h"
class CSMESFileThread;

class LIBINSTRUMENTATION_API Executions
{
  public:
    enum execution_loadstate_t
    {
      LOAD_STATE_INVALID,
      LOAD_STATE_DIRTY,
      LOAD_STATE_NOT_LOADED,
      LOAD_STATE_LOADED
    };
    enum execution_status_t 
    {
      EXECUTION_STATUS_PASSED=2,
      EXECUTION_STATUS_FAILED=1,
      EXECUTION_STATUS_TO_BE_CHECK_MANUALLY=3 ,
      EXECUTION_STATUS_UNKNOWN=0 
    } ;
    typedef QVector<Instrumentation::execution_state_t> executions_t;
    typedef QHash<QString,executions_t> list_modules_executions_t;
    struct modules_executions_t {
      QString comment;
      list_modules_executions_t executions;
      execution_status_t execution_status;
    };
    struct modules_executions_private_t {
      modules_executions_t data;
      int execution_section_hint;
      execution_loadstate_t load_state;
    } ;
    Executions();
    executions_t getExecutionModule(const QString &name, const QString & module) const;
    modules_executions_t getExecution(const QString &name) const ;
    bool exists(const QString &name) const;
    bool isLoadedInRAM(const QString &name) const;
    void remove(const QString &name);
    bool moduleExists(const QString &name,const QString &module) const;
    void setExecutionStatus(const QString &name,execution_status_t);
    execution_status_t getExecutionStatus(const QString &name) const;
    void setExecutionComment(const QString &name,const QString &comment);
    void getExecutionComment(const QString &name,QString &comment) const;
    QStringList getExecutionList() const;
    QStringList getModuleList(const QString &name) const;
    QStringList optimizeExecutionListForCaching(const QStringList &) const ;
    void clear();
    void setCSMes(CSMESFileThread *file);
    void setExecutionModule(const QString &name, const QString & module,const Executions::executions_t &e);
    void copy(const QString &dest,const QString &src);
    bool load();
    bool save();
    void getCacheStatistics(int &cache_hit,int &cache_miss,int &max_cost,int &cost) const;
    void setCacheMaxCost(long value);
    void setExecution(const QString &name,const modules_executions_t &exec);
    void getExecution(const QString &execution_name,modules_executions_t &execution) const;
    void restoreExecution(const QString &name,const modules_executions_private_t &exec);
    bool backupExecution(const QString &execution_name,modules_executions_private_t &execution) const;
  private:
    typedef QHash<QString,modules_executions_private_t> list_executions_t;
    void ensureLoaded(const QString &execution_name) const;
    bool preload(const QString &execution_name);
    bool save_old_format();
    bool save_compact_format();
    bool save_executions_old_format();
    bool save_executions_compact_format();
    bool delete_obselete_executions();
    bool save_comment();
    list_executions_t executions;
    CSMESFileThread *csmes_p;
    mutable QMutex locker;
    mutable int cache_miss,cache_hit;
    mutable QCache<QString,list_modules_executions_t> cache;
    void clearExecutionCache();
    mutable AccessLocker access_locker;
} ;

#endif
