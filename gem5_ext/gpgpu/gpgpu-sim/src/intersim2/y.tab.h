/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_MNT_D_SOURCE_GITHUB_SIM_GPGPU_SIM_DISTRIBUTION_1_BUILD_GCC_7_4_0_CUDA_10000_DEBUG_INTERSIM2_Y_TAB_H_INCLUDED
# define YY_YY_MNT_D_SOURCE_GITHUB_SIM_GPGPU_SIM_DISTRIBUTION_1_BUILD_GCC_7_4_0_CUDA_10000_DEBUG_INTERSIM2_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STR = 258,
    NUM = 259,
    FNUM = 260
  };
#endif
/* Tokens.  */
#define STR 258
#define NUM 259
#define FNUM 260

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 16 "config.y" /* yacc.c:1909  */

  char   *name;
  int    num;
  double fnum;

#line 70 "/mnt/d/source/github/sim/gpgpu-sim_distribution-1/build/gcc-7.4.0/cuda-10000/debug/intersim2/y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MNT_D_SOURCE_GITHUB_SIM_GPGPU_SIM_DISTRIBUTION_1_BUILD_GCC_7_4_0_CUDA_10000_DEBUG_INTERSIM2_Y_TAB_H_INCLUDED  */
