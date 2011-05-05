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
#include "expression_while.h"
#include "patch_cpp_full_while_start1.h"
#include "patch_cpp_full_while_start2.h"
#include "patch_cpp_full_while_end1.h"
#include "patch_cpp_full_while_end2.h"
#include "patch_cpp_semi_while_start1.h"
#include "patch_cpp_semi_while_start2.h"
#include "patch_cpp_semi_while_end1.h"
#include "patch_cpp_semi_while_end2.h"
#include "patch_cs_full_while_start1.h"
#include "patch_cs_full_while_start2.h"
#include "patch_cs_full_while_end1.h"
#include "patch_cs_full_while_end2.h"
#include "patch_cs_semi_while_start1.h"
#include "patch_cs_semi_while_start2.h"
#include "patch_cs_semi_while_end1.h"
#include "patch_cs_semi_while_end2.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringpool.h"
#include "system.h"
#include <sstream>
#include "expressions.h"
#include "source.h"

ExpressionWhile::ExpressionWhile(const char *f,instrumentation_option_t options,long s_line_absolute, long s_line_org, long s_column_org, long s_line_pre, long s_column_pre, long e_line_absolute, long e_line_org, long e_column_org, long e_line_pre, long e_column_pre,long depth,
     long s_line_patch1, long s_column_patch1,  long e_line_patch1, long e_column_patch1,
     long s_line_patch2, long s_column_patch2,  long e_line_patch2, long e_column_patch2,
     Expression *expression_id1
    )
{
  clear();
  write(INSTRUMENTATION_WHILE, f,options,s_line_absolute, s_line_org, s_column_org, s_line_pre, s_column_pre, e_line_absolute, e_line_org, e_column_org, e_line_pre, e_column_pre,depth);
  writePatchArea(0, s_line_patch1, s_column_patch1, e_line_patch1, e_column_patch1,depth);
  writePatchArea(1, s_line_patch2, s_column_patch2, e_line_patch2, e_column_patch2,depth);
  if (expression_id1)
	addChildren(expression_id1);
}

ExpressionWhile::~ExpressionWhile()
{
  FUNCTION_TRACE;
}

std::list<const Patch*> ExpressionWhile::generatePatchCode(code_style_t t,const char *table_name,long index)
{
  std::list<const Patch *> _patches;
  _code_style  = t;
  _index       = index;
  if (!getOption().coverage_decision)
    return _patches;
  switch(_code_style)
  {
    case CODESTYLE_CPP:
      if (getOption().partial_condition)
      {
        _patches.push_back(new PatchCppSemiWhileStart1(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCppSemiWhileEnd1(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCppSemiWhileStart2(file(),startLineOrg(),startLineAbsolutePatch(1),startColumnPatch(1),startDepthPatch(1),getOption(),_index,table_name));
        _patches.push_back(new PatchCppSemiWhileEnd2(file(),endLineOrg(),endLineAbsolutePatch(1),endColumnPatch(1),endDepthPatch(1),getOption(),_index,table_name));
      }
      else
      {
        _patches.push_back(new PatchCppFullWhileStart1(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCppFullWhileEnd1(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCppFullWhileStart2(file(),startLineOrg(),startLineAbsolutePatch(1),startColumnPatch(1),startDepthPatch(1),getOption(),_index,table_name));
        _patches.push_back(new PatchCppFullWhileEnd2(file(),endLineOrg(),endLineAbsolutePatch(1),endColumnPatch(1),endDepthPatch(1),getOption(),_index,table_name));
      }
      break;
    case CODESTYLE_CS:
      if (getOption().partial_condition)
      {
        _patches.push_back(new PatchCsSemiWhileStart1(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCsSemiWhileEnd1(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCsSemiWhileStart2(file(),startLineOrg(),startLineAbsolutePatch(1),startColumnPatch(1),startDepthPatch(1),getOption(),_index,table_name));
        _patches.push_back(new PatchCsSemiWhileEnd2(file(),endLineOrg(),endLineAbsolutePatch(1),endColumnPatch(1),endDepthPatch(1),getOption(),_index,table_name));
      }
      else
      {
        _patches.push_back(new PatchCsFullWhileStart1(file(),startLineOrg(),startLineAbsolutePatch(0),startColumnPatch(0),startDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCsFullWhileEnd1(file(),endLineOrg(),endLineAbsolutePatch(0),endColumnPatch(0),endDepthPatch(0),getOption(),_index,table_name));
        _patches.push_back(new PatchCsFullWhileStart2(file(),startLineOrg(),startLineAbsolutePatch(1),startColumnPatch(1),startDepthPatch(1),getOption(),_index,table_name));
        _patches.push_back(new PatchCsFullWhileEnd2(file(),endLineOrg(),endLineAbsolutePatch(1),endColumnPatch(1),endDepthPatch(1),getOption(),_index,table_name));
      }
      break;
  }
  return _patches;
}

#if LOG
void ExpressionWhile::PDEBUG() const
{
  FUNCTION_TRACE;
  DEBUG1("ExpressionWhile");
  Expression::PDEBUG();
}
#endif

long ExpressionWhile::indexes() const
{
  FUNCTION_TRACE;
  if (!getOption().coverage_decision)
    return 0;

  if (getOption().partial_condition)
    return 1;
  else
    return 2;
}

CSMESFile::instrumentation_t ExpressionWhile::instrumentationType() const
{
  FUNCTION_TRACE;
  if (getOption().partial_condition)
    return CSMESFile::_INSTRUMENTATION_CONDITION_FALSE_ONLY;
  else
    return CSMESFile::_INSTRUMENTATION_CONDITION_FULL;
}

