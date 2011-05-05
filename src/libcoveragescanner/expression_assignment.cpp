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
#include "expression_assignment.h"
#include "patch_cpp_computation_start.h"
#include "patch_cpp_computation_end.h"
#include "patch_cs_computation_start.h"
#include "patch_cs_computation_end.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"

ExpressionAssignment::ExpressionAssignment(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,Expression *expression_id1):
ExpressionNOP(f,options,s_line_absolute, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_absolute, e_line_org, e_column_org, e_line_pre, e_column_pre,depth)
{
  clear();
  write(INSTRUMENTATION_ASSIGNMENT, f,options,s_line_absolute, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_absolute, e_line_org, e_column_org, e_line_pre, e_column_pre,depth);
  if (expression_id1)
	addChildren(expression_id1);
}

ExpressionAssignment::~ExpressionAssignment()
{
  FUNCTION_TRACE;
}

#if LOG
void ExpressionAssignment::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG1("ExpressionAssignment");
  Expression::PDEBUG();
}
#endif

