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

%option prefix="c_cs_cpp_parser"
/*%option outfile="lex.yy.c"*/
/*%option fast*/
%option 8bit
%option stack
/*%option nounistd*/
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
SP          [ \t\v\r\f]

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
/* [\xEE-\xEF][\x80-\xBF][\x80-\xBF] - UTF8_BYTE_ORDER_MARK */
U6          {U6_1}|{U6_2}|{U6_3}
U7          [\xF0][\x90-\xBF][\x80-\xBF][\x80-\xBF]
U8          [\xF1-\xF3][\x80-\xBF][\x80-\xBF][\x80-\xBF]
U9          [\xF4][\x80-\x8F][\x80-\xBF][\x80-\xBF]

L       {ASCII}|{U2}|{U3}|{U4}|{U5}|{U6}|{U7}|{U8}|{U9}
U      {U1}|{U2}|{U3}|{U4}|{U5}|{U6}|{U7}|{U8}|{U9}
UTF8_BYTE_ORDER_MARK   \xEF\xBB\xBF
letter_char            {L}
ident_char             {D}|{L}|"@"
identifier             {letter_char}{ident_char}*
at_identifier          \@{identifier}
preprocessor           ^{SP}*#{SP}*

%{
#include <string.h>
#include <stdio.h>
#include <list>
#include "coveragescannerpdef.h"
#include "parsemisc.h"
#include "compilerinterface.h"
#include "c_cs_cpp_parser.h"
#ifdef __GNUC__
#define yyunput __attribute__ ((unused)) yyunput
#define yy_top_state __attribute__ ((unused)) yy_top_state
#endif
static int (*yy_input)(char *,int)=NULL;
FILE *test_input_file=NULL;
#define YY_INPUT(buf,result,max_size) result=yy_input(buf,max_size)
extern void *source_p;
extern void *expressions_p;
int bracket_level=0;
#define yyinput input
#define yylval c_cs_cpp_parserlval
#define yylloc c_cs_cpp_parserlloc
extern YYLTYPE yylloc;
extern YYSTYPE yylval;
bool in_template_definition=false;
static bool ignore_file=false;
static bool ignore_define=false;
static CompilerInterface::extension_t parse_language=CompilerInterface::EXT_SOURCE_C;
static std::list<bool> ignore_brackets;
static inline int token_for(const char *lexeme,CompilerInterface::extension_t);
static inline int return_a_token() { return !(ignore_file || ignore_define) ; }
#define RETURN_TOKEN(X) { if (return_a_token()) return X; }
#define PUSH_STATE(X) { DEBUG2("yy_push_state:%s\n",#X); yy_push_state(X); }
#define POP_STATE() { DEBUG1("yy_pop_state\n"); yy_pop_state(); }
%}

%s PARSE_C PARSE_CPP PREPROCESSOR PARSE_CS COMMENT LINE_COMMENT
%%
                      /***** Preprocessor *****/
<PARSE_C,PARSE_CPP,PARSE_CS>{preprocessor}"pragma"{SP}*CoverageScanner[^\n]*    { process_pragma_coveragescanner(true,yytext,yyleng,&yylval,&yylloc); }
<PARSE_C,PARSE_CPP,PARSE_CS>{preprocessor}                    { process_token(yytext,yyleng,&yylval,&yylloc); PUSH_STATE(PREPROCESSOR); }
<PREPROCESSOR>\n               { process_return(yytext,yyleng,&yylval,&yylloc);  ignore_define=preprocessor_ignore_area(); POP_STATE(); }
<PREPROCESSOR>"region"{SP}+CoverageScanner[^\r\n]*         { process_pragma_coveragescanner(false,yytext,yyleng,&yylval,&yylloc); }
<PREPROCESSOR>"region"[^\r\n]*         { push_instrumentation_option(yylloc.first_line,instrumentation_option(yylloc.first_line)) ;;process_token(yytext,yyleng,&yylval,&yylloc);  /* ignore */ }
<PREPROCESSOR>"endregion"      { pop_instrumentation_option(yylloc.first_line) ; process_token(yytext,yyleng,&yylval,&yylloc); /* ignore */ }
<PREPROCESSOR>("line")?{SP}*{D}+[^\n]*    { extract_name_preprocessor_line(yytext,yyleng,&yylval,&yylloc,ignore_file); }
<PREPROCESSOR>"pragma"[^\n]*    { process_pragma(PRAGMA_UNHANDLED,false,yytext,yyleng,&yylval,&yylloc);  }
<PREPROCESSOR>[^\n\r]*                {   process_token(yytext,yyleng,&yylval,&yylloc); preprocessor_parse(yytext) ;   ignore_define=preprocessor_ignore_area(); POP_STATE(); /* ignore */ }
<PREPROCESSOR>.                { process_token(yytext,yyleng,&yylval,&yylloc); ; /* ignore */ }

                      /***** Comments *****/
