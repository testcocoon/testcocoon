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

#ifndef CPARSE_OPTION_INCLUDE_H
#define CPARSE_OPTION_INCLUDE_H
#include "parser.h"
#include <list>
#include <map>


class ParserOption
{
  public:
    ParserOption();

    void clear(const instrumentation_option_t &defaults);
    instrumentation_option_t option(int line) const;
    void push (int line,const instrumentation_option_t &option);
    void pop(int line);
  private:
    struct  instrumentation_option_stack_t
    {
      instrumentation_option_t option;
      int line_abs;
    };
    std::list<instrumentation_option_stack_t> _instrumentation_options;
    instrumentation_option_t _instrumentation_option_def;
    std::map<int,instrumentation_option_t>  _option_per_line;
    instrumentation_option_t option() const;
};

#endif
