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
#include "patch.h"

StringPool Patch::filenames_pool;

Patch::Patch(const char *__f,long __line_rel,long __line,long __col,long __depth,const instrumentation_option_t &__option,long __offset,const char *__table_name,PatchPriority __priority) :_offset(__offset),_option(__option),_table_name(__table_name),_line(__line),_column(__col),_depth(__depth),_line_rel(__line_rel),_priority(__priority)
  {
    _file = filenames_pool.add(__f);
  }
std::string Patch::patchCode() const
{
  return code(_table_name);
}

#if LOG
void Patch::PDEBUG() const
{
  DEBUG5("PatchCode: %s line=%i column=%i depth=%i",name().c_str(),line(),column(),depth());
}
#endif

