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
#include "patch_cpp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filename.h"
#include "system.h"

PatchCpp::PatchCpp(const char *_f,long _line_rel,long _line,long _col,long _depth,const instrumentation_option_t &_option,long _offset,const char *_table_name,PatchPriority p ) : Patch(_f,_line_rel,_line,_col,_depth,_option,_offset,_table_name,p) 
{
}

std::string PatchCpp::correctEol(const std::string &str)
{
  std::string ret;
  ret.reserve(str.length());
  for (std::string::const_iterator i=str.begin();i!=str.end();++i)
  {
    char c=*i;
    switch (c)
    {
      default:
        ret+=c;
        break;
      case '\r':
        break;
      case '\n':
        ret+=' ';
        break;
    }
  }
  return ret;
}

