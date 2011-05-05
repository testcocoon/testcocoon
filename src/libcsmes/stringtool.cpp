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

#include "../commoncode/commonglobal.h"
#include "stringtool.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


std::string  string_printf( const char* format, ... )
{
  char tmp[CHAINE_LEN];
  va_list arg;

  va_start( arg, format );     /* Initialize variable arguments. */
  int ret=vsnprintf(tmp,sizeof(tmp),format,arg);
  va_end( arg );              /* Reset variable arguments.      */
  return std::string(tmp,ret);
}

