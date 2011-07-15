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

#define CSEXE_LEX_YACC_PARSER 1
#include "libinstrumentationpdef.h"
#include "csmesio.h"
#include "Service.h"
#include <QObject>
#include <QThread>
#include <QTextDocument>
#include <QFile>
#include <stdio.h>
#include <QDateTime>
#include <QTextStream>
#include <QProcess>
#include <QRegExp>
#include <QTime>
#include "instrumentation.h"
#ifdef OS_WIN32
#include <windows.h>
#endif
#if CSEXE_LEX_YACC_PARSER
#include "csexe_parser.h"
#endif


CSMesIO::CSMesIO() : CSMesComment()
{
  has_condition_instrumentation=true;
}

CSMesIO::~CSMesIO()
{
  clear();
}

bool CSMesIO::lock_csexe(QIODevice &file)
{
   const int lock_max_time=300000;
  const QFile *file_real_file=dynamic_cast<const QFile*>(&file);
  if (file_real_file)
  {
    QString csexe_filename=file_real_file->fileName();
    QString csexe_lock_filename=csexe_filename+".lck";
    QFile csexe_lock_file(csexe_lock_filename);
    QTime startTime;
    startTime.start();
    QByteArray check_string="cb:";
#ifdef OS_WIN32
    QString pid=QString::number(GetCurrentProcessId());
#elif defined (OS_LINUX) || defined(OS_MACX)
    QString pid=QString::number(getpid());
#endif
    check_string+=pid;

    for (;startTime.elapsed()<lock_max_time;)
    {
      bool can_lock=false;
      if (csexe_lock_file.exists())
      {
        if (csexe_lock_file.open(QIODevice::ReadOnly))
        {
          QByteArray reread=csexe_lock_file.read(100);
          csexe_lock_file.close();
          if (reread.length()==0)
            can_lock=true;
        }
      }
      else
        can_lock=true;
      if (can_lock)
      {
        if (csexe_lock_file.open(QIODevice::WriteOnly))
        {
          csexe_lock_file.write(check_string);
          csexe_lock_file.close();
          if (csexe_lock_file.open(QIODevice::ReadOnly))
          {
            QByteArray reread=csexe_lock_file.read(100);
            csexe_lock_file.close();
            if (reread==check_string)
              return true;
          }
        }
      }
#ifdef OS_WIN32
      Sleep(1000);
#else
      sleep(1);
#endif
      printStatus(QObject::tr("Trying to get the lock file (%1)").arg(csexe_lock_filename),static_cast<float>(startTime.elapsed())/static_cast<float>(lock_max_time));
    }
    err=QObject::tr("Cannot lock file '%1'").arg(csexe_lock_filename);
    return false;
  }
  return true;
}

bool CSMesIO::unlock_csexe(QIODevice &file)
{
  const QFile *file_real_file=dynamic_cast<const QFile*>(&file);
  if (file_real_file)
  {
    QString csexe_filename=file_real_file->fileName();
    QString csexe_lock_filename=csexe_filename+".lck";
    QFile csexe_lock_file(csexe_lock_filename);
    csexe_lock_file.remove();
  }
  return true;
}

bool CSMesIO::isCSExeDirectory(const ExecutionName &s) const
{
  if (s.isEmpty())
    return true;
  if (s.right(1)=="/")
    return true;

  return false;
}

ExecutionName CSMesIO::executionName(const ExecutionName &default_name,const ExecutionName &execution_name,csexe_import_policy_t policy) const
{
  ExecutionName name;
  if (execution_name.isEmpty())
    name=default_name;
  else
    name=execution_name;

  switch (policy)
  {
    case CSEXE_POLICY_INVALID:
      ASSERT(false);
      break;
    case CSEXE_POLICY_MERGE_ALL_TOGETHER:
      if (isCSExeDirectory(name))
        name+=QObject::tr("Unnamed");
      break;
    case CSEXE_POLICY_IMPORT_DUPLICATES:
    case CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY:
    case CSEXE_POLICY_IGNORE_DUPLICATES:
      {
        QString ret;
        QString basename=name;

        if (isCSExeDirectory(basename))
          basename+=QObject::tr("Unnamed");
        const int basename_lg=basename.length();
        ExecutionNames execs=executions.getExecutionList();
        int count=0;
        for (ExecutionNames::const_iterator it=execs.begin();it!=execs.end();++it)
        {
          const QString &item=*it;
          if ( item.startsWith(basename) )
          {
            const int item_lg=item.length();
            if (item_lg==basename_lg)
              count++;
            else
            {
              if (item_lg>=basename_lg+4)
              {
                int pos=basename_lg;
                if (item[pos]==' ')
                {
                  pos++;
                  if (item[pos]=='(')
                  {
                    bool found=true;
                    pos++;
                    for (;pos<item_lg-1;pos++)
                    {
                      if ( ! (item[pos]>='0' && item[pos]<='9') )
                      {
                        found=false;
                        break;
                      }
                    }
                    if (found)
                    {
                      if (item[pos]==')')
                        count++;
                    }
                  }
                }
              }
            }
          }
        }
        ret.reserve(basename.length()+50);
        int i=count+1;

        do
        {
          ret=basename;
          if (i>1)
            ret+=" ("+QString::number(i)+")";
          i++;
        }
        while (executions.exists(ret));
        name=ret;
      }
      break;
  }
  return name;
}

