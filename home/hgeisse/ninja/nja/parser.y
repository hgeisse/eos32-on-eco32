%{

/*
 * parser.y -- parser for assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "scanner.h"
#include "parser.h"
#include "coder.h"
#include "table.h"

%}

%union {
  NoVal noVal;
  IntVal intVal;
  StringVal stringVal;
}

%token	<noVal>		NL
%token	<noVal>		DOT_VERS
%token	<noVal>		HALT
%token	<noVal>		PUSHC
%token	<noVal>		ADD
%token	<noVal>		SUB
%token	<noVal>		MUL
%token	<noVal>		DIV
%token	<noVal>		MOD
%token	<noVal>		RDINT
%token	<noVal>		WRINT
%token	<noVal>		RDCHR
%token	<noVal>		WRCHR
%token	<noVal>		PUSHG
%token	<noVal>		POPG
%token	<noVal>		ASF
%token	<noVal>		RSF
%token	<noVal>		PUSHL
%token	<noVal>		POPL
%token	<noVal>		EQ
%token	<noVal>		NE
%token	<noVal>		LT
%token	<noVal>		LE
%token	<noVal>		GT
%token	<noVal>		GE
%token	<noVal>		JMP
%token	<noVal>		BRF
%token	<noVal>		BRT
%token	<noVal>		CALL
%token	<noVal>		RET
%token	<noVal>		DROP
%token	<noVal>		PUSHR
%token	<noVal>		POPR
%token	<noVal>		DUP
%token	<noVal>		NEW
%token	<noVal>		GETF
%token	<noVal>		PUTF
%token	<noVal>		NEWA
%token	<noVal>		GETFA
%token	<noVal>		PUTFA
%token	<noVal>		GETSZ
%token	<noVal>		PUSHN
%token	<noVal>		REFEQ
%token	<noVal>		REFNE
%token	<stringVal>	IDENT
%token	<noVal>		COLON
%token	<intVal>	INTLIT

%start			program


%%


program			: statements
			;

statements		: /* empty */
			| statements labelled_statement
			;

labelled_statement	: statement
			| labels statement
			;

labels			: label
			| labels label
			;

label			: IDENT COLON
			  {
			    enter($1.val, getCurrAddr());
			  }
			;

statement		: NL
			| DOT_VERS INTLIT NL
			  {
			    if ($2.val != VERSION) {
			      error("wrong assembler version");
			    }
			  }
			| HALT NL
			  {
			    code0(OP_HALT);
			  }
			| PUSHC INTLIT NL
			  {
			    code1(OP_PUSHC, $2.val);
			  }
			| PUSHC IDENT NL
			  {
			    code1(OP_PUSHC, lookup($2.val, getCurrAddr()));
			  }
			| ADD NL
			  {
			    code0(OP_ADD);
			  }
			| SUB NL
			  {
			    code0(OP_SUB);
			  }
			| MUL NL
			  {
			    code0(OP_MUL);
			  }
			| DIV NL
			  {
			    code0(OP_DIV);
			  }
			| MOD NL
			  {
			    code0(OP_MOD);
			  }
			| RDINT NL
			  {
			    code0(OP_RDINT);
			  }
			| WRINT NL
			  {
			    code0(OP_WRINT);
			  }
			| RDCHR NL
			  {
			    code0(OP_RDCHR);
			  }
			| WRCHR NL
			  {
			    code0(OP_WRCHR);
			  }
			| PUSHG INTLIT NL
			  {
			    code1(OP_PUSHG, $2.val);
			    data($2.val);
			  }
			| POPG INTLIT NL
			  {
			    code1(OP_POPG, $2.val);
			    data($2.val);
			  }
			| ASF INTLIT NL
			  {
			    code1(OP_ASF, $2.val);
			  }
			| RSF NL
			  {
			    code0(OP_RSF);
			  }
			| PUSHL INTLIT NL
			  {
			    code1(OP_PUSHL, $2.val);
			  }
			| POPL INTLIT NL
			  {
			    code1(OP_POPL, $2.val);
			  }
			| EQ NL
			  {
			    code0(OP_EQ);
			  }
			| NE NL
			  {
			    code0(OP_NE);
			  }
			| LT NL
			  {
			    code0(OP_LT);
			  }
			| LE NL
			  {
			    code0(OP_LE);
			  }
			| GT NL
			  {
			    code0(OP_GT);
			  }
			| GE NL
			  {
			    code0(OP_GE);
			  }
			| JMP IDENT NL
			  {
			    code1(OP_JMP, lookup($2.val, getCurrAddr()));
			  }
			| BRF IDENT NL
			  {
			    code1(OP_BRF, lookup($2.val, getCurrAddr()));
			  }
			| BRT IDENT NL
			  {
			    code1(OP_BRT, lookup($2.val, getCurrAddr()));
			  }
			| CALL IDENT NL
			  {
			    code1(OP_CALL, lookup($2.val, getCurrAddr()));
			  }
			| RET NL
			  {
			    code0(OP_RET);
			  }
			| DROP INTLIT NL
			  {
			    code1(OP_DROP, $2.val);
			  }
			| PUSHR NL
			  {
			    code0(OP_PUSHR);
			  }
			| POPR NL
			  {
			    code0(OP_POPR);
			  }
			| DUP NL
			  {
			    code0(OP_DUP);
			  }
			| NEW INTLIT NL
			  {
			    code1(OP_NEW, $2.val);
			  }
			| GETF INTLIT NL
			  {
			    code1(OP_GETF, $2.val);
			  }
			| PUTF INTLIT NL
			  {
			    code1(OP_PUTF, $2.val);
			  }
			| NEWA NL
			  {
			    code0(OP_NEWA);
			  }
			| GETFA NL
			  {
			    code0(OP_GETFA);
			  }
			| PUTFA NL
			  {
			    code0(OP_PUTFA);
			  }
			| GETSZ NL
			  {
			    code0(OP_GETSZ);
			  }
			| PUSHN NL
			  {
			    code0(OP_PUSHN);
			  }
			| REFEQ NL
			  {
			    code0(OP_REFEQ);
			  }
			| REFNE NL
			  {
			    code0(OP_REFNE);
			  }
			;


%%


void yyerror(char *msg) {
  error("%s in line %d", msg, yylval.noVal.line);
}
