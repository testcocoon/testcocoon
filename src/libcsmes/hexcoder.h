/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ** This file and the library libcsmes can be integrated in any kind
 ** of products without any licensing restrictions.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#ifndef HEXCODER_H
#define HEXCODER_H

#include "libcsmespdef.h"

LIBCSMES_API int hexencode(const char *source,int source_lg,char **destination);
LIBCSMES_API int hexdecode(const char *source,char **destination,int *destination_lg);

#endif
