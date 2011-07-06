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
%option 8bit
%option bison-locations
%option bison-bridge
%option yylineno
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


%s START_LINE LINE_INSTRUMENTATION_SOURCE_FILENAME LINE_INSTRUMENTATION_SOURCE_SIGNATURE LINE_INSTRUMENTATION_SOURCE_SIZE LINE_INSTRUMENTATION_VALUE LINE_TITLE LINE_STATUS

%{
#include <string.h>
#include <stdio.h>
#if OS_WIN32
#include <io.h>
#define isatty _isatty
#endif
#include "debug.h"
#include "csexe_parser.h"
#include "instrumentation.h"
#ifdef ___GNUC___
#define yyunput ____attribute____ ((unused)) yyunput
#define yy_top_state ____attribute____ ((unused)) yy_top_state
#endif
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno;
static int (*yy_input)(char *,int)=NULL;
#define YY_INPUT(buf,result,max_size) result=yy_input(buf,max_size)
#define yyinput input
#define yylval csexe_parserlval
extern YYSTYPE yylval;
static inline unsigned int hex2uint(const char *s);
%}

%%

<INITIAL,START_LINE>{
"# Measurements"           {  return __CSEXE_MEASUREMENT__ ; }
"!"                        {  BEGIN(LINE_STATUS); return __CSEXE_STATUS__ ; }
"*"                        {  BEGIN(LINE_TITLE); return __CSEXE_TITLE__ ; }
[\xA7]                     {  BEGIN(LINE_TITLE); return __CSEXE_TITLE__ ; /* § */ }
"/"                        {  BEGIN(LINE_INSTRUMENTATION_SOURCE_SIZE); return __CSEXE_INSTRUMENTATION_SOURCE__ ; }
"\\"                       {  BEGIN(LINE_INSTRUMENTATION_VALUE); return __CSEXE_INSTRUMENTATION_VALUES__ ; }
}

<LINE_INSTRUMENTATION_SOURCE_SIZE>{
{number}     {  return __NUMBER__ ; }
":"          {  BEGIN(LINE_INSTRUMENTATION_SOURCE_SIGNATURE); return ':'; }
}

<LINE_INSTRUMENTATION_SOURCE_SIGNATURE>{
{number}     {  return __NUMBER__ ; }
":"          {  BEGIN(LINE_INSTRUMENTATION_SOURCE_FILENAME); return ':'; }
}

<LINE_INSTRUMENTATION_SOURCE_FILENAME>[^\r\n]*  { yylval->str=STRDUP(yytext); return __STRING__; }

<LINE_TITLE>[^\r\n]*                    {  yylval->str=STRDUP(yytext); return __STRING__; }

<LINE_INSTRUMENTATION_VALUE>{
"?"                      {  yylval->value=Instrumentation::EXECUTION_STATE_HIDDEN; return __NUMBER__ ; }
"-"                      {  yylval->value=Instrumentation::EXECUTION_STATE_NOT_EXECUTED;return __NUMBER__ ; }
"+"                      {  yylval->value=static_cast<Instrumentation::execution_state_t>(1) ; return __NUMBER__ ; }
[0-9A-F]{8}              {  yylval->value=hex2uint(yytext) ; 
                            if (yylval->value>Instrumentation::EXECUTION_MAX)
                              yylval->value=Instrumentation::EXECUTION_MAX;
                            return __NUMBER__ ; 
                         }
}

<LINE_STATUS>{
"PASSED"                      {  return __STATUS_PASSED__ ; }
"FAILED"                      {  return __STATUS_FAILED__ ; }
"CHECK_MANUALLY"              {  return __STATUS_CHECK_MANUALLY__ ; }
}

<*>\n  {  BEGIN(START_LINE); /* ignore */ }
<*>\r  {  /* ignore */ }
%%

static inline unsigned int hex2uint(const char *s)
{
  unsigned int v=0;
  ASSERT(strlen(s)==8);
  for (int i=0;i<8;i++)
  {
    char c=s[i] ;
    if (c<='9' && c>='0')
      c-='0';
    else
      c-='A'-10;
    v|=c<<(4*(7-i));
  }
  return v;
}

static QIODevice *qiodevice_p=NULL;
static int yy_input_qiodevice(char *buf,int max_size)
{
  int result;
  if ( (result = qiodevice_p->read( (char *) buf, max_size )) < 0 ) 
    return 0;
  return result;
}

int yywrap()
{
	return(1);
}

void CSExeParser::init_csexe_parserlex(CSMesIO &csmes,const QString &filename,QIODevice &file,const ExecutionName &name_orig,CSMesIO::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,ExecutionNames &new_executions,QString &info,QString &short_status,QString &errmsgs,QHash<ExecutionName,Executions::modules_executions_private_t> *undo_backup_p,CSMesIO::progress_function_t progress_p)
{
  BEGIN(START_LINE);
  yylineno=1;
  yy_input=yy_input_qiodevice;
  qiodevice_p=&file;
}
