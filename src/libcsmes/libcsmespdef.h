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

#ifndef _PDEF_H_LIBCSMES
#define _PDEF_H_LIBCSMES
#define EVAL_MAX_INSTRUMENTATION 500
#include "../commoncode/commonglobal.h"

#define INSTRUMENTATION_TABLE_STR "__cs_tb_"
#define INSTRUMENTATION_CODE_MAX_LENGTH 64

#if defined(OS_WIN32)
#  if defined(TESTCOCOON_SHARED) // DLL
#    if defined(TESTCOCOON_SHARED_CSMES) // inside DLL
#      define LIBCSMES_API   __declspec(dllexport)
#    else // outside DLL
#      define LIBCSMES_API   __declspec(dllimport)
#    endif 
#  else
#    define LIBCSMES_API 
#  endif
#else
#  define LIBCSMES_API 
#endif

#include "debug.h"
#endif
