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

#include "ignoreareas.h"
#include "coveragescannerpdef.h"
#include <stdlib.h>
#include <algorithm>

IgnoreAreas::IgnoreAreas()
{
  FUNCTION_TRACE;
  areas.clear();
}

IgnoreAreas::~IgnoreAreas()
{
  FUNCTION_TRACE;
  clear();
}

void IgnoreAreas::clear()
{
  FUNCTION_TRACE;
  for (std::list<IgnoreArea*>::iterator it=areas.begin();it!=areas.end();++it)
	delete *it;
  areas.clear();
}

bool IgnoreAreas::append(long start_line,long start_column, long end_line, long end_column )
{
  FUNCTION_TRACE;
  areas.push_back(new IgnoreArea(start_line,start_column, end_line, end_column ));
  return true;
}

#if LOG
void IgnoreAreas::PDEBUG() const
{
  FUNCTION_TRACE;
  for (std::list<IgnoreArea*>::const_iterator it=areas.begin();it!=areas.end();++it)
  {
	(*it)->PDEBUG();
  }
}
#endif

bool IgnoreAreas::isInIgnoreArea(const Expression &e) const
{
  for (std::list<IgnoreArea*>::const_iterator it=areas.begin();it!=areas.end();++it)
  {
    if ((*it)->isInIgnoreArea(e))
      return true;
  }
  return false;
}



