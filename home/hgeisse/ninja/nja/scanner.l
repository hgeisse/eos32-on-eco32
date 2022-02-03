%{

/*
 * scanner.l -- scanner for assembler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "scanner.h"
#include "parser.tab.h"

static int lineNumber = 1;

%}

L		[A-Za-z_]
D		[0-9]
H		[0-9A-Fa-f]
S		(\+|\-)
ID		{L}({L}|{D})*
DECNUM		{S}?{D}+
HEXNUM		{S}?0x{H}+


%%


\/\/.*		{
		  /* comment: nothing returned */
		}

[\ \t\r]	{
		  /* white space: nothing returned */
		}

\n		{
		  lineNumber++;
		  yylval.noVal.line = lineNumber;
		  return NL;
		}

\.vers		{
		  yylval.noVal.line = lineNumber;
		  return DOT_VERS;
		}

halt		{
		  yylval.noVal.line = lineNumber;
		  return HALT;
		}

pushc		{
		  yylval.noVal.line = lineNumber;
		  return PUSHC;
		}

add		{
		  yylval.noVal.line = lineNumber;
		  return ADD;
		}

sub		{
		  yylval.noVal.line = lineNumber;
		  return SUB;
		}

mul		{
		  yylval.noVal.line = lineNumber;
		  return MUL;
		}

div		{
		  yylval.noVal.line = lineNumber;
		  return DIV;
		}

mod		{
		  yylval.noVal.line = lineNumber;
		  return MOD;
		}

rdint		{
		  yylval.noVal.line = lineNumber;
		  return RDINT;
		}

wrint		{
		  yylval.noVal.line = lineNumber;
		  return WRINT;
		}

rdchr		{
		  yylval.noVal.line = lineNumber;
		  return RDCHR;
		}

wrchr		{
		  yylval.noVal.line = lineNumber;
		  return WRCHR;
		}

pushg		{
		  yylval.noVal.line = lineNumber;
		  return PUSHG;
		}

popg		{
		  yylval.noVal.line = lineNumber;
		  return POPG;
		}

asf		{
		  yylval.noVal.line = lineNumber;
		  return ASF;
		}

rsf		{
		  yylval.noVal.line = lineNumber;
		  return RSF;
		}

pushl		{
		  yylval.noVal.line = lineNumber;
		  return PUSHL;
		}

popl		{
		  yylval.noVal.line = lineNumber;
		  return POPL;
		}

eq		{
		  yylval.noVal.line = lineNumber;
		  return EQ;
		}

ne		{
		  yylval.noVal.line = lineNumber;
		  return NE;
		}

lt		{
		  yylval.noVal.line = lineNumber;
		  return LT;
		}

le		{
		  yylval.noVal.line = lineNumber;
		  return LE;
		}

gt		{
		  yylval.noVal.line = lineNumber;
		  return GT;
		}

ge		{
		  yylval.noVal.line = lineNumber;
		  return GE;
		}

jmp		{
		  yylval.noVal.line = lineNumber;
		  return JMP;
		}

brf		{
		  yylval.noVal.line = lineNumber;
		  return BRF;
		}

brt		{
		  yylval.noVal.line = lineNumber;
		  return BRT;
		}

call		{
		  yylval.noVal.line = lineNumber;
		  return CALL;
		}

ret		{
		  yylval.noVal.line = lineNumber;
		  return RET;
		}

drop		{
		  yylval.noVal.line = lineNumber;
		  return DROP;
		}

pushr		{
		  yylval.noVal.line = lineNumber;
		  return PUSHR;
		}

popr		{
		  yylval.noVal.line = lineNumber;
		  return POPR;
		}

dup		{
		  yylval.noVal.line = lineNumber;
		  return DUP;
		}

new		{
		  yylval.noVal.line = lineNumber;
		  return NEW;
		}

getf		{
		  yylval.noVal.line = lineNumber;
		  return GETF;
		}

putf		{
		  yylval.noVal.line = lineNumber;
		  return PUTF;
		}

newa		{
		  yylval.noVal.line = lineNumber;
		  return NEWA;
		}

getfa		{
		  yylval.noVal.line = lineNumber;
		  return GETFA;
		}

putfa		{
		  yylval.noVal.line = lineNumber;
		  return PUTFA;
		}

getsz		{
		  yylval.noVal.line = lineNumber;
		  return GETSZ;
		}

pushn		{
		  yylval.noVal.line = lineNumber;
		  return PUSHN;
		}

refeq		{
		  yylval.noVal.line = lineNumber;
		  return REFEQ;
		}

refne		{
		  yylval.noVal.line = lineNumber;
		  return REFNE;
		}

{ID}		{
		  yylval.stringVal.line = lineNumber;
		  yylval.stringVal.val = allocate(strlen(yytext) + 1);
		  strcpy(yylval.stringVal.val, yytext);
		  return IDENT;
		}

\:		{
		  yylval.noVal.line = lineNumber;
		  return COLON;
		}

{DECNUM}	{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = strtol(yytext, NULL, 10);
		  return INTLIT;
		}

{HEXNUM}	{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = strtol(yytext, NULL, 16);
		  return INTLIT;
		}

\'\\n\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\n';
		  return INTLIT;
		}

\'\\r\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\r';
		  return INTLIT;
		}

\'\\t\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\t';
		  return INTLIT;
		}

\'\\b\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\b';
		  return INTLIT;
		}

\'\\a\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\a';
		  return INTLIT;
		}

\'\\\'\'	{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\'';
		  return INTLIT;
		}

\'\\\"\'	{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\"';
		  return INTLIT;
		}

\'\\\\\'	{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) '\\';
		  return INTLIT;
		}

\'\\.		{
		  error("line %d, illegal escape sequence",
		        lineNumber);
		}

\'.\'		{
		  yylval.intVal.line = lineNumber;
		  yylval.intVal.val = (unsigned char) yytext[1];
		  return INTLIT;
		}

\'		{
		  error("line %d, illegal character constant",
		        lineNumber);
		}

.		{
		  error("line %d, illegal character 0x%02X",
		        lineNumber, (unsigned char) yytext[0]);
		}


%%


int yywrap(void) {
  return 1;
}
