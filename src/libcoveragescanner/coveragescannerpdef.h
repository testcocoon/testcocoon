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

#ifndef PDEF_H_COVERAGESCANNER
#define PDEF_H_COVERAGESCANNER
#include "../commoncode/commonglobal.h"
#include "debug.h"
#define SCOPE_NAME_SIZE 16

#ifdef OS_WIN32
#define ext_strcmp stricmp
#define ext_strncmp strnicmp
#define filename_strcmp stricmp
#define filename_strncmp strnicmp
#define setenv(NAME,VALUE,X) _putenv_s(NAME,VALUE)
#elif defined(OS_LINUX) || defined(OS_MACX)
#define ext_strcmp strcmp
#define ext_strncmp strncmp
#define filename_strcmp strcmp
#define filename_strncmp strncmp
#else
#error "Unsupported platform"
#endif

#define INSTRUMENTATION_TABLE_STR "__cs_tb_"
#define INSTRUMENTATION_CODE_MAX_LENGTH 64
#define MAIN_NAME "__cm"
#endif
