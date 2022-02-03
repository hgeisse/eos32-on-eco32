%{

/*
 * parser.y -- parser specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "absyn.h"
#include "scanner.h"
#include "parser.h"

Absyn *progTree;

%}

%union {
  NoVal noVal;
  IntVal intVal;
  StringVal stringVal;
  Absyn *node;
}

%token	<noVal>		BREAK DO ELSE GLOBAL IF LOCAL NEW NIL
%token	<noVal>		RECORD RETURN SIZEOF TYPE VOID WHILE
%token	<noVal>		LPAREN RPAREN LCURL RCURL LBRACK RBRACK
%token	<noVal>		ASGN COMMA SEMIC DOT
%token	<noVal>		LOGOR LOGAND LOGNOT
%token	<noVal>		EQ NE LT LE GT GE
%token	<noVal>		PLUS MINUS STAR SLASH PERCENT
%token	<stringVal>	INTEGERLIT
%token	<intVal>	CHARACTERLIT
%token	<intVal>	BOOLEANLIT
%token	<stringVal>	STRINGLIT
%token	<stringVal>	IDENT

%type	<node>		glob_dec_list
%type	<node>		glob_dec
%type	<node>		void
%type	<node>		type
%type	<node>		more_dims
%type	<node>		memb_dec_list
%type	<node>		memb_dec
%type	<node>		type_dec
%type	<node>		gvar_dec
%type	<node>		func_dec
%type	<node>		par_dec_list
%type	<node>		non_empty_par_dec_list
%type	<node>		par_dec
%type	<node>		lvar_dec_list
%type	<node>		lvar_dec
%type	<node>		stm_list
%type	<node>		stm
%type	<node>		empty_stm
%type	<node>		compound_stm
%type	<node>		assign_stm
%type	<node>		if_stm
%type	<node>		while_stm
%type	<node>		do_stm
%type	<node>		break_stm
%type	<node>		call_stm
%type	<node>		return_stm
%type	<node>		exp
%type	<node>		or_exp
%type	<node>		and_exp
%type	<node>		rel_exp
%type	<node>		add_exp
%type	<node>		mul_exp
%type	<node>		unary_exp
%type	<node>		primary_exp
%type	<node>		new_obj_spec
%type	<node>		var
%type	<node>		arg_list
%type	<node>		non_empty_arg_list

%start			program


%%


program			: glob_dec_list
			  {
			    progTree = $1;
			  }
			;

glob_dec_list		: /* empty */
			  {
			    $$ = emptyDecList();
			  }
			| glob_dec glob_dec_list
			  {
			    $$ = newDecList($1, $2);
			  }
			;

glob_dec		: type_dec
			  {
			    $$ = $1;
			  }
			| gvar_dec
			  {
			    $$ = $1;
			  }
			| func_dec
			  {
			    $$ = $1;
			  }
			;

void			: VOID
			  {
			    $$ = newVoidTy($1.line);
			  }
			;

type			: IDENT
			  {
			    $$ = newNameTy($1.line, newSym($1.val));
			  }
			| IDENT LBRACK RBRACK more_dims
			  {
			    Absyn *p;
			    p = newArrayTy($2.line, NULL, $4);
			    $$ = p;
			    while (p->u.arrayTy.baseTy != NULL) {
			      p = p->u.arrayTy.baseTy;
			    }
			    p->u.arrayTy.baseTy =
			      newNameTy($1.line, newSym($1.val));
			  }
			| RECORD LCURL memb_dec_list RCURL
			  {
			    $$ = newRecordTy($1.line, $3);
			  }
			;

more_dims		: /* empty */
			  {
			    /* end of dims list, will be set later */
			    $$ = NULL;
			  }
			| LBRACK RBRACK more_dims
			  {
			    $$ = newArrayTy($1.line, NULL, $3);
			  }
			;

memb_dec_list		: /* empty */
			  {
			    $$ = emptyDecList();
			  }
			| memb_dec memb_dec_list
			  {
			    $$ = newDecList($1, $2);
			  }
			;

memb_dec		: type IDENT SEMIC
			  {
			    $$ = newMembDec($2.line, newSym($2.val), $1);
			  }
			;

type_dec		: TYPE IDENT ASGN type SEMIC
			  {
			    $$ = newTypeDec($3.line, newSym($2.val), $4);
			  }
			;

gvar_dec		: GLOBAL type IDENT SEMIC
			  {
			    $$ = newGvarDec($3.line, newSym($3.val), $2);
			  }
			;

func_dec		: void IDENT LPAREN par_dec_list RPAREN
			  LCURL lvar_dec_list stm_list RCURL
			  {
			    $$ = newFuncDec($2.line, newSym($2.val),
			                    $1, $4, $7, $8);
			  }
			| type IDENT LPAREN par_dec_list RPAREN
			  LCURL lvar_dec_list stm_list RCURL
			  {
			    $$ = newFuncDec($2.line, newSym($2.val),
			                    $1, $4, $7, $8);
			  }
			;

par_dec_list		: /* empty */
			  {
			    $$ = emptyDecList();
			  }
			| non_empty_par_dec_list
			  {
			    $$ = $1;
			  }
			;

non_empty_par_dec_list	: par_dec
			  {
			    $$ = newDecList($1, emptyDecList());
			  }
			| par_dec COMMA non_empty_par_dec_list
			  {
			    $$ = newDecList($1, $3);
			  }
			;

par_dec			: type IDENT
			  {
			    $$ = newParDec($2.line, newSym($2.val), $1);
			  }
			;

lvar_dec_list		: /* empty */
			  {
			    $$ = emptyDecList();
			  }
			| lvar_dec lvar_dec_list
			  {
			    $$ = newDecList($1, $2);
			  }
			;

lvar_dec		: LOCAL type IDENT SEMIC
			  {
			    $$ = newLvarDec($3.line, newSym($3.val), $2);
			  }
			;

stm_list		: /* empty */
			  {
			    $$ = emptyStmList();
			  }
			| stm stm_list
			  {
			    $$ = newStmList($1, $2);
			  }
			;

stm			: empty_stm
			  {
			    $$ = $1;
			  }
			| compound_stm
			  {
			    $$ = $1;
			  }
			| assign_stm
			  {
			    $$ = $1;
			  }
			| if_stm
			  {
			    $$ = $1;
			  }
			| while_stm
			  {
			    $$ = $1;
			  }
			| do_stm
			  {
			    $$ = $1;
			  }
			| break_stm
			  {
			    $$ = $1;
			  }
			| call_stm
			  {
			    $$ = $1;
			  }
			| return_stm
			  {
			    $$ = $1;
			  }
			;

empty_stm		: SEMIC
			  {
			    $$ = newEmptyStm($1.line);
			  }
			;

compound_stm		: LCURL stm_list RCURL
			  {
			    $$ = newCompStm($1.line, $2);
			  }
			;

assign_stm		: var ASGN exp SEMIC
			  {
			    $$ = newAssignStm($2.line, $1, $3);
			  }
			;

if_stm			: IF LPAREN exp RPAREN stm
			  {
			    $$ = newIfStm1($1.line, $3, $5);
			  }
			| IF LPAREN exp RPAREN stm ELSE stm
			  {
			    $$ = newIfStm2($1.line, $3, $5, $7);
			  }
			;

while_stm		: WHILE LPAREN exp RPAREN stm
			  {
			    $$ = newWhileStm($1.line, $3, $5);
			  }
			;

do_stm			: DO stm WHILE LPAREN exp RPAREN SEMIC
			  {
			    $$ = newDoStm($1.line, $5, $2);
			  }
			;

break_stm		: BREAK SEMIC
			  {
			    $$ = newBreakStm($1.line);
			  }
			;

call_stm		: IDENT LPAREN arg_list RPAREN SEMIC
			  {
			    $$ = newCallStm($1.line, newSym($1.val), $3);
			  }
			;

return_stm		: RETURN SEMIC
			  {
			    $$ = newRetStm($1.line);
			  }
			| RETURN exp SEMIC
			  {
			    $$ = newRetExpStm($1.line, $2);
			  }
			;

exp			: or_exp
			  {
			    $$ = $1;
			  }
			;

or_exp			: and_exp
			  {
			    $$ = $1;
			  }
			| or_exp LOGOR and_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_LOR,
			                     $1, $3);
			  }
			;

