
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _IDENTIFIER_ = 258,
     _STRUCT_ = 259,
     _EXTERN_ = 260,
     _PRIVATE_ = 261,
     _AND_OP_ = 262,
     _OR_OP_ = 263,
     _CASE_ = 264,
     _DEFAULT_ = 265,
     _IF_ = 266,
     _ELSE_ = 267,
     _SWITCH_ = 268,
     _WHILE_ = 269,
     _DO_ = 270,
     _FOR_ = 271,
     _GOTO_ = 272,
     _CONTINUE_ = 273,
     _BREAK_ = 274,
     _RETURN_ = 275,
     _THROW_ = 276,
     _SOMETHING_ = 277,
     _SOME_BINARY_OPERATOR_ = 278,
     _QUALIF_ = 279,
     _STATIC_ = 280,
     _TRY_ = 281,
     _CATCH_ = 282,
     _ASM_ = 283,
     _TOKEN_NOT_FOUND_ = 284,
     _TOKEN_TO_IGNORE_ = 285,
     _ASM_MS_LINE_ = 286,
     _UNSAFE_ = 287,
     _FINALLY_ = 288,
     _FOREACH_ = 289,
     _LOCK_ = 290,
     _USING_ = 291,
     _SET_GET_ = 292,
     _DELEGATE_ = 293,
     _YIELD_ = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 39 "c_cs_cpp_parser.y"

    token_t                         token                     ; 
    compound_t                      compound                  ; 
    exit_function_t                 exit_function             ; 
    qualified_identifier_t          qualified_identifier      ; 
    qualified_identifier_func_t     qualified_identifier_func ; 
    definition_decl_t               definition_decl           ; 
    logical_expr_t                  logical_expr              ; 
    function_statement_expr_t     function_statement_expr ; 



/* Line 1676 of yacc.c  */
#line 104 "c_cs_cpp_parser_yacc_debug.hxx"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE c_cs_cpp_parserlval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE c_cs_cpp_parserlloc;

