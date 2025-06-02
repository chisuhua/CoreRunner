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

#ifndef YY_PTX_PTX_TAB_H_INCLUDED
# define YY_PTX_PTX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptx_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STRING = 258,
    OPCODE = 259,
    WMMA_DIRECTIVE = 260,
    LAYOUT = 261,
    CONFIGURATION = 262,
    ALIGN_DIRECTIVE = 263,
    BRANCHTARGETS_DIRECTIVE = 264,
    BYTE_DIRECTIVE = 265,
    CALLPROTOTYPE_DIRECTIVE = 266,
    CALLTARGETS_DIRECTIVE = 267,
    CONST_DIRECTIVE = 268,
    CONSTPTR_DIRECTIVE = 269,
    PTR_DIRECTIVE = 270,
    ENTRY_DIRECTIVE = 271,
    EXTERN_DIRECTIVE = 272,
    FILE_DIRECTIVE = 273,
    FUNC_DIRECTIVE = 274,
    GLOBAL_DIRECTIVE = 275,
    LOCAL_DIRECTIVE = 276,
    LOC_DIRECTIVE = 277,
    MAXNCTAPERSM_DIRECTIVE = 278,
    MAXNNREG_DIRECTIVE = 279,
    MAXNTID_DIRECTIVE = 280,
    MINNCTAPERSM_DIRECTIVE = 281,
    PARAM_DIRECTIVE = 282,
    PRAGMA_DIRECTIVE = 283,
    REG_DIRECTIVE = 284,
    REQNTID_DIRECTIVE = 285,
    SECTION_DIRECTIVE = 286,
    SHARED_DIRECTIVE = 287,
    SREG_DIRECTIVE = 288,
    SSTARR_DIRECTIVE = 289,
    STRUCT_DIRECTIVE = 290,
    SURF_DIRECTIVE = 291,
    TARGET_DIRECTIVE = 292,
    TEX_DIRECTIVE = 293,
    UNION_DIRECTIVE = 294,
    VERSION_DIRECTIVE = 295,
    ADDRESS_SIZE_DIRECTIVE = 296,
    VISIBLE_DIRECTIVE = 297,
    WEAK_DIRECTIVE = 298,
    IDENTIFIER = 299,
    INT_OPERAND = 300,
    FLOAT_OPERAND = 301,
    DOUBLE_OPERAND = 302,
    S8_TYPE = 303,
    S16_TYPE = 304,
    S32_TYPE = 305,
    S64_TYPE = 306,
    U8_TYPE = 307,
    U16_TYPE = 308,
    U32_TYPE = 309,
    U64_TYPE = 310,
    F16_TYPE = 311,
    F32_TYPE = 312,
    F64_TYPE = 313,
    FF64_TYPE = 314,
    B8_TYPE = 315,
    B16_TYPE = 316,
    B32_TYPE = 317,
    B64_TYPE = 318,
    BB64_TYPE = 319,
    BB128_TYPE = 320,
    PRED_TYPE = 321,
    TEXREF_TYPE = 322,
    SAMPLERREF_TYPE = 323,
    SURFREF_TYPE = 324,
    V2_TYPE = 325,
    V3_TYPE = 326,
    V4_TYPE = 327,
    COMMA = 328,
    PRED = 329,
    HALF_OPTION = 330,
    EXTP_OPTION = 331,
    EQ_OPTION = 332,
    NE_OPTION = 333,
    LT_OPTION = 334,
    LE_OPTION = 335,
    GT_OPTION = 336,
    GE_OPTION = 337,
    LO_OPTION = 338,
    LS_OPTION = 339,
    HI_OPTION = 340,
    HS_OPTION = 341,
    EQU_OPTION = 342,
    NEU_OPTION = 343,
    LTU_OPTION = 344,
    LEU_OPTION = 345,
    GTU_OPTION = 346,
    GEU_OPTION = 347,
    NUM_OPTION = 348,
    NAN_OPTION = 349,
    CF_OPTION = 350,
    SF_OPTION = 351,
    NSF_OPTION = 352,
    LEFT_SQUARE_BRACKET = 353,
    RIGHT_SQUARE_BRACKET = 354,
    WIDE_OPTION = 355,
    SPECIAL_REGISTER = 356,
    MINUS = 357,
    PLUS = 358,
    COLON = 359,
    SEMI_COLON = 360,
    EXCLAMATION = 361,
    PIPE = 362,
    RIGHT_BRACE = 363,
    LEFT_BRACE = 364,
    EQUALS = 365,
    PERIOD = 366,
    BACKSLASH = 367,
    DIMENSION_MODIFIER = 368,
    RN_OPTION = 369,
    RZ_OPTION = 370,
    RM_OPTION = 371,
    RP_OPTION = 372,
    RNI_OPTION = 373,
    RZI_OPTION = 374,
    RMI_OPTION = 375,
    RPI_OPTION = 376,
    UNI_OPTION = 377,
    GEOM_MODIFIER_1D = 378,
    GEOM_MODIFIER_2D = 379,
    GEOM_MODIFIER_3D = 380,
    SAT_OPTION = 381,
    FTZ_OPTION = 382,
    NEG_OPTION = 383,
    SYNC_OPTION = 384,
    RED_OPTION = 385,
    ARRIVE_OPTION = 386,
    ATOMIC_POPC = 387,
    ATOMIC_AND = 388,
    ATOMIC_OR = 389,
    ATOMIC_XOR = 390,
    ATOMIC_CAS = 391,
    ATOMIC_EXCH = 392,
    ATOMIC_ADD = 393,
    ATOMIC_INC = 394,
    ATOMIC_DEC = 395,
    ATOMIC_MIN = 396,
    ATOMIC_MAX = 397,
    LEFT_ANGLE_BRACKET = 398,
    RIGHT_ANGLE_BRACKET = 399,
    LEFT_PAREN = 400,
    RIGHT_PAREN = 401,
    APPROX_OPTION = 402,
    FULL_OPTION = 403,
    ANY_OPTION = 404,
    ALL_OPTION = 405,
    BALLOT_OPTION = 406,
    GLOBAL_OPTION = 407,
    CTA_OPTION = 408,
    SYS_OPTION = 409,
    EXIT_OPTION = 410,
    ABS_OPTION = 411,
    TO_OPTION = 412,
    CA_OPTION = 413,
    CG_OPTION = 414,
    CS_OPTION = 415,
    LU_OPTION = 416,
    CV_OPTION = 417,
    WB_OPTION = 418,
    WT_OPTION = 419,
    NC_OPTION = 420,
    UP_OPTION = 421,
    DOWN_OPTION = 422,
    BFLY_OPTION = 423,
    IDX_OPTION = 424,
    PRMT_F4E_MODE = 425,
    PRMT_B4E_MODE = 426,
    PRMT_RC8_MODE = 427,
    PRMT_RC16_MODE = 428,
    PRMT_ECL_MODE = 429,
    PRMT_ECR_MODE = 430
  };
