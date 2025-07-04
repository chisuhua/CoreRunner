/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_PTXINFO_PTXINFO_TAB_H_INCLUDED
# define YY_PTXINFO_PTXINFO_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptxinfo_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT_OPERAND = 258,
    HEADER = 259,
    INFO_GPGPU = 260,
    FUNC = 261,
    USED = 262,
    REGS = 263,
    BYTES = 264,
    LMEM = 265,
    SMEM = 266,
    CMEM = 267,
    GMEM = 268,
    IDENTIFIER = 269,
    PLUS = 270,
    COMMA = 271,
    LEFT_SQUARE_BRACKET = 272,
    RIGHT_SQUARE_BRACKET = 273,
    COLON = 274,
    SEMICOLON = 275,
    QUOTE = 276,
    LINE = 277,
    WARNING = 278,
    FOR = 279,
    TEXTURES = 280,
    DUPLICATE = 281,
    FUNCTION = 282,
    VARIABLE = 283,
    FATAL_GPGPU = 284
  };
#endif
/* Tokens.  */
#define INT_OPERAND 258
#define HEADER 259
#define INFO_GPGPU 260
#define FUNC 261
#define USED 262
#define REGS 263
#define BYTES 264
#define LMEM 265
#define SMEM 266
#define CMEM 267
#define GMEM 268
#define IDENTIFIER 269
#define PLUS 270
#define COMMA 271
#define LEFT_SQUARE_BRACKET 272
#define RIGHT_SQUARE_BRACKET 273
#define COLON 274
#define SEMICOLON 275
#define QUOTE 276
#define LINE 277
#define WARNING 278
#define FOR 279
#define TEXTURES 280
#define DUPLICATE 281
#define FUNCTION 282
#define VARIABLE 283
#define FATAL_GPGPU 284

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 41 "ptxinfo.y"

  int    int_value;
  char * string_value;

#line 120 "ptxinfo.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int ptxinfo_parse (yyscan_t scanner, ptxinfo_data* ptxinfo);

#endif /* !YY_PTXINFO_PTXINFO_TAB_H_INCLUDED  */
