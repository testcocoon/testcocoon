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

#ifndef GLOBAL_COMMON_H
#define GLOBAL_COMMON_H

#ifdef NO_DEBUG
#define ASSERT(X)
#define ABORT() fflush(NULL),exit(-1)
#else
#include <assert.h>
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(X) assert(X)
#define ABORT() fflush(NULL),ASSERT(0)
#endif

#if !defined(OS_WIN32) && !defined(OS_LINUX) && !defined(OS_MACX)
#error "No platform define set"
#endif
#if defined(OS_WIN32)
#pragma warning (disable: 4996)
#endif
#if _MSC_VER == 1200
// VC6.0
#define _I64 __int64
#define _U64 unsigned __int64
#else
#ifndef _I64
#define _I64 long long
#endif
#ifndef _U64
#define _U64 unsigned long long
#endif
#endif 

#define HAVE_REALPATH 1
#if defined(OS_LINUX) || defined(OS_MACX)

#define FILE_SEPARATOR_CHAR '/'/*!< file separator */
#define FILE_SEPARATOR_CHAR_BIS '/'/*!< file separator */
#define FILE_SEPARATOR_UNIX 1 /* Unix file separator */
#define PATH_SEPARATOR_CHAR ':' /* path separator (for PATH variable) */
#define HAVE_REGCOMP 1
#define HAVE_SYMLINK 1

#elif defined(OS_WIN32)
#pragma warning(disable:4996) 
#define FILE_SEPARATOR_CHAR '\\'/*!< file separator */
#define FILE_SEPARATOR_CHAR_BIS '/'/*!< file separator */
#define FILE_SEPARATOR_UNIX 0 /* Unix file separator */
#define PATH_SEPARATOR_CHAR ';' /* path separator (for PATH variable) */
#define HAVE_REGCOMP 0
#define HAVE_SYMLINK 0
#ifdef __cplusplus
extern "C" { 
#endif
void __debugbreak(void);
#ifdef __cplusplus 
}
#endif
#define strcasecmp stricmp
#endif

#define CHAINE_LEN 1024
#ifndef FALSE
#define FALSE (0==1)
#endif
#ifndef TRUE
#define TRUE (0==0)
#endif
#define OPEN_OFFICE 0
#endif
