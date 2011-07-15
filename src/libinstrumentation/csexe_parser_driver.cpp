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

#include "csexe_parser_driver.h"
#include "csexe_parser.h"
#include <QTime>
#include "Service.h"
#include <QTextDocument>

CSExeParserDriver::CSExeParserDriver (CSMesIO &c,const CSExeParser &p) : _csexe_parser(p),_csmes(c)
{
}

CSExeParserDriver::~CSExeParserDriver ()
{
}

bool CSExeParserDriver::parse (const QString &f,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
{
  QTime timeWatch;
  timeWatch.restart();
  _line=1;
  _flushpos=0;
  _execution_status= Executions::EXECUTION_STATUS_UNKNOWN;
  _new_executions.clear();
  _progress_p=progress_p;
  _undo_backup_p=undo_backup_p;
  _errmsg.clear();
  _info.clear();
  _errmsgs.clear();
  _short_status.clear();
  _skip_module=false;
  _wrong_executions=false;
  _execution_title_file.clear();
  _execution_title.clear();
  _mes_p=NULL;
  _nb_mes_invalid=0;
  _nb_mes_duplicates=0;
  _mes_p_index=0;
  _execution_nr=0;
  _execution_title_file.clear();
  _mes_new.clear();
  _mes_modif.clear();
  _detailled_info.clear();
  _max_file_size=CSExeParser::qiodevice_p->size();

  file = f.toStdString();
  yy::CSExeParser parser (*this);
  //parser.set_debug_level (trace_parsing);


  _detailled_info="<TABLE cellpadding=\"3\" cellspacing=\"0\" border=\"1\">";
  _detailled_info+="<TR>";
  _detailled_info+="<TD><B>#</TD>";
  _detailled_info+="<TD><B>"+QObject::tr("Name")+"</B></TD>";
  _detailled_info+="<TD><B>"+QObject::tr("Status")+"</B></TD>";
  _detailled_info+="<TD><B>"+QObject::tr("Description")+"</B></TD>";
  _detailled_info+="</TR>";
  parser.parse ();
  _detailled_info+="</TABLE>";

  _info.clear();

  ExecutionNames::Iterator it;
  if ( (!_mes_new.isEmpty()) || (!_mes_modif.isEmpty()))
    _csmes.setModificationFlag();

  _info+="<HTML><BODY>";
  if (!_mes_new.isEmpty())
  {
    _info+=QObject::tr("New executions added:")+"<BR>";
    _info+="<UL>";
    for ( it = _mes_new.begin(); it != _mes_new.end(); ++it )
      _info+="<LI><TT>"+Qt::escape(*it)+"</TT></LI>";
    _info+="</UL>";
    _info+="<BR>";
  }

  if (!_mes_modif.isEmpty())
  {
    _info+=QObject::tr("Modified executions:")+"<BR>";
    _info+="<UL>";
    for ( it = _mes_modif.begin(); it != _mes_modif.end(); ++it )
      _info+="<LI><TT>"+Qt::escape(*it)+"</TT></LI>";
    _info+="</UL>";
    _info+="<BR>";
  }

  if (_nb_mes_invalid>0)
  {
    _info+="<HR>";
    _info+=QObject::tr("Number of invalid executions (not loaded):")+QString::number(_nb_mes_invalid)+"<BR>";
    _info+="<BR>";
  }

  if (_nb_mes_duplicates>0)
  {
    _info+="<HR>";
    _info+=QObject::tr("Duplicate executions (not loaded):")+QString::number(_nb_mes_duplicates);
    _info+="<BR>";
  }
  _info+="<P>";
  _info+=QObject::tr("Detailed information:");
  _info+="<BR>";
  _info+=_detailled_info;
  _info+="</P>";
  _info+="</HTML></BODY>";

  _new_executions=_mes_new;
  _new_executions+=_mes_modif;

  _short_status.clear();
  if (_mes_new.count()>0)
    _short_status += QObject::tr("%1 new executions").arg(_mes_new.count());
  if (_mes_modif.count()>0)
  {
    if (!_short_status.isEmpty()) _short_status+=", ";
    _short_status += QObject::tr("%1 merged executions").arg(_mes_modif.count());
  }
  if (_nb_mes_duplicates>0)
  {
    if (!_short_status.isEmpty()) _short_status+=", ";
    _short_status += QObject::tr("%1 duplicates (not loaded)").arg(_nb_mes_duplicates);
  }
  if (_nb_mes_invalid)
  {
    if (!_short_status.isEmpty()) _short_status+=", ";
    _short_status += QObject::tr("%1 invalid executions (not loaded)").arg(_nb_mes_invalid);
  }

  double loadTime=static_cast<double>(timeWatch.elapsed())/1000.0;
  bool result=_errmsgs.isEmpty();
  if (result)
    printStatus(QObject::tr("Execution report loaded. (%1s)").arg(QString::number(loadTime,'f',3)),-1.0);
  else
    printStatus(QObject::tr("Importing execution report fails."),-1.0);

  if (_progress_p)
    _progress_p(1.0,true);

  new_executions=_new_executions;
  info=_info;
  short_status=_short_status;
  errmsgs=_errmsgs;

  return result;
}

void CSExeParserDriver::error (const yy::location& l, const std::string& m)
{
  if (l.begin.filename)
    _errmsg += QString::fromStdString(*l.begin.filename) + ":" ;
  _errmsg += QString::number(l.begin.line) + "." ;
  _errmsg += QString::number(l.begin.column) + ":" ;
  _errmsg += QString::fromStdString(m);
}

void CSExeParserDriver::error (const std::string& m)
{
  std::cerr << m << std::endl;
  _errmsg = QString::fromStdString(m);
}

bool CSExeParserDriver::begin_csexe_measurement()
{
  if (_progress_p)
  {
    if (_progress_p(((float)CSExeParser::qiodevice_p->pos())/_max_file_size,false))
      return false; // Interrupt
  }
  _errmsg.clear();
  _execution_status= Executions::EXECUTION_STATUS_UNKNOWN;
  _wrong_executions=false;
  _mts.clear();
  _csmes.createEmptyExecution(_mts);
  _execution_nr++;
  _detailled_info+="<TR>";
  _detailled_info+="<TD>"+QString::number(_execution_nr)+"</TD>";
  return true;
}

void CSExeParserDriver::end_csexe_measurement()
{
  if (_wrong_executions)
  {
    _detailled_info+="<TD></TD>";
    _detailled_info+="<TD></TD>";
    _detailled_info+="<TD>"+QObject::tr("Invalid Execution")+ "(" + Qt::escape(_errmsg) + ")" +"</TD>";
    if (!_errmsgs.isEmpty())
      _errmsgs+="\n";
    _errmsgs+=_errmsg;
    _nb_mes_invalid++;
  }
  else
  {
    _detailled_info+="<TD><TT>"+Qt::escape(_execution_title)+"</TT></TD>";
    _detailled_info+="<TD bgcolor=\""+CSMesIO::executionStatusColor(_mts.execution_status).name()+"\">"+CSMesIO::executionsStatusStr()[_mts.execution_status]+"</TD>";
    bool duplicate=false;
    ExecutionName duplicate_execution;
    bool empty_execution=false;
    if (_csexe_parser.policy()!=CSMesIO::CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY)
      empty_execution=_csmes.emptyExecution(_mts);
    if (_csexe_parser.policy()==CSMesIO::CSEXE_POLICY_IGNORE_DUPLICATES && !empty_execution)
    {
      duplicate_execution=_csmes.duplicateExecution(_mts,ExecutionNames());
      duplicate = ! duplicate_execution.isEmpty();
    }

    if (empty_execution)
      ; // do nothing
    else if (duplicate)
      _nb_mes_duplicates++;
    else if (_csmes.executions.exists(_execution_title))
    {
      if ( ! (_mes_modif.contains(_execution_title) || _mes_new.contains(_execution_title)) )
        _mes_modif+=_execution_title;
    }
    else
      _mes_new+=_execution_title;

    switch (_csexe_parser.policy())
    {
      case CSMesIO::CSEXE_POLICY_INVALID:
        ASSERT(false);
        break;
      case CSMesIO::CSEXE_POLICY_MERGE_ALL_TOGETHER:
        if (empty_execution)
          _detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
        else
        {
          _detailled_info+="<TD>"+QObject::tr("merged with the existing execution.")+"</TD>";
          if (_undo_backup_p)
          {
            if (!_undo_backup_p->contains(_execution_title))
            {
              Executions::modules_executions_private_t exec;
              bool ret = _csmes.backupExecution(_execution_title,exec);
              if (ret)
                (*_undo_backup_p)[_execution_title]=exec;
            }
          }
          _csmes.mergeInExecution(_execution_title,_mts);
        }
        break;
      case CSMesIO::CSEXE_POLICY_IMPORT_DUPLICATES_AND_EMPTY:
        _detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
        if (_undo_backup_p)
        {
          if (!_undo_backup_p->contains(_execution_title))
          {
            Executions::modules_executions_private_t exec;
            bool ret = _csmes.backupExecution(_execution_title,exec);
            if (ret)
              (*_undo_backup_p)[_execution_title]=exec;
          }
        }
        _csmes.executions.setExecution(_execution_title,_mts);
        break;
      case CSMesIO::CSEXE_POLICY_IMPORT_DUPLICATES:
        if (empty_execution)
          _detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
        else if (duplicate)
          _detailled_info+="<TD>"+QObject::tr("not imported: same as '%1'.").arg("<TT>"+Qt::escape(duplicate_execution)+"</TT>")+"</TD>";
        else
        {
          _detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
          if (_undo_backup_p)
          {
            if (!_undo_backup_p->contains(_execution_title))
            {
              Executions::modules_executions_private_t exec;
              bool ret = _csmes.backupExecution(_execution_title,exec);
              if (ret)
                (*_undo_backup_p)[_execution_title]=exec;
            }
          }
          _csmes.executions.setExecution(_execution_title,_mts);
        }
        break;
      case CSMesIO::CSEXE_POLICY_IGNORE_DUPLICATES:
        if (empty_execution)
          _detailled_info+="<TD>"+QObject::tr("not imported: empty execution.")+"</TD>";
        else if (duplicate)
          _detailled_info+="<TD>"+QObject::tr("not imported: same as '%1'.").arg("<TT>"+Qt::escape(duplicate_execution)+"</TT>")+"</TD>";
        else
        {
          _detailled_info+="<TD>"+QObject::tr("imported")+"</TD>";
          if (_undo_backup_p)
          {
            if (!_undo_backup_p->contains(_execution_title))
            {
              Executions::modules_executions_private_t exec;
              bool ret = _csmes.backupExecution(_execution_title,exec);
              if (ret)
                (*_undo_backup_p)[_execution_title]=exec;
            }
          }
          _csmes.executions.setExecution(_execution_title,_mts);
        }
        break;
    }
  }
  _detailled_info+="</TR>";

  if (_undo_backup_p==NULL)
  {
    qint64 pos=CSExeParser::qiodevice_p->pos();
    static const qint64 flush_size=100*1024*1024;
    if (pos-_flushpos>flush_size)
    {
      _csmes.flushCSMes(); // Write executions to minimize memory usage
      _flushpos=pos;
    }
  }
}

ExecutionName CSExeParserDriver::executionName(const ExecutionName &default_name,const ExecutionName &execution_name,CSMesIO::csexe_import_policy_t policy) 
{
  return _csmes.executionName(default_name,execution_name,policy) ;
}

void CSExeParserDriver::set_status(Executions::execution_status_t s) 
{
  switch (_mts.execution_status)
  {
    case Executions::EXECUTION_STATUS_UNKNOWN:
      _mts.execution_status=s;
      break;
    case Executions::EXECUTION_STATUS_FAILED:
    case Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY:
    case Executions::EXECUTION_STATUS_PASSED:
      if (_mts.execution_status!=s)
        _mts.execution_status=Executions::EXECUTION_STATUS_TO_BE_CHECK_MANUALLY;
      break;
  }
}

void  CSExeParserDriver::init_add_instrumentation(const QString &module,long nb_mes, unsigned long signature)
{
  _skip_module=false;
  _mes_p=NULL;
  _mes_p_index=0;
  if (_wrong_executions)
    return ;

  if (!_csmes.moduleExists(module))
  {
    _skip_module=true;
  }
  else
  {
    if (_csmes.instrumentations.modules[module].signature!=signature)
      mark_execution_as_wrong(QObject::tr("Invalid signature"));
    else if (nb_mes!=_csmes.nbInstrumentations(module))
      mark_execution_as_wrong(QObject::tr("Invalid number of instrumentations"));
  }

  if (!_skip_module)
  {
    _mts.execution_status=_execution_status;
    _mes_p=&(_mts.executions[module]);
    int mes_p_size=_mes_p->size();
    if (mes_p_size!=nb_mes)
    {
      mark_execution_as_wrong(QObject::tr("Invalid file format")
        +" (" +QObject::tr("Line ")+QString::number(line()) +":"+QObject::tr("Wrong instrumentation size")+")");
      _mes_p=NULL;
    }
  }
}


void  CSExeParserDriver::endup_add_instrumentation()
{
  if (_mes_p)
  {
    if (_mes_p_index!=static_cast<unsigned int>(_mes_p->size()))
    {
      _mes_p=NULL;
      mark_execution_as_wrong(QObject::tr("Invalid file format")
        +" (" +QObject::tr("Line ")+QString::number(line()) +"):"+QObject::tr("Wrong instrumentation size"));
      return;
    }
  }
}

void CSExeParserDriver::set_title(const QString &s)
{
  _execution_title_file=s;
}


void CSExeParserDriver::begin_measurement()
{
  _execution_title=_csmes.executionName(_csexe_parser.defaultTitle(),_execution_title_file,_csexe_parser.policy());
  _execution_title_file.clear();
}