and_exp			: rel_exp
			  {
			    $$ = $1;
			  }
			| and_exp LOGAND rel_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_LAND,
			                     $1, $3);
			  }
			;

rel_exp			: add_exp
			  {
			    $$ = $1;
			  }
			| add_exp EQ add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_EQ,
			                     $1, $3);
			  }
			| add_exp NE add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_NE,
			                     $1, $3);
			  }
			| add_exp LT add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_LT,
			                     $1, $3);
			  }
			| add_exp LE add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_LE,
			                     $1, $3);
			  }
			| add_exp GT add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_GT,
			                     $1, $3);
			  }
			| add_exp GE add_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_GE,
			                     $1, $3);
			  }
			;

add_exp			: mul_exp
			  {
			    $$ = $1;
			  }
			| add_exp PLUS mul_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_ADD,
			                     $1, $3);
			  }
			| add_exp MINUS mul_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_SUB,
			                     $1, $3);
			  }
			;

mul_exp			: unary_exp
			  {
			    $$ = $1;
			  }
			| mul_exp STAR unary_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_MUL,
			                     $1, $3);
			  }
			| mul_exp SLASH unary_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_DIV,
			                     $1, $3);
			  }
			| mul_exp PERCENT unary_exp
			  {
			    $$ = newBinopExp($2.line, ABSYN_BINOP_MOD,
			                     $1, $3);
			  }
			;

