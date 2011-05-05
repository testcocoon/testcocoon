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

%option prefix="preprocessor_parser"
/*%option outfile="lex.yy.c"*/
%option fast
%option stack
%option 8bit
/*%option nounistd*/

D           [0-9]
ASCII     [a-zA-Z_]

U1          [\x0-\x7F]
U2          [\xC2-\xDF][\x80-\xBF]
U3          [\xE0][\xA0-\xBF][\x80-\xBF]
U4          [\xE1-\xEC][\x80-\xBF][\x80-\xBF]
U5          [\xED][\x80-\x9F][\x80-\xBF]
U6_1        [\xEE-\xEF][\x80-\xBA][\x80-\xBF]
U6_2        [\xEE-\xEF]\xBB[\x80-\xBE]
U6_3          [\xEE-\xEF][\xBC-\xBF][\x80-\xBF]
/* [\xEE-\xEF][\x80-\xBF][\x80-\xBF] - UTF8__BYTE_ORDER__MARK */
U6          {U6_1}|{U6_2}|{U6_3}
U7          [\xF0][\x90-\xBF][\x80-\xBF][\x80-\xBF]
U8          [\xF1-\xF3][\x80-\xBF][\x80-\xBF][\x80-\xBF]
U9          [\xF4][\x80-\x8F][\x80-\xBF][\x80-\xBF]

L       {ASCII}|{U2}|{U3}|{U4}|{U5}|{U6}|{U7}|{U8}|{U9}
U      {U1}|{U2}|{U3}|{U4}|{U5}|{U6}|{U7}|{U8}|{U9}
UTF8__BYTE_ORDER__MARK   \xEF\xBB\xBF



white_space            [ \t\n\r]

preprocessor           ^[ \t]*#

dec_digit              [0-9]
hex_digit              [0-9A-Fa-f]
int_suffix             [UuLl]|[Uu][Ll]|[Ll][Uu]
dec_literal            {dec_digit}+{int_suffix}?
hex_literal            0[xX]{hex_digit}+{int_suffix}?
integer_literal        {dec_literal}|{hex_literal}

real_suffix            [FfDdMm]
sign                   [+\-]
exponent_part          [eE]{sign}?{dec_digit}+
whole_real1            {dec_digit}+{exponent_part}{real_suffix}?
whole_real2            {dec_digit}+{real_suffix}
part_real              {dec_digit}*\.{dec_digit}+{exponent_part}?{real_suffix}?
real_literal           {whole_real1}|{whole_real2}|{part_real}

single_char            [^\\\']
simple_esc_seq         \\[\'\"\\0abfnrtv]
uni_esc_seq1           \\u{hex_digit}{4}
uni_esc_seq2           \\U{hex_digit}{8}
uni_esc_seq            {uni_esc_seq1}|{uni_esc_seq2}
hex_esc_seq            \\x{hex_digit}{1,4}
character              {single_char}|{simple_esc_seq}|{hex_esc_seq}|{uni_esc_seq}
character_literal      \'{character}\'


single_string_char     [^\\\"]
reg_string_char        {single_string_char}|{simple_esc_seq}|{hex_esc_seq}|{uni_esc_seq}
regular_string         \"{reg_string_char}*\"
single_verbatim_char   [^\"]
quote_esc_seq          \"\"
verb_string_char       {single_verbatim_char}|{quote_esc_seq}
verbatim_string        @\"{verb_string_char}*\"
string_literal         {regular_string}|{verbatim_string}


letter_char            {L}
ident_char             {D}|{L}|"@"
identifier             {letter_char}{ident_char}*
number                 {D}*


rank_specifier         "["{white_space}*(","{white_space}*)*"]"


%x IN_COMMENT IN_PREPROCESSOR IN__LINE__COMMENT

%s IN_ATTRIB IN_ACCESSOR IN_GETSET


%{
#include <string.h>
#include <stdio.h>
#include "coveragescannerpdef.h"
#include "parsemisc.h"
#include "preprocessor_parser.h"
#ifdef ___GNUC___
#define yyunput ____attribute____ ((unused)) yyunput
#define yy_top_state ____attribute____ ((unused)) yy_top_state
#endif
static int (*yy_input)(char *,int)=NULL;
#define YY_INPUT(buf,result,max_size) result=yy_input(buf,max_size)
#define yyinput input
#define yylval preprocessor_parserlval
#define yylloc preprocessor_parserlloc
#define COPY_YYTEXT { char *_preprocessor_text=strdup(yytext); yylval.token.text = _preprocessor_text; _preprocessor_texts.push_back(_preprocessor_text); }
extern std::list<char*> _preprocessor_texts;

extern YYLTYPE yylloc;
extern YYSTYPE yylval;
%}

%%

"line"           { COPY_YYTEXT ; ; /* ignore */ }
"define"         { COPY_YYTEXT ; return __DEFINE__; }
"if"             { COPY_YYTEXT ; return __IF__; }
"else"           { COPY_YYTEXT ; return __ELSE__; }
"endif"          { COPY_YYTEXT ; return __ENDIF__; }
"elif"           { COPY_YYTEXT ; return __ELIF__ ; }
"undef"          { COPY_YYTEXT ; return __UNDEF__ ; }
"true"           { COPY_YYTEXT ; return __TRUE__ ; }
"false"          { COPY_YYTEXT ; return __FALSE__; }
{identifier}     { COPY_YYTEXT ; return __IDENTIFIER__ ; }
{number}         { COPY_YYTEXT ; return __NUMBER__ ; }
{string_literal} { COPY_YYTEXT ; ; /* ignore */ }
{white_space}    { COPY_YYTEXT ; ; /* ignore */ }
"!"              { COPY_YYTEXT ; return __NOT__ ; }
"=="             { COPY_YYTEXT ; return __EQUAL__ ; }
"!="             { COPY_YYTEXT ; return __NOT_EQUAL__; }
"&&"             { COPY_YYTEXT ; return __AND__ ; }
"||"             { COPY_YYTEXT ; return __OR__ ; }
"("              { COPY_YYTEXT ; return '('; }
")"              { COPY_YYTEXT ; return ')'; }
.                { COPY_YYTEXT ; ; /* ignore */ }



%%


int
yywrap()
{
	return(1);
}

std::list<char*> _preprocessor_texts;
char *preprocessor_input_buffer=NULL;
static int   preprocessor_input_buffer_pos=0;
int   preprocessor_input_buffer_sz=0;

static void clear_preprocessor_texts()
{
  for (std::list<char*>::iterator it=_preprocessor_texts.begin();it!=_preprocessor_texts.end();++it)
    FREE(*it);
  _preprocessor_texts.clear();
}

static int yy_input_buffer(char *buf,int max_size)
{
  int sz=max_size;
  if (sz>=preprocessor_input_buffer_sz-preprocessor_input_buffer_pos)
    sz=preprocessor_input_buffer_sz-preprocessor_input_buffer_pos;
  if (sz)
  {
    memcpy(buf,&preprocessor_input_buffer[preprocessor_input_buffer_pos],sz);
    preprocessor_input_buffer_pos+=sz;
  }

  return sz;
}

void init_preprocessor_parserlex( )
{
  clear_preprocessor_texts();
  FREE(preprocessor_input_buffer);
  preprocessor_input_buffer=NULL;
  preprocessor_input_buffer_pos=0;
  preprocessor_input_buffer_sz=0;
  yy_input=yy_input_buffer;
  init_parser_lex(input,sizeof(YYSTYPE),NULL);
}
