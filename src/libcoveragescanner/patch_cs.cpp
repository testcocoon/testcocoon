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
#include "patch_cs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filename.h"
#include "system.h"


PatchCs::PatchCs(const char *_f,long _line_rel,long _line,long _col,long _depth,const instrumentation_option_t &_option,long _offset,const char *_table_name,PatchPriority p) : Patch(_f,_line_rel,_line,_col,_depth,_option,_offset,_table_name,p) 
  {
  }

void PatchCs::indent_code_cs(long column_abs,std::string &patch_code) const
{
  int i;
  for (i=0;i<column_abs-1;i++)
    patch_code+=' ';
}

void PatchCs::start_hide_code_cs(std::string &patch_code) const
{
  patch_code+="\n#line hidden\n";
}

void PatchCs::end_hide_code_cs(long line_rel,long column_abs,std::string &patch_code) const
{
  patch_code+="\n#line "+LongToString(line_rel)+" \""+std::string(file())+"\"\n";
  indent_code_cs(column_abs,patch_code);
}

void PatchCs::hide_code_cs(long line_rel,long column_abs,std::string &patch_code) const
{
  if (!patch_code.empty())
  {
    std::string patch;
    start_hide_code_cs(patch);
    patch+=patch_code;
    end_hide_code_cs(line_rel,column_abs,patch);
    patch_code=patch;
  }
}


std::string PatchCs::code(const std::string &table_name) const
{
  std::string patch_code=_code(table_name);

  hide_code_cs(line_rel(),column(),patch_code);
  return patch_code;
}
