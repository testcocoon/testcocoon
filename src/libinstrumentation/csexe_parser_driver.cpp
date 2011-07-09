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

#include "csexe_parser_driver.h"
#include "csexe_parser.h"

CSExeParserDriver::CSExeParserDriver () : trace_scanning (false), trace_parsing (false)
{
}

CSExeParserDriver::~CSExeParserDriver ()
{
}

void CSExeParserDriver::parse (const QString &f)
{
  file = f.toStdString();
  yy::CSExeParser parser (*this);
  //parser.set_debug_level (trace_parsing);
  parser.parse ();
}

void CSExeParserDriver::error (const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

void CSExeParserDriver::error (const std::string& m)
{
  std::cerr << m << std::endl;
}