unary_exp		: primary_exp
			  {
			    $$ = $1;
			  }
			| PLUS unary_exp
			  {
			    $$ = newUnopExp($1.line, ABSYN_UNOP_PLUS, $2);
			  }
			| MINUS unary_exp
			  {
			    $$ = newUnopExp($1.line, ABSYN_UNOP_MINUS, $2);
			  }
			| LOGNOT unary_exp
			  {
			    $$ = newUnopExp($1.line, ABSYN_UNOP_LNOT, $2);
			  }
			;

primary_exp		: NIL
			  {
			    $$ = newNilExp($1.line);
			  }
			| INTEGERLIT
			  {
			    $$ = newIntExp($1.line, $1.val);
			  }
			| CHARACTERLIT
			  {
			    $$ = newCharExp($1.line, $1.val);
			  }
			| BOOLEANLIT
			  {
			    $$ = newBoolExp($1.line, $1.val);
			  }
			| STRINGLIT
			  {
			    $$ = newStringExp($1.line, $1.val);
			  }
			| NEW LPAREN new_obj_spec RPAREN
			  {
			    $$ = newNewExp($1.line, $3);
			  }
			| SIZEOF LPAREN exp RPAREN
			  {
			    $$ = newSizeofExp($1.line, $3);
			  }
			| LPAREN exp RPAREN
			  {
			    $$ = $2;
			  }
			| var
			  {
			    $$ = $1;
			  }
			| IDENT LPAREN arg_list RPAREN
			  {
			    $$ = newCallExp($1.line, newSym($1.val), $3);
			  }
			;

new_obj_spec		: IDENT
			  {
			    $$ = newNameTy($1.line, newSym($1.val));
			  }
			| IDENT LBRACK exp RBRACK more_dims
			  {
			    Absyn *p;
			    p = newArrayTy($2.line, $3, $5);
			    $$ = p;
			    while (p->u.arrayTy.baseTy != NULL) {
			      p = p->u.arrayTy.baseTy;
			    }
			    p->u.arrayTy.baseTy =
			      newNameTy($1.line, newSym($1.val));
			  }
			;

var			: IDENT
			  {
			    $$ = newSimpleVar($1.line, newSym($1.val));
			  }
			| primary_exp LBRACK exp RBRACK
			  {
			    $$ = newArrayVar($2.line, $1, $3);
			  }
			| primary_exp DOT IDENT
			  {
			    $$ = newRecordVar($3.line, $1, newSym($3.val));
			  }
			;

arg_list		: /* empty */
			  {
			    $$ = emptyExpList();
			  }
			| non_empty_arg_list
			  {
			    $$ = $1;
			  }
			;

non_empty_arg_list	: exp
			  {
			    $$ = newExpList($1, emptyExpList());
			  }
			| exp COMMA non_empty_arg_list
			  {
			    $$ = newExpList($1, $3);
			  }
			;


%%


void yyerror(char *msg) {
  error("%s in line %d", msg, yylval.noVal.line);
}
