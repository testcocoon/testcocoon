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


#ifndef IGNORE_AREAS_H
#define IGNORE_AREAS_H
#include "ignorearea.h"
#include "function.h"
#include "expression.h"
#include <list>

class TestIgnoreArea;
class IgnoreAreas
{
public:
    IgnoreAreas();
    virtual ~IgnoreAreas();

    bool append(long start_line,long start_column, long end_line, long end_column );
    void clear();
    bool isInIgnoreArea(const Expression &) const;

#if LOG
    void PDEBUG() const;
#endif
private:
    std::list<IgnoreArea*> areas;
    friend class TestIgnoreArea;
} ;
#endif

