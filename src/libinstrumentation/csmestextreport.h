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

#ifndef CSMES_TEXT_REPORT_H
#define CSMES_TEXT_REPORT_H
#include "csmesreport.h"
#include <QVector>
#include <QVariant>

class CSMesTextReport : public CSMesReport
{
  protected:
    CSMesTextReport();
  protected:
    QString exportTextReport(const QString &filename,const QString & format_executed,const QString & format_unexecuted,int coverage_level,Instrumentation::coverage_method_t method) const;
} ;
#endif