<PARSE_C,PARSE_CPP,PARSE_CS>"/*"                  { process_token(yytext,yyleng,&yylval,&yylloc); PUSH_STATE(COMMENT); }
<COMMENT>\n        { process_return(yytext,yyleng,&yylval,&yylloc); ; /* ignore */ }
<COMMENT>"*/"      { process_token(yytext,yyleng,&yylval,&yylloc); POP_STATE(); }
<COMMENT>.    {process_token(yytext,yyleng,&yylval,&yylloc); /*extract_name_preprocessor_borland(yytext,yyleng,&yylval,&yylloc,&ignore_file); */}

<PARSE_C,PARSE_CPP,PARSE_CS>"//"                  { process_token(yytext,yyleng,&yylval,&yylloc); PUSH_STATE(LINE_COMMENT); }
<LINE_COMMENT>\n        { process_return(yytext,yyleng,&yylval,&yylloc); POP_STATE(); }
<LINE_COMMENT>.         { process_token(yytext,yyleng,&yylval,&yylloc); ; /* ignore */ }

                      /***** C/C++/C# parser  ******/
<PARSE_C,PARSE_CPP>("__attribute__"|_?"_declspec"|"__pragma"|"_Pragma") { skip_attribute(yytext,yyleng,&yylval,&yylloc); }

<PARSE_C,PARSE_CPP,PARSE_CS>"__asm"{SP}[^;(){}]"\n" 	{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_ASM_MS_LINE_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"~"{SP}*{L}({L}|{D})*		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_IDENTIFIER_); }
<PARSE_CPP,PARSE_CS>"operator"{SP}*("[]"|[*+-/=%<>|^&~][*+-/=%<>|^&~]*)		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_IDENTIFIER_); }


<PARSE_C,PARSE_CPP,PARSE_CS>0[xX]{H}+{IS}?		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>0{D}+{IS}?		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>{D}+{IS}?		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"L"?'(\\.|[^\\'])+'		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }

<PARSE_C,PARSE_CPP,PARSE_CS>{D}+{E}{FS}?i?		{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>{D}*"."{D}+({E})?{FS}?i?	{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>{D}+"."{D}*({E})?{FS}?i?	{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }

<PARSE_C,PARSE_CPP,PARSE_CS>"L"?\"(\\.|[^\\"])*\"	{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }

