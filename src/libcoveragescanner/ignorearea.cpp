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
#include "ignorearea.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"


IgnoreArea::IgnoreArea(long s_line_absolute,  long s_column_pre, long e_line_absolute, long e_column_pre)
{
  _s_line_absolute = s_line_absolute;
  _s_column        = s_column_pre;
  _e_line_absolute = e_line_absolute;
  _e_column        = e_column_pre;
}

#if LOG
void IgnoreArea::PDEBUG() const
{
  DEBUG5(" (%li,%li)->(%li,%li)",_s_line_absolute,_s_column,_e_line_absolute,_e_column);
}
#endif

bool IgnoreArea::isInIgnoreArea(const Expression &e) const
{
  FUNCTION_TRACE;
  return 
    ( 
     ( _s_line_absolute<e.startLineAbsolute() 
       || 
       ( _s_line_absolute==e.startLineAbsolute() && _s_column<=e.startColumnPre() )
     )
     && 
     ( _e_line_absolute>e.endLineAbsolute() 
       || 
       ( _e_line_absolute==e.endLineAbsolute() && _e_column>=e.endColumnPre() )
     )
    );
}


