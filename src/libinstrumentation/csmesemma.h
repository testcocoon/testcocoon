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

#ifndef CSMES_EMMA_H
#define CSMES_EMMA_H
#include "csmescsv.h"
#include <QXmlStreamWriter>

class CSMesEmma : public CSMesCsv
{
  protected:
    CSMesEmma();
  protected:
    QString exportEMMAStatistic(const QString &filename,int coverage_level,Instrumentation::coverage_method_t method) const;

  private:
    static QString emmaStatistic(int nb_tested,int nb_untested);
    static void emmaStatisticMethods( const QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested_function_list,int &nb_tested,int &nb_untested);
    static void emmaStatisticClasses( const QHash<CSMesFunctionInfo::functionskey_t,int> &nb_tested_function_list,int &nb_tested,int &nb_untested) ;
    static QString classFromFunction(const QString &function) ;
    static void exportEmmaStatistics(QXmlStreamWriter &stream, int nb_tested_line,int nb_untested_line, int nb_tested_method,int nb_untested_method, int nb_tested_class,int nb_untested_class, int nb_tested_block,int nb_untested_block);
    static void exportEmmaStatisticsField(QXmlStreamWriter &stream, const QString &name, int nb_tested,int nb_untested);
} ;
#endif