<PARSE_C,PARSE_CPP,PARSE_CS>"&&"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_AND_OP_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"||"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_OR_OP_); }
<PARSE_C,PARSE_CPP,PARSE_CS>":"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(':'); }
<PARSE_C,PARSE_CPP,PARSE_CS>";"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(';'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('='); }
<PARSE_C,PARSE_CPP,PARSE_CS>"!"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('!'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"=="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<<"			{ if (in_template_definition) { REJECT; } process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>">>"			{ if (in_template_definition) { REJECT; } process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"->"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('<'); }
<PARSE_C,PARSE_CPP,PARSE_CS>">"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('>'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<?"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>">?"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>">="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"!="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>[+\-*^/%]"="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('='); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<<="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('='); }
<PARSE_C,PARSE_CPP,PARSE_CS>">>="			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('='); }
<PARSE_C,PARSE_CPP,PARSE_CS>"("			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('('); }
<PARSE_C,PARSE_CPP,PARSE_CS>")"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(')'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"["			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('['); }
<PARSE_C,PARSE_CPP,PARSE_CS>"<":/[^:]			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('['); }
<PARSE_C,PARSE_CPP,PARSE_CS>"]"|":>"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(']'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"[]"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP>("__if_exists"|"__if_not_exists"){SP}*"("[^)]*")"{SP}*"{"			{ process_token(yytext,yyleng,&yylval,&yylloc); ignore_brackets.push_back(true); }
<PARSE_C,PARSE_CPP,PARSE_CS>"{"|"<%"			{ bracket_level++; process_token(yytext,yyleng,&yylval,&yylloc); ignore_brackets.push_back(false); RETURN_TOKEN('{'); }
<PARSE_C,PARSE_CPP,PARSE_CS>"}"|"%>"			{ bracket_level--; process_token(yytext,yyleng,&yylval,&yylloc); if (ignore_brackets.empty()) { RETURN_TOKEN('}'); } bool ignore=ignore_brackets.back() ; ignore_brackets.pop_back() ; if (!ignore) { RETURN_TOKEN('}'); } }
<PARSE_C,PARSE_CPP,PARSE_CS>","			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(','); }
<PARSE_C,PARSE_CPP,PARSE_CS>"?"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN('?'); }
<PARSE_CPP,PARSE_CS>"::"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_QUALIF_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"++"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>"--"			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }
<PARSE_C,PARSE_CPP,PARSE_CS>[+\-^/%]			{ process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOME_BINARY_OPERATOR_); }
<PARSE_C,PARSE_CPP,PARSE_CS>{UTF8_BYTE_ORDER_MARK}     { }
<PARSE_C,PARSE_CPP,PARSE_CS>{identifier}          { process_token(yytext,yyleng,&yylval,&yylloc); int token=token_for(yytext,parse_language); if (token!=_TOKEN_TO_IGNORE_) { RETURN_TOKEN(token); } }
<PARSE_C,PARSE_CPP,PARSE_CS>{at_identifier}       { process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_IDENTIFIER_); }
<PARSE_C,PARSE_CPP,PARSE_CS>{SP}+		{ process_spaces(yytext,yyleng,&yylval,&yylloc); }
<PARSE_C,PARSE_CPP,PARSE_CS>\n		{ process_return(yytext,yyleng,&yylval,&yylloc); }
<PARSE_C,PARSE_CPP,PARSE_CS>.					         { process_token(yytext,yyleng,&yylval,&yylloc); RETURN_TOKEN(_SOMETHING_); }

%%

