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

#ifndef __f_FILEINFO_H
#define __f_FILEINFO_H
#if defined(OS_LINUX) || defined(OS_MACX)
#include <sys/param.h>
#define MAX_PATH MAXPATHLEN
#elif defined(OS_WIN32)
#include <windows.h>
#else
#error "No platform defined"
#endif
#include "libcsmespdef.h"


 LIBCSMES_API void  extractPath(const char *file,char *path,char *name) ;
 LIBCSMES_API char* realPath(const char *path,char resolved_path[MAX_PATH]);
 LIBCSMES_API void cannonicalFileName(const char *input,char *output) ;
 LIBCSMES_API bool realLink(const char *path,char resolved_path[MAX_PATH]);


#endif
