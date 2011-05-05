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
#include "parseroption.h"

ParserOption::ParserOption()
{
}

void ParserOption::clear(const instrumentation_option_t &defaults)
{
  _instrumentation_option_def=defaults;
  _instrumentation_options.clear();
  _option_per_line.clear();
}

instrumentation_option_t ParserOption::option(int line) const
{
  instrumentation_option_t o=_instrumentation_option_def;
  int line_max=-1;
  for (std::map<int,instrumentation_option_t>::const_iterator it=_option_per_line.begin();it!=_option_per_line.end();++it)
  {
    int l=(*it).first;
    if (l<=line && l>line_max)
    {
      line_max=l;
      o=(*it).second;
    }
  }
  return o;
}

void ParserOption::push (int line,const instrumentation_option_t &option)
{
  instrumentation_option_stack_t o;
  o.option=option;
  o.line_abs=line;
  _instrumentation_options.push_back(o);
  _option_per_line[line]=option;
}

void ParserOption::pop(int line)
{
  if (!_instrumentation_options.empty())
    _instrumentation_options.pop_back();
  instrumentation_option_t o=option();
  _option_per_line[line]=o;
}

instrumentation_option_t ParserOption::option() const
{
  if (!_instrumentation_options.empty())
    return _instrumentation_options.back().option;
  else
    return _instrumentation_option_def;
}
