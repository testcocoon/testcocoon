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

#ifndef GLOBAL_COVERAGE_H
#define GLOBAL_COVERAGE_H
#include "cmreportpdef.h"
#include <qstring.h>
#include "csmes.h"

class GlobalCoverage
{
  public:
    GlobalCoverage(const CSMes &);
    void setCoverageLevel(int l) { _coverage_level=l; }
    int getCoverageLevel() const { return _coverage_level; }
    void setCoverageMethod(Instrumentation::coverage_method_t m) {_method=m;}
    QString statistic();

  private:
    int _coverage_level;
    Instrumentation::coverage_method_t _method;
    QString _filename;
    const CSMes &_csmes;
};

#endif
