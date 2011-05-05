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

#ifndef CSMES_CSV_H
#define CSMES_CSV_H
#include "csmestextreport.h"
#include <QVector>
#include <QVariant>
class Writer;

class CSMesCsv : public CSMesTextReport
{
  protected:
    CSMesCsv();
  protected:
    QString exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const;
    QString exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep,int coverage_level,Instrumentation::coverage_method_t method) const;
  private:
    QString exportCSVtable(const table_t &table,const QString &filename,QChar separator,QChar floatSep) const;
    static QString CSVstat(int nb_tested,int nb_untested,QChar floatSep);
    static QString CSVEscape(const QString &v);
} ;
#endif
