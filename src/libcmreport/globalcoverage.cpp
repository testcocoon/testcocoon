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

#include "globalcoverage.h"

GlobalCoverage::GlobalCoverage(const CSMes &m):_csmes(m)
{
  _coverage_level=1;
  _method=Instrumentation::COVERAGE_CONDITION;
}


QString GlobalCoverage::statistic()
{
  int nb_tested,nb_untested;
  if (! _csmes.statistic(_coverage_level,_method,nb_tested,nb_untested))
    return "--";
  return CSMes::printStat(nb_tested,nb_untested);
}
