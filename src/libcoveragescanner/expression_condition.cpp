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
#include "expression_condition.h"
#include "patch_cpp_full_condition_start.h"
#include "patch_cpp_full_condition_end.h"
#include "patch_cs_full_condition_end.h"
#include "patch_cs_full_condition_start.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"

ExpressionCondition::ExpressionCondition(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,Expression *expression_id1)
{
  clear();
  write(INSTRUMENTATION_CONDITION_FULL, f,options,s_line_absolute, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_absolute, e_line_org, e_column_org, e_line_pre, e_column_pre,depth);
  writePatchArea(0, s_line_absolute, s_column_pre, e_line_absolute, e_column_pre,depth);
  if (expression_id1)
    addChildren(expression_id1);
}

ExpressionCondition::~ExpressionCondition()
{
  FUNCTION_TRACE;
}

std::list<const Patch*> ExpressionCondition::generatePatchCode(code_style_t t,const char *table_name,long index)
{
  std::list<const Patch *> _patches;
  _code_style  = t;
  _index       = index;
  if (getOption().coverage_decision)
  {
    switch(_code_style)
    {
      case CODESTYLE_CPP:
        _patches.push_back(new PatchCppFullConditionStart(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCppFullConditionEnd(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        break;
      case CODESTYLE_CS:
        _patches.push_back(new PatchCsFullConditionStart(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCsFullConditionEnd(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        break;
    }
  }
  return _patches;
}

#if LOG
void ExpressionCondition::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG1("ExpressionCondition");
  Expression::PDEBUG();
}
#endif

long ExpressionCondition::indexes() const
{
  FUNCTION_TRACE;
  if (getOption().coverage_decision)
    return 2;
  else
    return 0;
}

CSMESFile::instrumentation_t ExpressionCondition::instrumentationType() const
{
  FUNCTION_TRACE;
  return CSMESFile::_INSTRUMENTATION_CONDITION_FULL;
}