typedef struct name_value_c_cpp {
  const char *name;
  int value_cpp;
  int value_c;
  int value_cs;
} name_value_c_cpp;
static struct name_value_c_cpp const keywords_c_cpp [] = { 
  /* This list must remain sorted!!! */
/* keyword         , C++               , C                 , C#  */
  {"__asm"         , _ASM_             , _ASM_             , _IDENTIFIER_                     } ,
  {"__asm__"       , _ASM_             , _ASM_             , _IDENTIFIER_                     } ,
  {"__except"      , _CATCH_           , _IDENTIFIER_      , _IDENTIFIER_                     } ,
  {"__extension__" , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _IDENTIFIER_                     } ,
  {"__finally"     , _FINALLY_         , _IDENTIFIER_      , _IDENTIFIER_                     } ,
  {"__stdcall"     , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _IDENTIFIER_                     } ,
  {"__try"         , _TRY_             , _IDENTIFIER_      , _IDENTIFIER_                     } ,
  {"__volatile"    , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _IDENTIFIER_                     } ,
  {"__volatile__"  , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _IDENTIFIER_                     } ,
  {"as"            , _IDENTIFIER_      , _IDENTIFIER_      , _SOME_BINARY_OPERATOR_           } ,
  {"asm"           , _ASM_             , _ASM_             , _IDENTIFIER_                     } ,
  {"break"         , _BREAK_           , _BREAK_           , _BREAK_                          } ,
  {"case"          , _CASE_            , _CASE_            , _CASE_                           } ,
  {"catch"         , _CATCH_           , _IDENTIFIER_      , _CATCH_                          } ,
  {"checked"       , _IDENTIFIER_      , _IDENTIFIER_      , _UNSAFE_                         } ,
  {"class"         , _STRUCT_          , _IDENTIFIER_      , _STRUCT_                         } ,
  {"const"         , _SOMETHING_       , _SOMETHING_       , _SOMETHING_                      } ,
  {"continue"      , _CONTINUE_        , _CONTINUE_        , _CONTINUE_                       } ,
  {"default"       , _DEFAULT_         , _DEFAULT_         , _DEFAULT_                        } ,
  {"delegate"      , _IDENTIFIER_      , _IDENTIFIER_      , _DELEGATE_                       } ,
  {"do"            , _DO_              , _DO_              , _DO_                             } ,
  {"else"          , _ELSE_            , _ELSE_            , _ELSE_                           } ,
  {"explicit"      , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"extern"        , _EXTERN_          , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_                } ,
  {"finally"       , _IDENTIFIER_      , _IDENTIFIER_      , _FINALLY_                        } ,
  {"for"           , _FOR_             , _FOR_             , _FOR_                            } ,
  {"foreach"       , _IDENTIFIER_      , _IDENTIFIER_      , _FOREACH_                        } ,
  {"get"           , _IDENTIFIER_      , _IDENTIFIER_      , _SET_GET_                        } ,
  {"goto"          , _GOTO_            , _GOTO_            , _GOTO_                           } ,
  {"if"            , _IF_              , _IF_              , _IF_                             } ,
  {"implicit"      , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"interface"     , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"internal"      , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"is"            , _IDENTIFIER_      , _IDENTIFIER_      , _SOME_BINARY_OPERATOR_           } ,
  {"lock"          , _IDENTIFIER_      , _IDENTIFIER_      , _LOCK_                           } ,
  {"namespace"     , _STRUCT_          , _IDENTIFIER_      , _STRUCT_                         } ,
  {"override"      , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"partial"       , _IDENTIFIER_      , _IDENTIFIER_      , _UNSAFE_                         } ,
  {"private"       , _PRIVATE_         , _IDENTIFIER_      , _PRIVATE_                        } ,
  {"protected"     , _PRIVATE_         , _IDENTIFIER_      , _PRIVATE_                        } ,
  {"public"        , _PRIVATE_         , _IDENTIFIER_      , _PRIVATE_                        } ,
  {"readonly"      , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"return"        , _RETURN_          , _RETURN_          , _RETURN_                         } ,
  {"sealed"        , _IDENTIFIER_      , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"set"           , _IDENTIFIER_      , _IDENTIFIER_      , _SET_GET_                        } ,
  {"static"        , _STATIC_          , _STATIC_          , _STATIC_                         } ,
  {"struct"        , _STRUCT_          , _STRUCT_          , _STRUCT_                         } ,
  {"switch"        , _SWITCH_          , _SWITCH_          , _SWITCH_                         } ,
  {"throw"         , _THROW_           , _IDENTIFIER_      , _THROW_                          } ,
  {"try"           , _TRY_             , _IDENTIFIER_      , _TRY_                            } ,
  {"typedef"       , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _IDENTIFIER_                     } ,
  {"unchecked"     , _IDENTIFIER_      , _IDENTIFIER_      , _UNSAFE_                         } ,
  {"union"         , _STRUCT_          , _STRUCT_          , _IDENTIFIER_                     } ,
  {"unsafe"        , _IDENTIFIER_      , _IDENTIFIER_      , _UNSAFE_                         } ,
  {"using"         , _TOKEN_TO_IGNORE_ , _IDENTIFIER_      , _USING_                          } ,
  {"virtual"       , _TOKEN_TO_IGNORE_ , _IDENTIFIER_      , _TOKEN_TO_IGNORE_                } ,
  {"volatile"      , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_ , _TOKEN_TO_IGNORE_                } ,
  {"while"         , _WHILE_           , _WHILE_           , _WHILE_                          } ,
  {"yield"         , _IDENTIFIER_      , _IDENTIFIER_      , _YIELD_                          } ,
};    


