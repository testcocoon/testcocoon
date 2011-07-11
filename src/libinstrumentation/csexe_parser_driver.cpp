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

CSExeParserDriver::CSExeParserDriver (CSMesIO &c,const CSExeParser &p) : _csexe_parser(p),_csmes(c)
{
}

CSExeParserDriver::~CSExeParserDriver ()
{
}

bool CSExeParserDriver::parse (const QString &f)
{
  _errors.clear();
  _skip_module=false;
  _mes_p=NULL;
  _execution_title_file.clear();
  file = f.toStdString();
  yy::CSExeParser parser (*this);
  //parser.set_debug_level (trace_parsing);
  parser.parse ();

  return _errors.isEmpty();
}

void CSExeParserDriver::error (const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
  _errors += QString::fromStdString(m);
}

void CSExeParserDriver::error (const std::string& m)
{
  std::cerr << m << std::endl;
  _errors += QString::fromStdString(m);
}

void CSExeParserDriver::begin_csexe_measurement()
{
  _execution_title_file.clear();
  _execution_status= Executions::EXECUTION_STATUS_UNKNOWN;
  _wrong_executions=false;
  _csmes.createEmptyExecution(_mts);
}

void CSExeParserDriver::end_csexe_measurement()
{
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

void  CSExeParserDriver::init_add_instrumentation(int line_nr,const QString &module,long nb_mes, unsigned long signature)
{
  _skip_module=false;
  _mes_p=NULL;
  _mes_p_index=0;

  if (!_csmes.moduleExists(module))
  {
    _skip_module=true;
  }
  else
  {
    if (_csmes.instrumentations.modules[module].signature!=signature)
      _wrong_executions=true;
    else if (nb_mes!=_csmes.nbInstrumentations(module))
      _wrong_executions=true;
  }

  if (!_skip_module)
  {
    _mts.execution_status=_execution_status;
    _mes_p=&(_mts.executions[module]);
    int mes_p_size=_mes_p->size();
    if (mes_p_size!=nb_mes)
    {
      QString err=QObject::tr("Invalid file format")
        +" (" +QObject::tr("Line ")+QString::number(line_nr) +":"+QObject::tr("Wrong instrumentation size")+")";
      _errors+=err;
      _mes_p=NULL;
    }
  }
}

void CSExeParserDriver::add_instrumentation(int line_nr, Instrumentation::execution_state_t instrumentation_item)
{
  if (_mes_p)
  {
    if (_mes_p_index>static_cast<unsigned int>(_mes_p->size()))
    {
      QString err=QObject::tr("Invalid number of executions (too many instrumentation per file)")
        +" (" +QObject::tr("Line ")+QString::number(line_nr) +")";
      _errors+=err;
      _mes_p=NULL;
      return;
    }
    (*_mes_p)[_mes_p_index]=Instrumentation::combineExecution( (*_mes_p)[_mes_p_index] , instrumentation_item ) ;
    _mes_p_index++;
  }
}

void  CSExeParserDriver::endup_add_instrumentation(int line_nr)
{
  if (_mes_p)
  {
    if (_mes_p_index!=static_cast<unsigned int>(_mes_p->size()))
    {
      QString err=QObject::tr("Invalid file format")
        +" (" +QObject::tr("Line ")+QString::number(line_nr) +"):"+QObject::tr("Wrong instrumentation size");
      _errors+=err;
      _mes_p=NULL;
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
}
