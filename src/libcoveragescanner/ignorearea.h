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

#ifndef IGNORE_AREA_H
#define IGNORE_AREA_H
#include "coveragescannerpdef.h"
#include "csmesfile.h"
#include "patch.h"
#include "expression.h"
#include <string>
#include <list>

class IgnoreArea
{
public:
    IgnoreArea(long s_line_absolute, long s_column, long e_line_absolute, long e_column_org);
    bool isInIgnoreArea(const Expression &) const;

#if LOG
    void PDEBUG() const ;
#endif
private:
    long _s_line_absolute;
    long _s_column; 
    long _e_line_absolute;
    long _e_column;
    void operator=(const IgnoreArea &);
    IgnoreArea(const IgnoreArea &);
};

#endif