static inline unsigned int hex2uint(const char *s)
{
  unsigned int v=0;
  ASSERT(strlen(s)==8);
  for (int i=0;i<8;i++)
  {
    char c=s[i] ;
    if (c<='9' && c>='0')
      c-='0';
    else
      c-='A'-10;
    v|=c<<(4*(7-i));
  }
  return v;
}


bool CSMesIO::load_csexe_one(Executions::modules_executions_t &mts, ExecutionName &name,QString &err,QIODevice &file,QString &line,int &line_nr,const ExecutionName &name_orig,csexe_import_policy_t policy,Executions::execution_status_t default_execution_status) const
{
  err.clear();
  bool import_error=false;
  bool skip_module=false;
  Executions::execution_status_t execution_status=Executions::EXECUTION_STATUS_UNKNOWN;
  QRegExp module_rx("^/(\\d*):(\\d*):(.*)$");
  QString name_in_csexe_file;
  name.clear();
  // name=executionName(name_orig,name_in_csexe_file,policy);
  bool wrong_executions=false;
  bool execution_complete=false;
  bool measurement_header_detected=false;
  createEmptyExecution(mts);

  bool first_time=true;
  Executions::executions_t *mes_p=NULL;
  do
  {
    if (!first_time)
    {
      line = file.readLine(0);
      line_nr++;
    }
    else
      first_time=false;
    if (!line.isEmpty())
    {
      QChar lastChar=line.at(line.length()-1);
      line.remove('\r');
      line.remove('\n');
    }
    if (!line.isEmpty())
    {
      QChar first_char=line.at(0);
      if (line == "# Measurements")
      { // # Executions
        if (measurement_header_detected)
          execution_complete=true;
        else
        {
          execution_status=Executions::EXECUTION_STATUS_UNKNOWN;
          name=executionName(name_orig,name_in_csexe_file,policy);
          name_in_csexe_file=QString();
          measurement_header_detected=true;
        }
      }
      else if (first_char=='!')
      { // !execution status
        if (measurement_header_detected)
        {
          QString statusstr=line.mid(1);
          if (statusstr=="PASSED")
            execution_status=Executions::EXECUTION_STATUS_PASSED;
          else if (statusstr=="FAILED")
            execution_status=Executions::EXECUTION_STATUS_FAILED;
          else if (statusstr=="CHECK_MANUALLY")
            execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
          switch (mts.execution_status)
          {
            case Executions::EXECUTION_STATUS_UNKNOWN:
                mts.execution_status=execution_status;
              break;
            case Executions::EXECUTION_STATUS_FAILED:
            case Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY:
            case Executions::EXECUTION_STATUS_PASSED:
              if (mts.execution_status!=execution_status)
                mts.execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
              break;
          }
        }
      }
      else if (first_char=='§' || first_char=='*')
      { // §instrumentation name
        if (!measurement_header_detected)
        {
          name=line.mid(1);
          name_in_csexe_file=name;
        }
        else
          execution_complete=true;
      }
      else if (first_char=='/' && module_rx.indexIn(line)!=-1)
      {
        if (measurement_header_detected)
        {
          skip_module=false;
          long nb_mes=module_rx.cap(1).toLong();
          unsigned long signature=module_rx.cap(2).toULong();
          QString module=module_rx.cap(3).toAscii();

          if (!moduleExists(module))
          {
            skip_module=true;
            continue; /* skip additional files */
          }
          else
          {
            if (instrumentations.modules[module].signature!=signature)
              wrong_executions=true;
            else if (nb_mes!=nbInstrumentations(module))
              wrong_executions=true;
          }

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
      }
      else if (first_char=='\\')
      {
        if (measurement_header_detected)
        {
          if (skip_module)
            continue; /* skip additional files */
          if (wrong_executions)
            continue;
          unsigned int max=line.length();
          unsigned int i;
          unsigned int nb_mes=0;

          if (mes_p==NULL)
          {
            err=QObject::tr("Invalid format")
              +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
            wrong_executions=true;
            continue;
          }
          i=0;
          for (int ichar=1;ichar<static_cast<int>(max);ichar++)
          {
            char c=line[ichar].toAscii();
            Instrumentation::execution_state_t instrumentation_item;
            switch (c)
            {
              case '?':
                instrumentation_item=Instrumentation::EXECUTION_STATE_HIDDEN;
                break;
              case '+':
                instrumentation_item=static_cast<Instrumentation::execution_state_t>(1) ;
                break;
              case '-':
                instrumentation_item=Instrumentation::EXECUTION_STATE_NOT_EXECUTED;
                break;
              default:
                char hex[9];
                for (int k=0;k<8;k++)
                {
                  if ( (c>='0' && c<='9') || (c>='A' && c<='F') )
                    hex[k]=c;
                  else
                  {
                    err=QObject::tr("Invalid execution")
                      +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
                    import_error=true;
                    break;
                  }
                  hex[8]='\0';
                  c=line[++ichar].toAscii();
                }
                unsigned int v=hex2uint(hex);
                if (v>Instrumentation::EXECUTION_MAX)
                  v=Instrumentation::EXECUTION_MAX;
                instrumentation_item=static_cast<Instrumentation::execution_state_t>(v);
                ichar--;
                break;
            }
            (*mes_p)[i]=Instrumentation::combineExecution( (*mes_p)[i] , instrumentation_item ) ;
            i++;
            nb_mes++;
            if (nb_mes>static_cast<unsigned int>(mes_p->size()))
            {
              err=QObject::tr("Invalid number of executions (too many instrumentation per file)")
                      +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
              import_error=true;
              break;
            }
          }
          if (nb_mes!=static_cast<unsigned int>(mes_p->size()))
          {
            err=QObject::tr("Invalid number of executions")
              +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
            import_error=true;
          }
          mes_p=NULL;
        }
      }
      else
      {
        err=QObject::tr("Wrong file format")
          +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
        import_error=true;
      }
    }
    if (import_error)
      break;
    if (execution_complete)
      break;
  }
  while ( !file.atEnd() ) ;
  if (mts.execution_status==Executions::EXECUTION_STATUS_UNKNOWN)
    mts.execution_status=default_execution_status;

  return !import_error;
}

bool CSMesIO::loadCSExe(QIODevice &file,const ExecutionName &name_orig,csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,progress_function_t progress_p)
{
  bool result=true;
#if CSEXE_LEX_YACC_PARSER
  QString filename;
  QFile *file_p=dynamic_cast<QFile*>(&file);
  if (file_p)
    filename=file_p->fileName();
  CSExeParser csexe_parser(*this,name_orig,policy,default_execution_status);
  result = csexe_parser.parse(filename,file,new_executions,info,short_status,errmsgs,undo_backup_p,progress_p);
#else
  QTime timeWatch;
  errmsgs.clear();
  timeWatch.restart();
  if (undo_backup_p)
    undo_backup_p->clear();
  bool use_progress= (!file.isSequential()) && (progress_p!=NULL);
  float max_file_size=file.size();
  ExecutionNames mes_new,mes_modif;
  QRegExp module_rx("^/(\\d*):(\\d*):(.*)$");
  int nb_mes_duplicates=0;
  int nb_mes_invalid=0;
  new_executions.clear();
  QString detailled_info="<TABLE cellpadding=\"3\" cellspacing=\"0\" border=\"1\">";
  detailled_info+="<TR>";
  detailled_info+="<TD><B>#</TD>";
  detailled_info+="<TD><B>"+QObject::tr("Name")+"</B></TD>";
  detailled_info+="<TD><B>"+QObject::tr("Status")+"</B></TD>";
  detailled_info+="<TD><B>"+QObject::tr("Description")+"</B></TD>";
  detailled_info+="</TR>";

  if ( file.open( QIODevice::ReadOnly ) )
  {
    qint64 flushpos=file.pos();
    bool interrupt=false;
    QString line;
    int line_nr=0;
    int execution_nr=0;
    while (!file.atEnd())
    {
      if (use_progress)
        interrupt=progress_p(((float)file.pos())/max_file_size,false);
      if (interrupt)
        break;
      execution_nr++;
      Executions::modules_executions_t mts;
      ExecutionName name;
      QString errmsg;
      detailled_info+="<TR>";
      detailled_info+="<TD>"+QString::number(execution_nr)+"</TD>";
      bool import_execution=load_csexe_one(mts, name,errmsg,file,line,line_nr,name_orig,policy,default_execution_status);
      if (!import_execution)
      {
        detailled_info+="<TD></TD>";
        detailled_info+="<TD></TD>";
        detailled_info+="<TD>"+QObject::tr("Invalid Execution")+ "(" + Qt::escape(errmsg) + ")" +"</TD>";
        if (!errmsgs.isEmpty())
          errmsgs+="\n";
        errmsgs+=errmsg;
        nb_mes_invalid++;
      }
      else
      {
        detailled_info+="<TD><TT>"+Qt::escape(name)+"</TT></TD>";
        detailled_info+="<TD bgcolor=\""+executionStatusColor(mts.execution_status).name()+"\">"+executionsStatusStr()[mts.execution_status]+"</TD>";
        bool duplicate=false;
        ExecutionName duplicate_execution;
        bool empty_execution=false;
        if (policy!=CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY)
            empty_execution=emptyExecution(mts);
        if (policy==CSEXE_POLICY_IGNORE_DUPLICATES && !empty_execution)
        {
          duplicate_execution=duplicateExecution(mts,ExecutionNames());
          duplicate = ! duplicate_execution.isEmpty();
        }

        if (empty_execution)
          ; // do nothing
        else if (duplicate)
          nb_mes_duplicates++;
        else if (executions.exists(name))
        {
          if ( ! (mes_modif.contains(name) || mes_new.contains(name)) )
            mes_modif+=name;
        }
        else
          mes_new+=name;

        switch (policy)
        {
          case CSEXE_POLICY_INVALID:
            ASSERT(false);
            break;
          case CSEXE_POLICY_MERGE_ALL_TOGETHER:
            if (empty_execution)
              detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
            else
            {
              detailled_info+="<TD>"+QObject::tr("merged with the existing execution.")+"</TD>";
              if (undo_backup_p)
              {
                if (!undo_backup_p->contains(name))
                {
                  Executions::modules_executions_private_t exec;
                  bool ret = backupExecution(name,exec);
                  if (ret)
                    (*undo_backup_p)[name]=exec;
                }
              }
              mergeInExecution(name,mts);
            }
            break;
          case CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY:
            detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
            if (undo_backup_p)
            {
              if (!undo_backup_p->contains(name))
              {
                Executions::modules_executions_private_t exec;
                bool ret = backupExecution(name,exec);
                if (ret)
                  (*undo_backup_p)[name]=exec;
              }
            }
            executions.setExecution(name,mts);
            break;
          case CSEXE_POLICY_IMPORT_DUPLICATES:
            if (empty_execution)
              detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
            else if (duplicate)
              detailled_info+="<TD>"+QObject::tr("not imported: same as '%1'.").arg("<TT>"+Qt::escape(duplicate_execution)+"</TT>")+"</TD>";
            else
            {
              detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
              if (undo_backup_p)
              {
                if (!undo_backup_p->contains(name))
                {
                  Executions::modules_executions_private_t exec;
                  bool ret = backupExecution(name,exec);
                  if (ret)
                    (*undo_backup_p)[name]=exec;
                }
              }
              executions.setExecution(name,mts);
            }
            break;
          case CSEXE_POLICY_IGNORE_DUPLICATES:
            if (empty_execution)
              detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
            else if (duplicate)
              detailled_info+="<TD>"+QObject::tr("not imported: same as '%1'.").arg("<TT>"+Qt::escape(duplicate_execution)+"</TT>")+"</TD>";
            else
            {
              detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
              if (undo_backup_p)
              {
                if (!undo_backup_p->contains(name))
                {
                  Executions::modules_executions_private_t exec;
                  bool ret = backupExecution(name,exec);
                  if (ret)
                    (*undo_backup_p)[name]=exec;
                }
              }
              executions.setExecution(name,mts);
            }
            break;
        }
      }
      detailled_info+="</TR>";

      if (undo_backup_p==NULL)
      {
        qint64 pos=file.pos();
        static const qint64 flush_size=100*1024*1024;
        if (pos-flushpos>flush_size)
        {
          flushCSMes(); // Write executions to minimize memory usage
          flushpos=pos;
        }
      }
    }
    file.close();
    detailled_info+="</TABLE>";
  }

  info=QString::null;
  ExecutionNames::Iterator it;
  if ( (!mes_new.isEmpty()) || (!mes_modif.isEmpty()))
    setModificationFlag();

  info+="<HTML><BODY>";
  if (!mes_new.isEmpty())
  {
    info+=QObject::tr("New executions added:")+"<BR>";
    info+="<UL>";
    for ( it = mes_new.begin(); it != mes_new.end(); ++it )
      info+="<LI><TT>"+Qt::escape(*it)+"</TT></LI>";
    info+="</UL>";
    info+="<BR>";
  }

  if (!mes_modif.isEmpty())
  {
    info+=QObject::tr("Modified executions:")+"<BR>";
    info+="<UL>";
    for ( it = mes_modif.begin(); it != mes_modif.end(); ++it )
      info+="<LI><TT>"+Qt::escape(*it)+"</TT></LI>";
    info+="</UL>";
    info+="<BR>";
  }

  if (nb_mes_invalid>0)
  {
    info+="<HR>";
    info+=QObject::tr("Number of invalid executions (not loaded):")+QString::number(nb_mes_invalid)+"<BR>";
    info+="<BR>";
  }

  if (nb_mes_duplicates>0)
  {
    info+="<HR>";
    info+=QObject::tr("Duplicate executions (not loaded):")+QString::number(nb_mes_duplicates);
    info+="<BR>";
  }
  info+="<P>";
  info+=QObject::tr("Detailed information:");
  info+="<BR>";
  info+=detailled_info;
  info+="</P>";
  info+="</HTML></BODY>";

  new_executions=mes_new;
  new_executions+=mes_modif;

  short_status.clear();
  if (mes_new.count()>0)
    short_status += QObject::tr("%1 new executions").arg(mes_new.count());
  if (mes_modif.count()>0)
  {
    if (!short_status.isEmpty()) short_status+=", ";
    short_status += QObject::tr("%1 merged executions").arg(mes_modif.count());
  }
  if (nb_mes_duplicates>0)
  {
    if (!short_status.isEmpty()) short_status+=", ";
    short_status += QObject::tr("%1 duplicates (not loaded)").arg(nb_mes_duplicates);
  }
  if (nb_mes_invalid)
  {
    if (!short_status.isEmpty()) short_status+=", ";
    short_status += QObject::tr("%1 invalid executions (not loaded)").arg(nb_mes_invalid);
  }

  if (use_progress)
    progress_p(1.0,true);
  double loadTime=static_cast<double>(timeWatch.elapsed())/1000.0;
  printStatus(QObject::tr("Execution report loaded. (%1s)").arg(QString::number(loadTime,'f',3)),-1.0);
#endif
  update();
  return result;
}

void CSMesIO::clear()
{
  closeCSMes();
  CSMesComment::clear();
}

void CSMesIO::closeCSMes()
{
  if (csmes.isOpen())
    csmes.close();
}

bool CSMesIO::mergeCSMes(const QString &file_to_merge,CSMESFile::merge_policy_t policy)
{
  printStatus(QObject::tr("Mergin with '%1' ...").arg(file_to_merge),-1.0);
  QString opened_filename=filename;
  QByteArray opened_filename_cstr=opened_filename.toAscii();
  if (!saveCSMes())
    return false;

  bool was_open=csmes.isOpen();
  if (was_open)
    csmes.close();
  QByteArray filename_cstr=filename.toAscii();
  if (!csmes.open(filename_cstr.constData(),CSMESFile::APPEND_RW))
  {
    err=QObject::tr("Could not open")+" '"+filename+"':"+QString::fromStdString(csmes.getLastErrorMsg());
    return false;
  }
  QByteArray file_to_merge_cstr=file_to_merge.toAscii();
  CSMESFile csmesToMerge;
  if (!csmesToMerge.open(file_to_merge_cstr.constData(),CSMESFile::RO))
  {
    err=QObject::tr("Could not open file")+" '"+filename+"':"+QString::fromStdString(csmesToMerge.getLastErrorMsg());
    return false;
  }
  std::string errmsg;
  if (!csmes.merge(csmesToMerge,0,policy,errmsg))
  {
    err=QObject::tr("Could not merge")+":"+QString::fromStdString(errmsg);
    return false;
  }
  csmes.close();
  if (was_open)
    csmes.open(filename_cstr.constData(),CSMESFile::RO);

  resetModificationFlag();
  if (!loadCSMes(opened_filename))
    return false;
  return true;
}

bool CSMesIO::loadCSMes(const QString &s)
{
  has_condition_instrumentation=false;
  QTime timeWatch;
  timeWatch.restart();
  if (!QFile::exists(s))
  {
      err=QObject::tr("File not exists:")+s;
      return false;
  }
  clear();
  filename=s;
  if (csmes.isOpen())
    csmes.close();
  QByteArray filename_cstr=filename.toAscii();
  if (!csmes.open(filename_cstr.constData(),CSMESFile::RO))
  {
    clear();
    err=QObject::tr("Could not open")+" '"+s+"':"+QString::fromStdString(csmes.getLastErrorMsg());
    return false;
  }

  printStatus(QObject::tr("Reading Instrumentation..."),0.0);
  for (int sec_id=0;sec_id<csmes.nbSections();sec_id++)
  {
    switch (csmes.sectionType(sec_id))
    {
       case CSMESFile::_INSTRUMENTATION_V1:
       case CSMESFile::_INSTRUMENTATION_V2:
        {
          QString mod(csmes.sectionModule(sec_id));
          QString src(csmes.sectionName(sec_id));
          if (!instrumentations.modules.contains(mod))
          {
            CSMesInstrumentations::Module v;
            v.signature=csmes.signature(sec_id);
            v.module_relative_name=ModuleFile(csmes.sectionModuleRelative(sec_id));
            v.nb_measurements_items=0;
            instrumentations.modules[mod]=v;
          }
          int nb_index=instrumentations.modules[mod].nb_measurements_items;
          if (!instrumentations.modules[mod].sources.contains(src))
          {
            CSMesInstrumentations::Instrumentations in;
            CSMesInstrumentations::Functions fi;
            instrumentations.modules[mod].sources[src].functions=fi;
            instrumentations.modules[mod].sources[src].instrumentations=in;
            instrumentations.modules[mod].source_relative_name[src]=csmes.sectionNameRelative(sec_id);
          }
          CSMesInstrumentations::Instrumentations *inst_p=&instrumentations.modules[mod].sources[src].instrumentations;
          if (csmes.openSectionID(sec_id))
          {
            inst_p->resize(csmes.size()/24);
            int _nb_instrumentation=0;
            for (int j=0;j<csmes.size();j++)
            {
              long index;
              long _nb_index;
              CSMESFile::instrumentation_t t;
              std::vector<CSMESFile::instrumentation_part_t> instrumentation_parts;
              CSMESFile::instrumentation_childrens_t childrens;
              long s_line_pre;
              long s_line_org;
              long s_column_pre;
              long s_column_org;
              long e_line_org;
              long e_line_pre;
              long e_column_org;
              long e_column_pre;
              _I64 identifier;
              if (csmes.readInstrumentation(identifier,index,_nb_index, t, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_org, e_column_org,e_line_pre, e_column_pre, instrumentation_parts, childrens))
              {
                _nb_instrumentation++;
                Instrumentation::type_t type;
                switch (t)
                {
                  case CSMESFile::_INSTRUMENTATION_NOP:                 type=Instrumentation::NOP;                   break;
                  case CSMESFile::_INSTRUMENTATION_LABEL:               type=Instrumentation::LABEL;                break;
                  case CSMESFile::_INSTRUMENTATION_COMPUTATION:         type=Instrumentation::COMPUTATION;           break;
                  case CSMESFile::_INSTRUMENTATION_CONDITION_FULL:      type=Instrumentation::CONDITION_FULL;        break;
                  case CSMESFile::_INSTRUMENTATION_CONDITION_FALSE_ONLY:type=Instrumentation::CONDITION_FALSE_ONLY;  break;
                  case CSMESFile::_INSTRUMENTATION_CONDITION_TRUE_ONLY: type=Instrumentation::CONDITION_TRUE_ONLY;   break;
                  default:                                              type=Instrumentation::NOP;                   ASSERT(0); ;
                }
                (*inst_p)[j]=Instrumentation(index, type,s_line_org,s_column_org,e_line_org,e_column_org,s_line_pre,s_column_pre,e_line_pre,e_column_pre);
                int max_index=(*inst_p)[j].getMaxIndex()+1;
                if (max_index>nb_index) nb_index=max_index;
                has_condition_instrumentation = has_condition_instrumentation || (*inst_p)[j].isConditionInstrumentation();
              }
              else
                break;
            }
            inst_p->resize(_nb_instrumentation);
            csmes.closeSection();
            instrumentations.modules[mod].nb_measurements_items=nb_index;
          }
        }
        break;
      default:
        break;
    }
  }


  printStatus(QObject::tr("Reading Executions..."),0.166f);
  executions.setCSMes(&csmes);
  if (!executions.load())
    return false;

  printStatus(QObject::tr("Reading Manually Validated Sections..."),0.333f);
  for (int sec_id=0;sec_id<csmes.nbSections();sec_id++)
  {
    switch (csmes.sectionType(sec_id))
    {
      case CSMESFile::MANUAL_VALIDATION:
        {
          QString mod(csmes.sectionModule(sec_id));
          QString src(csmes.sectionName(sec_id));
          if (!instrumentations.modules.contains(mod))
          {
            clear();
            return false;
          }
          if (!instrumentations.modules[mod].sources.contains(src))
          {
            clear();
            return false;
          }
          if (csmes.openSectionID(sec_id))
          {
            long nb_elem;
            csmes.readLong(nb_elem);
            for (long i=0;i<nb_elem;i++)
            {
              long index;
              csmes.readLong(index);
              setManuallyValidated_intern(mod,src,index,true);
            }
            csmes.closeSection();
          }
        }
        break;
      default:
        break;
    }
  }


  printStatus(QObject::tr("Reading Comments..."),0.500);
  for (int sec_id=0;sec_id<csmes.nbSections();sec_id++)
  {
    switch (csmes.sectionType(sec_id))
    {
      case CSMESFile::COMMENT:
        {
          QRegExp mergeRx;
          mergeRx.setCaseSensitivity(Qt::CaseInsensitive);
          mergeRx.setMinimal(true);
          mergeRx.setPattern("</HTML>.*<HTML>");
          mergeRx.setPatternSyntax(QRegExp::RegExp);

          QString mod(csmes.sectionModule(sec_id));
          QString src(csmes.sectionName(sec_id));
          if (!comments.contains(mod))
          {
            sources_comments_t v;
            comments[mod]=v;
          }
          if (!comments[mod].contains(src))
          {
            comments_t v;
            comments[mod][src]=v;
          }
          if (csmes.openSectionID(sec_id))
          {
            long nb_elem;
            csmes.readLong(nb_elem);
            for (long i=0;i<nb_elem;i++)
            {
              long index;
              char *text;
              csmes.readComment(index,&text);
              QString comment=QString::fromUtf8(text);
              free(text);
              comment = comment.replace(mergeRx,"<HR>");
              comments[mod][src][index]=comment;
            }
            csmes.closeSection();
          }
        }
        break;
      default:
        break;
    }
  }

  printStatus(QObject::tr("Reading Namespaces/Classes/Methods..."),0.666f);
  for (int sec_id=0;sec_id<csmes.nbSections();sec_id++)
  {
    switch (csmes.sectionType(sec_id))
    {
      case CSMESFile::_FUNCTIONS_INFO_V2:
      case CSMESFile::_FUNCTIONS_INFO_V1:
        {
          QString mod(csmes.sectionModule(sec_id));
          QString src(csmes.sectionName(sec_id));
          QString mod_rel(csmes.sectionModuleRelative(sec_id));
          QString src_rel(csmes.sectionNameRelative(sec_id));

          if  (!instrumentations.modules.contains(mod))
             break;
          if (!instrumentations.modules[mod].sources.contains(src))
             break;

          CSMesInstrumentations::Functions *funct_p=&instrumentations.modules[mod].sources[src].functions;
          if (csmes.openSectionID(sec_id))
          {
            while (true)
            {
              char *scoped_name=NULL;
              char *proto=NULL;
        long s_line_orig, s_column_orig, s_line_pre, s_column_pre,e_line_orig, e_column_orig,e_line_pre, e_column_pre;
              if (csmes.readFunctionInfo(&scoped_name,&proto,s_line_orig, s_column_orig, s_line_pre, s_column_pre,e_line_orig, e_column_orig,e_line_pre, e_column_pre))
        {
        if (   s_line_orig     == -1L
          && s_column_orig   == -1L
          && s_line_pre      == -1L
          && s_column_pre    == -1L
          && e_line_orig     == -1L
          && e_column_orig   == -1L
          && e_line_pre      == -1L
          && e_column_pre    == -1L)
        {
          free(proto);
          free(scoped_name);
          break;
        }
        funct_p->append(FunctionInfo(QString(scoped_name),QString(proto).simplified(),s_line_orig, s_column_orig,e_line_orig, e_column_orig, s_line_pre,s_column_pre,e_line_pre, e_column_pre));
        free(proto);
        free(scoped_name);
        }
            }
            csmes.closeSection();
          }
        }
        break;
      default:
        break;
    }
  }
  printStatus(QObject::tr("Database Generation..."),0.833f);
  generateEquivalences(instrumentations);
  double loadTime=static_cast<double>(timeWatch.elapsed())/1000.0;
  clearExecutions();
  resetModificationFlag();
  update();
  printStatus(QObject::tr("File '%1' loaded. (%2s)").arg(s).arg(QString::number(loadTime,'f',3)),-1.0);
  return true;
}

bool CSMesIO::flushCSMes()
{
  if (!csmes.isOpen())
    return false;
  printStatus(QObject::tr("Saving new executions..."),0.0);
  if (!executions.save())
    return false;

  printStatus(QObject::tr("Saving manually validated sections..."),0.5);
  /* Manually Validated sections */
  CSMesInstrumentations::Modules::const_iterator mod_man_it;
  for (mod_man_it= instrumentations.modules.begin(); mod_man_it != instrumentations.modules.end(); ++mod_man_it )
  {
    CSMesInstrumentations::Sources::const_iterator src_it;
    QString mod=mod_man_it.key();
    QByteArray mod_cstr=mod.toAscii();
    for (src_it= (*mod_man_it).sources.begin(); src_it != (*mod_man_it).sources.end(); ++src_it )
    {
      QString src=src_it.key();
      QByteArray src_cstr=src.toAscii();
      const CSMesInstrumentations::Instrumentations *inst_p=&(src_it.value().instrumentations);
      CSMesInstrumentations::Instrumentations::const_iterator it;

      long nb_index=0;
      for (it= inst_p->begin(); it != inst_p->end(); ++it )
        if (it->getManuallyValidated())
          nb_index++;

      if (nb_index>0)
      {
        bool overwrite=false;
        if (csmes.sectionExists(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::MANUAL_VALIDATION))
        {
          if (csmes.openSection(mod_cstr.constData(),"",src_cstr.constData(),"","",CSMESFile::MANUAL_VALIDATION,0,CSMESFile::RO,0))
          {
            long nb_index_rec=0;
            csmes.readLong(nb_index_rec);
            if (nb_index!=nb_index_rec)
              overwrite=true;
            else
            {
              long id;
              for (int i=0;i<nb_index_rec;i++)
              {
                csmes.readLong(id);
                if (! (*inst_p)[id].getManuallyValidated())
                  overwrite=true;
              }
            }
            csmes.closeSection();
          }
        }
        else
          overwrite=true;

        if (overwrite)
        {
          if (csmes.openSection(mod_cstr.constData(),"",src_cstr.constData(),"","",CSMESFile::MANUAL_VALIDATION,0,CSMESFile::NEW_RW,0))
          {
            csmes.writeLong(nb_index);
            int sz=inst_p->size();
            for (int i=0; i < sz ; ++i )
            {
              if ((*inst_p)[i].getManuallyValidated())
                csmes.writeLong(i);
            }
            csmes.closeSection();
          }
        }
      }
      else
      {
        if (csmes.sectionExists(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::MANUAL_VALIDATION))
        {
          csmes.deleteSection(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::MANUAL_VALIDATION);
        }
      }
    }
  }

  printStatus(QObject::tr("Saving comments..."),0.75);
  /* Comments sections */
  modules_comments_t::const_iterator mod_com_it;
  for (mod_com_it= comments.begin(); mod_com_it != comments.end(); ++mod_com_it )
  {
    sources_comments_t::const_iterator src_it;
    QString mod=mod_com_it.key();
    QByteArray mod_cstr=mod.toAscii();
    for (src_it= (*mod_com_it).begin(); src_it != (*mod_com_it).end(); ++src_it )
    {
      QString src=src_it.key();
      QByteArray src_cstr=src.toAscii();
      const comments_t *comm_p=&(src_it.value());
      comments_t::const_iterator it;

      long nb_index=comm_p->size();
      if (nb_index>0)
      {
        bool overwrite=false;
        if (csmes.sectionExists(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::COMMENT))
        {
          if (csmes.openSection(mod_cstr.constData(),"",src_cstr.constData(),"","",CSMESFile::COMMENT,0,CSMESFile::RO,0))
          {
            long nb_index_rec=0;
            csmes.readLong(nb_index_rec);
            if (nb_index!=nb_index_rec)
              overwrite=true;
            else
            {
              for (int i=0;i<nb_index_rec;i++)
              {
                char *text;
                long id;
                csmes.readComment(id,&text);
                QString comment=QString::fromUtf8(text);
                free(text);
                if ((*comm_p)[id] != comment)
                  overwrite=true;
              }
            }
            csmes.closeSection();
          }
        }
        else
          overwrite=true;

        if (overwrite)
        {
          if (csmes.openSection(mod_cstr.constData(),"",src_cstr.constData(),"","",CSMESFile::COMMENT,0,CSMESFile::NEW_RW,0))
          {
            csmes.writeLong(nb_index);
            comments_t::const_iterator it;
            for (it=(*comm_p).begin();it != (*comm_p).end();++it)
            {
              QString data=it.value();
              QByteArray data_cstr=it.value().toUtf8();
              csmes.writeComment(it.key(),data_cstr.constData());
            }
            csmes.closeSection();
          }
        }
      }
      else
      {
        if (csmes.sectionExists(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::COMMENT))
        {
          csmes.deleteSection(mod_cstr.constData(),src_cstr.constData(),"",CSMESFile::COMMENT);
        }
      }
    }
  }
  return true;
}

bool CSMesIO::saveCSMes()
{
  QTime timeWatch;
  timeWatch.restart();
  bool was_open=csmes.isOpen();
  if (was_open)
    csmes.close();

  QByteArray filename_cstr=filename.toAscii();
  if (!csmes.open(filename_cstr.constData(),CSMESFile::APPEND_RW))
  {
    err=QObject::tr("Could not open")+" '"+filename+"':"+QString::fromStdString(csmes.getLastErrorMsg());
    return false;
  }

  flushCSMes();

  csmes.close();
  if (was_open)
    csmes.open(filename_cstr.constData(),CSMESFile::RO);
  double durationTime=static_cast<double>(timeWatch.elapsed())/1000.0;
  printStatus(QObject::tr("File '%1' saved. (%2s)").arg(filename).arg(QString::number(durationTime,'f',3)),-1.0);
  resetModificationFlag();
  return true;
}

bool CSMesIO::saveBlackBoxConfiguration(const QString &s)
{
  return csmes.save_blackbox(s.toAscii());
}

bool CSMesIO::isCoverageBranchOnly()
{
  return csmes.isCoverageBranchOnly();
}

bool CSMesIO::isBlackBoxConfiguration() const
{
  return csmes.isBlackBox();
}

bool CSMesIO::isSourceExisting(ModuleFile module,SourceFile source, CSMESFile::type_t t) const
{
  if (!findSourceModule(module,source))
    return false;
  QByteArray mod= module.toAscii();
  QByteArray src= source.toAscii();
  CSMESFileThread *csmes_const_p=const_cast<CSMESFileThread*>(&csmes);
  return csmes_const_p->sectionExists(mod.constData(),src.constData(),"",t);
}

void CSMesIO::read_source(const QString &module,const QString &source, CSMESFile::type_t t,QString &qbuf) const
{
  QByteArray mod= module.toAscii();
  QByteArray src= source.toAscii();
  CSMESFileThread *csmes_const_p=const_cast<CSMESFileThread*>(&csmes);
  if (csmes_const_p->openSection(mod.constData(),NULL,src.constData(),NULL,"",t,0,CSMESFile::RO,0))
  {
    int size = csmes.size();
    char *buf=(char*)malloc(size+2);
    csmes_const_p->readCharTab(buf,size);
    csmes_const_p->closeSection();
    if (size>=0 && buf[size-1]=='\n')
      buf[size]='\0';
    else
      buf[size]='\n';

    buf[size+1]='\0';
    qbuf=QString::fromUtf8(buf);
    free(buf);
  }
  else
    qbuf.clear();
}

unsigned long CSMesIO::checksum(const ModuleFile &module,const SourceFile &source) const
{
   QByteArray mod= module.toAscii();
   QByteArray src= source.toAscii();
   int sec_id=csmes.findSectionID(mod,src,NULL,CSMESFile::ORIGINAL);
   if (sec_id<0)
      return 0;
   else
      return csmes.checksum(sec_id);
}

