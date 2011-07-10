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

#include "csexe_parser.h"
#include <QIODevice>
#include <QtDebug>

CSExeParser::CSExeParser(CSMesIO &csmes,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status) : _csmes(csmes),
  _title_default(name_orig),
  _status_default(default_execution_status),
  _policy(policy)
{
}

QIODevice *CSExeParser::qiodevice_p=NULL;
int CSExeParser::yy_input_qiodevice(char *buf,int max_size)
{
  int result;
  if ( (result = qiodevice_p->read( (char *) buf, max_size )) < 0 ) 
    return 0;

  return result;
}

                                                                    
