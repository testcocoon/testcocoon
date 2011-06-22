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

%option prefix="csexe_parser"
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
string             {letter_char}{ident_char}*
number                 {D}*


rank_specifier         "["{white_space}*(","{white_space}*)*"]"


%x IN_COMMENT IN_PREPROCESSOR IN__LINE__COMMENT

%s IN_ATTRIB IN_ACCESSOR IN_GETSET


%{
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include "csexe_parser.h"
#ifdef ___GNUC___
#define yyunput ____attribute____ ((unused)) yyunput
#define yy_top_state ____attribute____ ((unused)) yy_top_state
#endif
static int (*yy_input)(char *,int)=NULL;
#define YY_INPUT(buf,result,max_size) result=yy_input(buf,max_size)
#define yyinput input
#define yylval csexe_parserlval
#define yylloc csexe_parserlloc

extern YYLTYPE yylloc;
extern YYSTYPE yylval;
%}

%%

'!'              { return '!' ; }
'*'              { return '*' ; }
'-'              { return '-' ; }
"+"              { return '+' ; }
"/"              { return '/' ; }
':'              { return ':'; }
'\\'             { return '\\'; }
'\n'             { return '\n'; }
'\r'             { /* ignore */ }
[\xA7]           { return '*' ; /* § */ }
{number}         { return __NUMBER__ ; }
.                { return __STRING__; }



%%

int
yywrap()
{
	return(1);
}

char *csexe_input_buffer=NULL;
static int   csexe_input_buffer_pos=0;
int   csexe_input_buffer_sz=0;

static int yy_input_buffer(char *buf,int max_size)
{
  int sz=max_size;
  if (sz>=csexe_input_buffer_sz-csexe_input_buffer_pos)
    sz=csexe_input_buffer_sz-csexe_input_buffer_pos;
  if (sz)
  {
    memcpy(buf,&csexe_input_buffer[csexe_input_buffer_pos],sz);
    csexe_input_buffer_pos+=sz;
  }

  return sz;
}

void init_csexe_parserlex( )
{
  FREE(csexe_input_buffer);
  csexe_input_buffer=NULL;
  csexe_input_buffer_pos=0;
  csexe_input_buffer_sz=0;
  yy_input=yy_input_buffer;
}
