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

#ifndef PARSEMISC_H
#define PARSEMISC_H
#include "coveragescannerpdef.h"
#include "compilerinterface.h"

void extract_string(char *s,long s_lg,const long *line_index, long nb_line_index,long start_line,long start_column, long end_line, long end_column,char **str_p);
void generate_line_index(const char *s,long s_lg,long **line_index,long *nb_line_index);
CompilerInterface::extension_t language_to_parse();

#endif