#endif
/* Tokens.  */
#define STRING 258
#define OPCODE 259
#define WMMA_DIRECTIVE 260
#define LAYOUT 261
#define CONFIGURATION 262
#define ALIGN_DIRECTIVE 263
#define BRANCHTARGETS_DIRECTIVE 264
#define BYTE_DIRECTIVE 265
#define CALLPROTOTYPE_DIRECTIVE 266
#define CALLTARGETS_DIRECTIVE 267
#define CONST_DIRECTIVE 268
#define CONSTPTR_DIRECTIVE 269
#define PTR_DIRECTIVE 270
#define ENTRY_DIRECTIVE 271
#define EXTERN_DIRECTIVE 272
#define FILE_DIRECTIVE 273
#define FUNC_DIRECTIVE 274
#define GLOBAL_DIRECTIVE 275
#define LOCAL_DIRECTIVE 276
#define LOC_DIRECTIVE 277
#define MAXNCTAPERSM_DIRECTIVE 278
#define MAXNNREG_DIRECTIVE 279
#define MAXNTID_DIRECTIVE 280
#define MINNCTAPERSM_DIRECTIVE 281
#define PARAM_DIRECTIVE 282
#define PRAGMA_DIRECTIVE 283
#define REG_DIRECTIVE 284
#define REQNTID_DIRECTIVE 285
#define SECTION_DIRECTIVE 286
#define SHARED_DIRECTIVE 287
#define SREG_DIRECTIVE 288
#define SSTARR_DIRECTIVE 289
#define STRUCT_DIRECTIVE 290
#define SURF_DIRECTIVE 291
#define TARGET_DIRECTIVE 292
#define TEX_DIRECTIVE 293
#define UNION_DIRECTIVE 294
#define VERSION_DIRECTIVE 295
#define ADDRESS_SIZE_DIRECTIVE 296
#define VISIBLE_DIRECTIVE 297
#define WEAK_DIRECTIVE 298
#define IDENTIFIER 299
#define INT_OPERAND 300
#define FLOAT_OPERAND 301
#define DOUBLE_OPERAND 302
#define S8_TYPE 303
#define S16_TYPE 304
#define S32_TYPE 305
#define S64_TYPE 306
#define U8_TYPE 307
#define U16_TYPE 308
#define U32_TYPE 309
#define U64_TYPE 310
#define F16_TYPE 311
#define F32_TYPE 312
#define F64_TYPE 313
#define FF64_TYPE 314
#define B8_TYPE 315
#define B16_TYPE 316
#define B32_TYPE 317
#define B64_TYPE 318
#define BB64_TYPE 319
#define BB128_TYPE 320
#define PRED_TYPE 321
#define TEXREF_TYPE 322
#define SAMPLERREF_TYPE 323
#define SURFREF_TYPE 324
#define V2_TYPE 325
#define V3_TYPE 326
#define V4_TYPE 327
#define COMMA 328
#define PRED 329
#define HALF_OPTION 330
#define EXTP_OPTION 331
#define EQ_OPTION 332
#define NE_OPTION 333
#define LT_OPTION 334
#define LE_OPTION 335
#define GT_OPTION 336
#define GE_OPTION 337
#define LO_OPTION 338
#define LS_OPTION 339
#define HI_OPTION 340
#define HS_OPTION 341
#define EQU_OPTION 342
#define NEU_OPTION 343
#define LTU_OPTION 344
#define LEU_OPTION 345
#define GTU_OPTION 346
#define GEU_OPTION 347
#define NUM_OPTION 348
#define NAN_OPTION 349
#define CF_OPTION 350
#define SF_OPTION 351
#define NSF_OPTION 352
#define LEFT_SQUARE_BRACKET 353
#define RIGHT_SQUARE_BRACKET 354
#define WIDE_OPTION 355
#define SPECIAL_REGISTER 356
#define MINUS 357
#define PLUS 358
#define COLON 359
#define SEMI_COLON 360
#define EXCLAMATION 361
#define PIPE 362
#define RIGHT_BRACE 363
#define LEFT_BRACE 364
#define EQUALS 365
#define PERIOD 366
#define BACKSLASH 367
#define DIMENSION_MODIFIER 368
#define RN_OPTION 369
#define RZ_OPTION 370
#define RM_OPTION 371
#define RP_OPTION 372
#define RNI_OPTION 373
#define RZI_OPTION 374
#define RMI_OPTION 375
#define RPI_OPTION 376
#define UNI_OPTION 377
#define GEOM_MODIFIER_1D 378
#define GEOM_MODIFIER_2D 379
#define GEOM_MODIFIER_3D 380
#define SAT_OPTION 381
#define FTZ_OPTION 382
#define NEG_OPTION 383
#define SYNC_OPTION 384
#define RED_OPTION 385
#define ARRIVE_OPTION 386
#define ATOMIC_POPC 387
#define ATOMIC_AND 388
#define ATOMIC_OR 389
#define ATOMIC_XOR 390
#define ATOMIC_CAS 391
#define ATOMIC_EXCH 392
#define ATOMIC_ADD 393
#define ATOMIC_INC 394
#define ATOMIC_DEC 395
#define ATOMIC_MIN 396
#define ATOMIC_MAX 397
#define LEFT_ANGLE_BRACKET 398
#define RIGHT_ANGLE_BRACKET 399
#define LEFT_PAREN 400
#define RIGHT_PAREN 401
#define APPROX_OPTION 402
#define FULL_OPTION 403
#define ANY_OPTION 404
#define ALL_OPTION 405
#define BALLOT_OPTION 406
#define GLOBAL_OPTION 407
#define CTA_OPTION 408
#define SYS_OPTION 409
#define EXIT_OPTION 410
#define ABS_OPTION 411
#define TO_OPTION 412
#define CA_OPTION 413
#define CG_OPTION 414
#define CS_OPTION 415
#define LU_OPTION 416
#define CV_OPTION 417
#define WB_OPTION 418
#define WT_OPTION 419
#define NC_OPTION 420
#define UP_OPTION 421
#define DOWN_OPTION 422
#define BFLY_OPTION 423
#define IDX_OPTION 424
#define PRMT_F4E_MODE 425
#define PRMT_B4E_MODE 426
#define PRMT_RC8_MODE 427
#define PRMT_RC16_MODE 428
#define PRMT_ECL_MODE 429
#define PRMT_ECR_MODE 430

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "ptx.y"

  double double_value;
  float  float_value;
  int    int_value;
  char * string_value;
  void * ptr_value;

#line 415 "ptx.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



// int ptx_parse (yyscan_t scanner, ptx_recognizer* recognizer);

#endif /* !YY_PTX_PTX_TAB_H_INCLUDED  */
