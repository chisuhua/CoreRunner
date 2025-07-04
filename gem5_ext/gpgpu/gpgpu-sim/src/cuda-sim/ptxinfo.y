/*
Copyright (c) 2009-2011, Tor M. Aamodt
The University of British Columbia
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.
Neither the name of The University of British Columbia nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

%{
typedef void * yyscan_t;
#include "ptx_loader.h"
%}

%define api.pure full
%parse-param {yyscan_t scanner}
%parse-param {ptxinfo_data* ptxinfo}
%lex-param {yyscan_t scanner}
%lex-param {ptxinfo_data* ptxinfo}

%union {
  int    int_value;
  char * string_value;
}

%token <int_value> INT_OPERAND
%token HEADER
%token INFO
%token FUNC
%token USED
%token REGS
%token BYTES
%token LMEM
%token SMEM
%token CMEM
%token GMEM
%token <string_value> IDENTIFIER
%token PLUS
%token COMMA
%token LEFT_SQUARE_BRACKET
%token RIGHT_SQUARE_BRACKET
%token COLON
%token SEMICOLON
%token QUOTE
%token LINE
%token <string_value> WARNING
%token FOR
%token TEXTURES
%token DUPLICATE
%token <string_value> FUNCTION
%token <string_value> VARIABLE
%token FATAL_GPGPU

%{
	#include <stdlib.h>
	#include <string.h>
	
	static unsigned g_declared;
	static unsigned g_system;
	int ptxinfo_lex(YYSTYPE * yylval_param, yyscan_t yyscanner, ptxinfo_data* ptxinfo);
	void yyerror(yyscan_t yyscanner, ptxinfo_data* ptxinfo, const char* msg);
	void ptxinfo_function(const char *fname );
	void ptxinfo_regs( unsigned nregs );
	void ptxinfo_lmem( unsigned declared, unsigned system );
	void ptxinfo_gmem( unsigned declared, unsigned system );
	void ptxinfo_smem( unsigned declared, unsigned system );
	void ptxinfo_cmem( unsigned nbytes, unsigned bank );
	void ptxinfo_linenum( unsigned );
	void ptxinfo_dup_type( const char* );
%}

%%

input:	/* empty */
	| input line
	;

line: 	HEADER INFO COLON line_info
	| HEADER IDENTIFIER COMMA LINE INT_OPERAND SEMICOLON WARNING
	| HEADER WARNING { printf("GPGPU-Sim: ptxas %s\n", $2); }
	| HEADER IDENTIFIER COMMA LINE INT_OPERAND SEMICOLON DUPLICATE duplicate { ptxinfo_linenum($5); }
	| HEADER FATAL_GPGPU
	;

line_info: function_name
	| function_info { ptxinfo->ptxinfo_addinfo(); }
	| gmem_info
	;

function_name:	FUNC QUOTE IDENTIFIER QUOTE { ptxinfo_function($3); }
	|  FUNC QUOTE IDENTIFIER QUOTE FOR QUOTE IDENTIFIER QUOTE { ptxinfo_function($3); }
	;
	
function_info: info
	| function_info COMMA info
	;

gmem_info: INT_OPERAND BYTES GMEM
	;

info: 	  USED INT_OPERAND REGS { ptxinfo_regs($2); }
	| tuple LMEM { ptxinfo_lmem(g_declared,g_system); }
	| tuple SMEM { ptxinfo_smem(g_declared,g_system); }
	| INT_OPERAND BYTES CMEM LEFT_SQUARE_BRACKET INT_OPERAND RIGHT_SQUARE_BRACKET { ptxinfo_cmem($1,$5); }
	| INT_OPERAND BYTES GMEM { ptxinfo_gmem($1,0); }
	| INT_OPERAND BYTES LMEM { ptxinfo_lmem($1,0); }
	| INT_OPERAND BYTES SMEM { ptxinfo_smem($1,0); }
	| INT_OPERAND BYTES CMEM { ptxinfo_cmem($1,0); }
	| INT_OPERAND REGS { ptxinfo_regs($1); }
	| INT_OPERAND TEXTURES {}
	;

tuple: INT_OPERAND PLUS INT_OPERAND BYTES { g_declared=$1; g_system=$3; }

duplicate:	FUNCTION QUOTE IDENTIFIER QUOTE { ptxinfo_dup_type($1); }
	| VARIABLE QUOTE IDENTIFIER QUOTE { ptxinfo_dup_type($1); }
	;

%%


