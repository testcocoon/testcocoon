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

#include "coveragescannerpdef.h"
#include "patch_cs_full_ifthenelse_start1.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filename.h"
#include "system.h"


std::string PatchCsFullIfThenElseStart1::_code(const std::string &table_name) const
{
  FUNCTION_TRACE;
  bool coverage_count=option().coverage_count!=0;
  if (coverage_count)
    return  " {  "+table_name+".val["+LongToString(offset())+"]++; ";
  else
    return  " {  "+table_name+".val["+LongToString(offset())+"]=1; ";
}
