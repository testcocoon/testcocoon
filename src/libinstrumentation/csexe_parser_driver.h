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

#ifndef _CSEXE_PARSER_DDRIVER_H_
#define _CSEXE_PARSER_DDRIVER_H_
#include "csexe_parser_yacc.hxx"
#include <QString>

class CSExeParser; 

// Announce to Flex the prototype we want for lexing function, ...
# define YY_DECL                                      \
 yy::CSExeParser::token_type                         \
 yylex (yy::CSExeParser::semantic_type* yylval,      \
        yy::CSExeParser::location_type* yylloc,      \
        CSExeParserDriver& driver)                                       
// ... and declare it for the parser's sake.       
YY_DECL;

class CSExeParserDriver
{
  public:
    CSExeParserDriver();
    virtual ~CSExeParserDriver();

    // Handling the scanner.
    bool trace_scanning;
    int result;

    // Handling the parser.
    void parse (const QString & f);
    std::string file;
    bool trace_parsing;

    // Error handling.
    void error (const yy::location& l, const std::string& m);
    void error (const std::string& m);
};

#endif
