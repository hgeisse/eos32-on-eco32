%{

/*
 * scanner.l -- scanner specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "absyn.h"
#include "scanner.h"
#include "parser.tab.h"

static int lineNumber = 1;

%}

L			[A-Za-z_]
D			[0-9]
H			[0-9A-Fa-f]
ID			{L}({L}|{D})*
DECNUM			{D}+
HEXNUM			0x{H}+


%%


\/\/.*			{
			  /* comment: nothing returned */
			}

[\ \t\r]+		{
			  /* white space: nothing returned */
			}

\n			{
			  /* newline: nothing returned */
			  lineNumber++;
			}

break			{
			  yylval.noVal.line = lineNumber;
			  return BREAK;
			}

do			{
			  yylval.noVal.line = lineNumber;
			  return DO;
			}

else			{
			  yylval.noVal.line = lineNumber;
			  return ELSE;
			}

global			{
			  yylval.noVal.line = lineNumber;
			  return GLOBAL;
			}

if			{
			  yylval.noVal.line = lineNumber;
			  return IF;
			}

local			{
			  yylval.noVal.line = lineNumber;
			  return LOCAL;
			}

new			{
			  yylval.noVal.line = lineNumber;
			  return NEW;
			}

record			{
			  yylval.noVal.line = lineNumber;
			  return RECORD;
			}

return			{
			  yylval.noVal.line = lineNumber;
			  return RETURN;
			}

sizeof			{
			  yylval.noVal.line = lineNumber;
			  return SIZEOF;
			}

type			{
			  yylval.noVal.line = lineNumber;
			  return TYPE;
			}

void			{
			  yylval.noVal.line = lineNumber;
			  return VOID;
			}

while			{
			  yylval.noVal.line = lineNumber;
			  return WHILE;
			}

\(			{
			  yylval.noVal.line = lineNumber;
			  return LPAREN;
			}

\)			{
			  yylval.noVal.line = lineNumber;
			  return RPAREN;
			}

\{			{
			  yylval.noVal.line = lineNumber;
			  return LCURL;
			}

\}			{
			  yylval.noVal.line = lineNumber;
			  return RCURL;
			}

\[			{
			  yylval.noVal.line = lineNumber;
			  return LBRACK;
			}

\]			{
			  yylval.noVal.line = lineNumber;
			  return RBRACK;
			}

\=			{
			  yylval.noVal.line = lineNumber;
			  return ASGN;
			}

\,			{
			  yylval.noVal.line = lineNumber;
			  return COMMA;
			}

\;			{
			  yylval.noVal.line = lineNumber;
			  return SEMIC;
			}

\.			{
			  yylval.noVal.line = lineNumber;
			  return DOT;
			}

\|\|			{
			  yylval.noVal.line = lineNumber;
			  return LOGOR;
			}

\&\&			{
			  yylval.noVal.line = lineNumber;
			  return LOGAND;
			}

\!			{
			  yylval.noVal.line = lineNumber;
			  return LOGNOT;
			}

\=\=			{
			  yylval.noVal.line = lineNumber;
			  return EQ;
			}

\!\=			{
			  yylval.noVal.line = lineNumber;
			  return NE;
			}

\<			{
			  yylval.noVal.line = lineNumber;
			  return LT;
			}

\<\=			{
			  yylval.noVal.line = lineNumber;
			  return LE;
			}

\>			{
			  yylval.noVal.line = lineNumber;
			  return GT;
			}

\>\=			{
			  yylval.noVal.line = lineNumber;
			  return GE;
			}

\+			{
			  yylval.noVal.line = lineNumber;
			  return PLUS;
			}

\-			{
			  yylval.noVal.line = lineNumber;
			  return MINUS;
			}

\*			{
			  yylval.noVal.line = lineNumber;
			  return STAR;
			}

\/			{
			  yylval.noVal.line = lineNumber;
			  return SLASH;
			}

\%			{
			  yylval.noVal.line = lineNumber;
			  return PERCENT;
			}

nil			{
			  yylval.noVal.line = lineNumber;
			  return NIL;
			}

