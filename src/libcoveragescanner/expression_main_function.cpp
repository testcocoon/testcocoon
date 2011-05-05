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
#include "expression_main_function.h"
#include "patch_cs_main_start.h"
#include "patch_cs_main_end.h"
#include "patch_cs_main_body_start.h"
#include "patch_cs_main_body_end.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"

ExpressionMainFunction::ExpressionMainFunction(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,
     long s_line_header, long s_column_header,  long e_line_header, long e_column_header,
     long s_line_body, long s_column_body,  long e_line_body, long e_column_body
    )
{
  clear();
  write(INSTRUMENTATION_MAIN_FUNCTION, f,options,s_line_absolute, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_absolute, e_line_org, e_column_org, e_line_pre, e_column_pre,depth);
  writePatchArea(0, s_line_header, s_column_header, e_line_header, e_column_header,depth);
  writePatchArea(1, s_line_body, s_column_body, e_line_body, e_column_body,depth);
}

ExpressionMainFunction::~ExpressionMainFunction()
{
  FUNCTION_TRACE;
}

std::list<const Patch*> ExpressionMainFunction::generatePatchCode(code_style_t t,const char *table_name,long index)
{
  std::list<const Patch *> _patches;
  _code_style  = t;
  _index       = index;
  if (!getOption().coverage_decision)
    return _patches;
  switch(_code_style)
  {
    case CODESTYLE_CPP:
      break;
    case CODESTYLE_CS:
      _patches.push_back(new PatchCsMainStart(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
      _patches.push_back(new PatchCsMainEnd(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
      _patches.push_back(new PatchCsMainBodyStart(file(),startLineOrg(),startLineAbsolutePatch(1),startColumnPatch(1),startDepthPatch(1),getOption(),_index,table_name));
      _patches.push_back(new PatchCsMainBodyEnd(file(),endLineOrg(),endLineAbsolutePatch(1),endColumnPatch(1),endDepthPatch(1),getOption(),_index,table_name));
      break;
  }
  return _patches;
}

#if LOG
void ExpressionMainFunction::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG1("ExpressionMainFunction");
  Expression::PDEBUG();
}
#endif

long ExpressionMainFunction::indexes() const
{
  FUNCTION_TRACE;
  return 0;
}

CSMESFile::instrumentation_t ExpressionMainFunction::instrumentationType() const
{
  FUNCTION_TRACE;
  return CSMESFile::_INSTRUMENTATION_NOP;
}

