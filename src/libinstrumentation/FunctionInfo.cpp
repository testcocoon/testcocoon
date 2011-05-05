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

#include "FunctionInfo.h"
void FunctionInfo::clear()
{
  _ScopedName.clear();
  _Prototype.clear();
  _end_line_org     = -1;
  _start_line_org   = -1;
  _end_column_org   = -1;
  _start_column_org = -1;
  _end_line_pre     = -1;
  _start_line_pre   = -1;
  _end_column_pre   = -1;
  _start_column_pre = -1;
}


bool FunctionInfo::valid() const
{
  return ! (
      _end_column_org    == -1 &&
      _end_column_pre    == -1 &&
      _end_line_org      == -1 &&
      _end_line_pre      == -1 &&
      _start_column_org  == -1 &&
      _start_column_pre  == -1 &&
      _start_line_org    == -1 &&
      _start_line_pre    == -1 );
}