{DECNUM}		{
			  yylval.stringVal.line = lineNumber;
			  yylval.stringVal.val =
			    (char *) allocate(strlen(yytext) + 1);
			  strcpy(yylval.stringVal.val, yytext);
			  return INTEGERLIT;
			}

{HEXNUM}		{
			  yylval.stringVal.line = lineNumber;
			  yylval.stringVal.val =
			    (char *) allocate(strlen(yytext) + 1);
			  strcpy(yylval.stringVal.val, yytext);
			  return INTEGERLIT;
			}

\'\\n\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\n';
			  return CHARACTERLIT;
			}

\'\\r\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\r';
			  return CHARACTERLIT;
			}

\'\\t\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\t';
			  return CHARACTERLIT;
			}

\'\\b\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\b';
			  return CHARACTERLIT;
			}

\'\\a\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\a';
			  return CHARACTERLIT;
			}

\'\\\'\'		{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\'';
			  return CHARACTERLIT;
			}

\'\\\"\'		{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\"';
			  return CHARACTERLIT;
			}

\'\\\\\'		{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) '\\';
			  return CHARACTERLIT;
			}

\'\\.			{
			  error("illegal escape sequence in line %d",
			        lineNumber);
			}

\'.\'			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = (unsigned char) yytext[1];
			  return CHARACTERLIT;
			}

\'			{
			  error("illegal character constant in line %d",
			        lineNumber);
			}

false			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = 0;
			  return BOOLEANLIT;
			}

true			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = 1;
			  return BOOLEANLIT;
			}

\"(\\.|[^\n\\"])*\"	{
			  int n, i;
			  char *p, *q, c;
			  n = strlen(yytext) - 2;
			  p = yytext + 1;
			  q = (char *) allocate(n + 1);
			  yylval.stringVal.line = lineNumber;
			  yylval.stringVal.val = q;
			  for (i = 0; i < n; i++) {
			    c = *p++;
			    if (c == '\\') {
			      c = *p++;
			      i++;
			      switch (c) {
			        case 'n':
			          *q = '\n';
			          break;
			        case 'r':
			          *q = '\r';
			          break;
			        case 't':
			          *q = '\t';
			          break;
			        case 'b':
			          *q = '\b';
			          break;
			        case 'a':
			          *q = '\a';
			          break;
			        case '\'':
			          *q = '\'';
			          break;
			        case '\"':
			          *q = '\"';
			          break;
			        case '\\':
			          *q = '\\';
			          break;
			        default:
			          error("illegal escape sequence in line %d",
			                lineNumber);
			          break;
                              }
			    } else {
			      *q = c;
			    }
			    q++;
			  }
			  *q = '\0';
			  return STRINGLIT;
			}

\"			{
			  error("illegal string constant in line %d",
			        lineNumber);
			}

{ID}			{
			  yylval.stringVal.line = lineNumber;
			  yylval.stringVal.val =
			    (char *) allocate(strlen(yytext) + 1);
			  strcpy(yylval.stringVal.val, yytext);
			  return IDENT;
			}

.			{
			  error("illegal character 0x%02x in line %d",
			        (unsigned char) yytext[0], lineNumber);
			}


%%


int yywrap(void) {
  return 1;
}


