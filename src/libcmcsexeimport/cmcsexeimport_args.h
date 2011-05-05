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

#ifndef CMCSEXEIMPORT_ARGS_H
#define CMCSEXEIMPORT_ARGS_H
#include "csmes.h"
#include <QString>

class CMCsExeImport
{
  private:
    CMCsExeImport();
  public:
    static CMCsExeImport &instance() ;
    bool analyse_args(int argc,const char*const * argv);
    bool getopt_process(int argc,const char* const* argv);
    void clear() ;

  private:
    static int getoption_passed(const char *);
    static int getoption_failed(const char *);
    static int getoption_check_manually(const char *);
    static int getoption_csmes_filename(const char *file);
    static int getoption_title(const char *file);
    static int getoption_csexe_filename(const char *file);
    static int getoption_import_policy(const char *p);

    CSMes csmes;
    QString csexe_filename;
    QString csmes_filename;
    QString title;
    CSMes::csexe_import_policy_t  policy;
    Executions::execution_status_t default_execution_status;
};
#endif