/* Conduct a binary search for lexeme in the keywords array
 * between indices start (inclusive) and finish (exclusive)
 */
static const name_value_c_cpp * bin_search(const char *lexeme, int start, int finish)
{
  if (start >= finish)  /* Not found */
      return NULL;
  else {
    int mid = (start+finish)/2;
    int cmp = strcmp(lexeme,keywords_c_cpp[mid].name);
    if (cmp == 0) 
      return &keywords_c_cpp[mid];
    else if (cmp < 0)
      return bin_search(lexeme,start,mid);
    else
      return bin_search(lexeme,mid+1,finish);
  }
}

#ifndef NO_DEBUG
static const name_value_c_cpp * simple_search(const char *lexeme, int start, int finish)
{
  for (int i=start;i<finish;i++)
  {
    if (strcmp(lexeme,keywords_c_cpp[i].name)==0)
      return &keywords_c_cpp[i];
  }
  return NULL;
}
#endif

int token_for(const char *lexeme,CompilerInterface::extension_t parse_language)
{
  static int num_keywords = sizeof(keywords_c_cpp) / sizeof(name_value_c_cpp);
  const name_value_c_cpp *token = bin_search(lexeme,0,num_keywords);
  ASSERT(token==simple_search(lexeme,0,num_keywords));
  if (token)
  {
    switch (parse_language)
    {
      default:
        FATAL1("Wrong language to parse\n");
      case CompilerInterface::EXT_SOURCE_C:   return token->value_c;
      case CompilerInterface::EXT_SOURCE_CPP: return token->value_cpp;
      case CompilerInterface::EXT_SOURCE_CS:  return token->value_cs;
    }
  }
  return _IDENTIFIER_;
}


int
yywrap()
{
	return(1);
}

static int yy_input_std(char *buf,int max_size)
{
  int result;
  if ( (result = fread( (char *) buf,1, max_size,test_input_file )) < 0 ) 
  {
    FATAL1("Error reading source file");
  }
  return result;
}

void init_c_cs_cpp_parserlex(CompilerInterface::extension_t _parse_language, int (*yy_input_func)(char *,int),const char *filename_orig)
{
  bracket_level=0;
  ignore_brackets.clear();
  in_template_definition=false;
  for (int i=1;i<(int)(sizeof(keywords_c_cpp) / sizeof(name_value_c_cpp));i++)
  {
    if (strcmp(keywords_c_cpp[i].name,keywords_c_cpp[i-1].name)<=0)
    {
      FATAL3("Internal error: table not sorted (%s<%s)",keywords_c_cpp[i].name,keywords_c_cpp[i-1].name);
    }
  }
  if (yy_input_func)
    yy_input=yy_input_func;
  else
    yy_input=yy_input_std;
  ignore_file=false;
  ignore_define=false;
  parse_language=_parse_language;
  switch (parse_language)
  {
    case CompilerInterface::EXT_SOURCE_CPP:
      DEBUG1("Parsing C++ code\n");
      BEGIN(PARSE_CPP);
      break;
    case CompilerInterface::EXT_SOURCE_CS: 
      DEBUG1("Parsing C# code\n");
      BEGIN(PARSE_CS);
      break;
    case CompilerInterface::EXT_SOURCE_C:  
      DEBUG1("Parsing C code\n");
      BEGIN(PARSE_C);
      break;
    default:
      FATAL1("Wrong language to parse\n");
  }

  init_parser_lex(input,sizeof(YYSTYPE),filename_orig);
}

void yyprint_token(FILE * f,const char * text)
{
  FUNCTION_TRACE;
  YYFPRINTF(f,"%li:%s:%li,%li %s ",line_absolute,filename,line,column,text);
}

CompilerInterface::extension_t language_to_parse()
{
  return parse_language;
}