void showToken(int token) {
  char *p, c;

  printf("TOKEN = ");
  switch (token) {
    case 0:
      printf("-- EOF --");
      break;
    case BREAK:
      printf("BREAK in line %d", yylval.noVal.line);
      break;
    case DO:
      printf("DO in line %d", yylval.noVal.line);
      break;
    case ELSE:
      printf("ELSE in line %d", yylval.noVal.line);
      break;
    case GLOBAL:
      printf("GLOBAL in line %d", yylval.noVal.line);
      break;
    case IF:
      printf("IF in line %d", yylval.noVal.line);
      break;
    case LOCAL:
      printf("LOCAL in line %d", yylval.noVal.line);
      break;
    case NEW:
      printf("NEW in line %d", yylval.noVal.line);
      break;
    case RECORD:
      printf("RECORD in line %d", yylval.noVal.line);
      break;
    case RETURN:
      printf("RETURN in line %d", yylval.noVal.line);
      break;
    case SIZEOF:
      printf("SIZEOF in line %d", yylval.noVal.line);
      break;
    case TYPE:
      printf("TYPE in line %d", yylval.noVal.line);
      break;
    case VOID:
      printf("VOID in line %d", yylval.noVal.line);
      break;
    case WHILE:
      printf("WHILE in line %d", yylval.noVal.line);
      break;
    case LPAREN:
      printf("LPAREN in line %d", yylval.noVal.line);
      break;
    case RPAREN:
      printf("RPAREN in line %d", yylval.noVal.line);
      break;
    case LCURL:
      printf("LCURL in line %d", yylval.noVal.line);
      break;
    case RCURL:
      printf("RCURL in line %d", yylval.noVal.line);
      break;
    case LBRACK:
      printf("LBRACK in line %d", yylval.noVal.line);
      break;
    case RBRACK:
      printf("RBRACK in line %d", yylval.noVal.line);
      break;
    case ASGN:
      printf("ASGN in line %d", yylval.noVal.line);
      break;
    case COMMA:
      printf("COMMA in line %d", yylval.noVal.line);
      break;
    case SEMIC:
      printf("SEMIC in line %d", yylval.noVal.line);
      break;
    case DOT:
      printf("DOT in line %d", yylval.noVal.line);
      break;
    case LOGOR:
      printf("LOGOR in line %d", yylval.noVal.line);
      break;
    case LOGAND:
      printf("LOGAND in line %d", yylval.noVal.line);
      break;
    case LOGNOT:
      printf("LOGNOT in line %d", yylval.noVal.line);
      break;
    case EQ:
      printf("EQ in line %d", yylval.noVal.line);
      break;
    case NE:
      printf("NE in line %d", yylval.noVal.line);
      break;
    case LT:
      printf("LT in line %d", yylval.noVal.line);
      break;
    case LE:
      printf("LE in line %d", yylval.noVal.line);
      break;
    case GT:
      printf("GT in line %d", yylval.noVal.line);
      break;
    case GE:
      printf("GE in line %d", yylval.noVal.line);
      break;
    case PLUS:
      printf("PLUS in line %d", yylval.noVal.line);
      break;
    case MINUS:
      printf("MINUS in line %d", yylval.noVal.line);
      break;
    case STAR:
      printf("STAR in line %d", yylval.noVal.line);
      break;
    case SLASH:
      printf("SLASH in line %d", yylval.noVal.line);
      break;
    case PERCENT:
      printf("PERCENT in line %d", yylval.noVal.line);
      break;
    case NIL:
      printf("NIL in line %d", yylval.noVal.line);
      break;
    case INTEGERLIT:
      printf("INTEGERLIT in line %d, value = %s",
             yylval.stringVal.line, yylval.stringVal.val);
      break;
    case CHARACTERLIT:
      printf("CHARACTERLIT in line %d, value = 0x%02X",
             yylval.intVal.line, yylval.intVal.val);
      break;
    case BOOLEANLIT:
      printf("BOOLEANLIT in line %d, value = %s",
             yylval.intVal.line, yylval.intVal.val ? "true" : "false");
      break;
    case STRINGLIT:
      printf("STRINGLIT in line %d, value = \"",
             yylval.stringVal.line);
      p = yylval.stringVal.val;
      while (*p != '\0') {
        c = *p++;
        switch (c) {
          case '\n':
            printf("\\n");
            break;
          case '\r':
            printf("\\r");
            break;
          case '\t':
            printf("\\t");
            break;
          case '\b':
            printf("\\b");
            break;
          case '\a':
            printf("\\a");
            break;
          case '\'':
            printf("\\\'");
            break;
          case '\"':
            printf("\\\"");
            break;
          case '\\':
            printf("\\\\");
            break;
          default:
            printf("%c", c);
            break;
        }
      }
      printf("\"");
      break;
    case IDENT:
      printf("IDENT in line %d, value = \"%s\"",
             yylval.stringVal.line, yylval.stringVal.val);
      break;
    default:
      /* this should never happen */
      error("unknown token %d in showToken", token);
  }
  printf("\n");
}
