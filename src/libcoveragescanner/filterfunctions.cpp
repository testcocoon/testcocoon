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

#include "filterfunctions.h"
#include <string.h>
#include <stdlib.h>
#include "regexp_custom.h"
#include "fileinfo.h"
#if defined(OS_WIN32)
#include <windows.h>
#include <process.h>
#endif


std::string FilterFunctions::appendDefault(bool ignore_flag,type_t type,const char *expression)
{
  FUNCTION_TRACE;
  return Filter::appendDefault(ignore_flag,static_cast<Filter::type_t>(type),expression);
}

std::string FilterFunctions::append(bool ignore_flag,type_t type,const char *expression)
{
  FUNCTION_TRACE;
  return Filter::append(ignore_flag,static_cast<Filter::type_t>(type),expression);
}


