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

#ifndef PATCH_CS_FULL_WHILE_START2_H
#define PATCH_CS_FULL_WHILE_START2_H
#include "coveragescannerpdef.h"
#include "patch_cs_start.h"

class PatchCsFullWhileStart2 : public PatchCsStart
{
public:
    PatchCsFullWhileStart2(const char *_f,long _line_rel,long _line,long _col,long _depth,const instrumentation_option_t &_option,long _offset,const char *_table_name) : PatchCsStart(_f,_line_rel,_line,_col,_depth,_option,_offset,_table_name) 
    {
    }
    virtual std::string _code(const std::string &table_name) const;
    virtual std::string name() const { return "PatchCsFullWhileStart2"; }

};
#endif
