/*
 * absyn.c -- abstract syntax
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "absyn.h"


/**************************************************************/


Absyn *newVoidTy(int line) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_VOIDTY;
  node->line = line;
  return node;
}


Absyn *newNameTy(int line, Sym *name) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_NAMETY;
  node->line = line;
  node->u.nameTy.name = name;
  return node;
}


Absyn *newArrayTy(int line, Absyn *nElems, Absyn *baseTy) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_ARRAYTY;
  node->line = line;
  node->u.arrayTy.nElems = nElems;
  node->u.arrayTy.baseTy = baseTy;
  return node;
}


Absyn *newRecordTy(int line, Absyn *members) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_RECORDTY;
  node->line = line;
  node->u.recordTy.members = members;
  return node;
}


Absyn *newMembDec(int line, Sym *name, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_MEMBDEC;
  node->line = line;
  node->u.membDec.name = name;
  node->u.membDec.ty = ty;
  return node;
}


Absyn *newTypeDec(int line, Sym *name, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_TYPEDEC;
  node->line = line;
  node->u.typeDec.name = name;
  node->u.typeDec.ty = ty;
  return node;
}


Absyn *newGvarDec(int line, Sym *name, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_GVARDEC;
  node->line = line;
  node->u.gvarDec.name = name;
  node->u.gvarDec.ty = ty;
  return node;
}


Absyn *newFuncDec(int line, Sym *name, Absyn *rtype, Absyn *params,
                  Absyn *locals, Absyn *body) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_FUNCDEC;
  node->line = line;
  node->u.funcDec.name = name;
  node->u.funcDec.rtype = rtype;
  node->u.funcDec.params = params;
  node->u.funcDec.locals = locals;
  node->u.funcDec.body = body;
  return node;
}


Absyn *newParDec(int line, Sym *name, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_PARDEC;
  node->line = line;
  node->u.parDec.name = name;
  node->u.parDec.ty = ty;
  return node;
}


Absyn *newLvarDec(int line, Sym *name, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_LVARDEC;
  node->line = line;
  node->u.lvarDec.name = name;
  node->u.lvarDec.ty = ty;
  return node;
}


Absyn *newEmptyStm(int line) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_EMPTYSTM;
  node->line = line;
  return node;
}


Absyn *newCompStm(int line, Absyn *stms) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_COMPSTM;
  node->line = line;
  node->u.compStm.stms = stms;
  return node;
}


Absyn *newAssignStm(int line, Absyn *var, Absyn *exp) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_ASSIGNSTM;
  node->line = line;
  node->u.assignStm.var = var;
  node->u.assignStm.exp = exp;
  return node;
}


Absyn *newIfStm1(int line, Absyn *test, Absyn *thenPart) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_IFSTM1;
  node->line = line;
  node->u.ifStm1.test = test;
  node->u.ifStm1.thenPart = thenPart;
  return node;
}


Absyn *newIfStm2(int line, Absyn *test, Absyn *thenPart, Absyn *elsePart) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_IFSTM2;
  node->line = line;
  node->u.ifStm2.test = test;
  node->u.ifStm2.thenPart = thenPart;
  node->u.ifStm2.elsePart = elsePart;
  return node;
}


Absyn *newWhileStm(int line, Absyn *test, Absyn *body) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_WHILESTM;
  node->line = line;
  node->u.whileStm.test = test;
  node->u.whileStm.body = body;
  return node;
}


Absyn *newDoStm(int line, Absyn *test, Absyn *body) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_DOSTM;
  node->line = line;
  node->u.doStm.test = test;
  node->u.doStm.body = body;
  return node;
}


Absyn *newBreakStm(int line) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_BREAKSTM;
  node->line = line;
  return node;
}


Absyn *newCallStm(int line, Sym *name, Absyn *args) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_CALLSTM;
  node->line = line;
  node->u.callStm.name = name;
  node->u.callStm.args = args;
  return node;
}


Absyn *newRetStm(int line) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_RETSTM;
  node->line = line;
  return node;
}


Absyn *newRetExpStm(int line, Absyn *exp) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_RETEXPSTM;
  node->line = line;
  node->u.retExpStm.exp = exp;
  return node;
}


Absyn *newBinopExp(int line, int op, Absyn *left, Absyn *right) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_BINOPEXP;
  node->line = line;
  node->u.binopExp.op = op;
  node->u.binopExp.left = left;
  node->u.binopExp.right = right;
  return node;
}


Absyn *newUnopExp(int line, int op, Absyn *right) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_UNOPEXP;
  node->line = line;
  node->u.unopExp.op = op;
  node->u.unopExp.right = right;
  return node;
}


Absyn *newNilExp(int line) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_NILEXP;
  node->line = line;
  return node;
}


Absyn *newIntExp(int line, char *val) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_INTEXP;
  node->line = line;
  node->u.intExp.val = val;
  return node;
}


Absyn *newCharExp(int line, int val) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_CHAREXP;
  node->line = line;
  node->u.charExp.val = val;
  return node;
}


Absyn *newBoolExp(int line, int val) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_BOOLEXP;
  node->line = line;
  node->u.boolExp.val = val;
  return node;
}


Absyn *newStringExp(int line, char *val) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_STRINGEXP;
  node->line = line;
  node->u.stringExp.val = val;
  return node;
}


Absyn *newNewExp(int line, Absyn *ty) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_NEWEXP;
  node->line = line;
  node->u.newExp.ty = ty;
  return node;
}


Absyn *newSizeofExp(int line, Absyn *exp) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_SIZEOFEXP;
  node->line = line;
  node->u.sizeofExp.exp = exp;
  return node;
}


Absyn *newCallExp(int line, Sym *name, Absyn *args) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_CALLEXP;
  node->line = line;
  node->u.callExp.name = name;
  node->u.callExp.args = args;
  return node;
}


Absyn *newSimpleVar(int line, Sym *name) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_SIMPLEVAR;
  node->line = line;
  node->u.simpleVar.name = name;
  return node;
}


Absyn *newArrayVar(int line, Absyn *array, Absyn *index) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_ARRAYVAR;
  node->line = line;
  node->u.arrayVar.array = array;
  node->u.arrayVar.index = index;
  return node;
}


Absyn *newRecordVar(int line, Absyn *record, Sym *member) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_RECORDVAR;
  node->line = line;
  node->u.recordVar.record = record;
  node->u.recordVar.member = member;
  return node;
}


Absyn *emptyDecList(void) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_DECLIST;
  node->line = -1;
  node->u.decList.isEmpty = TRUE;
  return node;
}


Absyn *newDecList(Absyn *head, Absyn *tail) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_DECLIST;
  node->line = -1;
  node->u.decList.isEmpty = FALSE;
  node->u.decList.head = head;
  node->u.decList.tail = tail;
  return node;
}


Absyn *emptyStmList(void) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_STMLIST;
  node->line = -1;
  node->u.stmList.isEmpty = TRUE;
  return node;
}


Absyn *newStmList(Absyn *head, Absyn *tail) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_STMLIST;
  node->line = -1;
  node->u.stmList.isEmpty = FALSE;
  node->u.stmList.head = head;
  node->u.stmList.tail = tail;
  return node;
}


Absyn *emptyExpList(void) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_EXPLIST;
  node->line = -1;
  node->u.expList.isEmpty = TRUE;
  return node;
}


Absyn *newExpList(Absyn *head, Absyn *tail) {
  Absyn *node;

  node = (Absyn *) allocate(sizeof(Absyn));
  node->type = ABSYN_EXPLIST;
  node->line = -1;
  node->u.expList.isEmpty = FALSE;
  node->u.expList.head = head;
  node->u.expList.tail = tail;
  return node;
}


/**************************************************************/


static void indent(int n) {
  int i;

  for (i = 0; i < n; i++) {
    printf("  ");
  }
}


static void say(char *s) {
  printf("%s", s);
}


static void sayInt(int i) {
  printf("%d", i);
}


static void sayCharHex(int c) {
  printf("0x%02X", c);
}


static void sayCharLit(int c) {
  printf("%c", c);
}


static void sayBool(int b) {
  if (b) {
    printf("true");
  } else {
    printf("false");
  }
}


static void showNode(Absyn *node, int indent);


static void showVoidTy(Absyn *node, int n) {
  indent(n);
  say("VoidTy()");
}


static void showNameTy(Absyn *node, int n) {
  indent(n);
  say("NameTy(");
  say(symToString(node->u.nameTy.name));
  say(")");
}


static void showArrayTy(Absyn *node, int n) {
  indent(n);
  say("ArrayTy(\n");
  if (node->u.arrayTy.nElems == NULL) {
    indent(n + 1);
    say("<no size>");
  } else {
    showNode(node->u.arrayTy.nElems, n + 1);
  }
  say(",\n");
  showNode(node->u.arrayTy.baseTy, n + 1);
  say(")");
}


static void showRecordTy(Absyn *node, int n) {
  indent(n);
  say("RecordTy(\n");
  showNode(node->u.recordTy.members, n + 1);
  say(")");
}


static void showMembDec(Absyn *node, int n) {
  indent(n);
  say("MembDec(\n");
  indent(n + 1);
  say(symToString(node->u.membDec.name));
  say(",\n");
  showNode(node->u.membDec.ty, n + 1);
  say(")");
}


static void showTypeDec(Absyn *node, int n) {
  indent(n);
  say("TypeDec(\n");
  indent(n + 1);
  say(symToString(node->u.typeDec.name));
  say(",\n");
  showNode(node->u.typeDec.ty, n + 1);
  say(")");
}


static void showGvarDec(Absyn *node, int n) {
  indent(n);
  say("GvarDec(\n");
  indent(n + 1);
  say(symToString(node->u.gvarDec.name));
  say(",\n");
  showNode(node->u.gvarDec.ty, n + 1);
  say(")");
}


static void showFuncDec(Absyn *node, int n) {
  indent(n);
  say("FuncDec(\n");
  indent(n + 1);
  say(symToString(node->u.funcDec.name));
  say(",\n");
  showNode(node->u.funcDec.rtype, n + 1);
  say(",\n");
  showNode(node->u.funcDec.params, n + 1);
  say(",\n");
  showNode(node->u.funcDec.locals, n + 1);
  say(",\n");
  showNode(node->u.funcDec.body, n + 1);
  say(")");
}


static void showParDec(Absyn *node, int n) {
  indent(n);
  say("ParDec(\n");
  indent(n + 1);
  say(symToString(node->u.parDec.name));
  say(",\n");
  showNode(node->u.parDec.ty, n + 1);
  say(")");
}


static void showLvarDec(Absyn *node, int n) {
  indent(n);
  say("LvarDec(\n");
  indent(n + 1);
  say(symToString(node->u.lvarDec.name));
  say(",\n");
  showNode(node->u.lvarDec.ty, n + 1);
  say(")");
}


static void showEmptyStm(Absyn *node, int n) {
  indent(n);
  say("EmptyStm()");
}


static void showCompStm(Absyn *node, int n) {
  indent(n);
  say("CompStm(\n");
  showNode(node->u.compStm.stms, n + 1);
  say(")");
}


static void showAssignStm(Absyn *node, int n) {
  indent(n);
  say("AssignStm(\n");
  showNode(node->u.assignStm.var, n + 1);
  say(",\n");
  showNode(node->u.assignStm.exp, n + 1);
  say(")");
}


static void showIfStm1(Absyn *node, int n) {
  indent(n);
  say("IfStm1(\n");
  showNode(node->u.ifStm1.test, n + 1);
  say(",\n");
  showNode(node->u.ifStm1.thenPart, n + 1);
  say(")");
}


static void showIfStm2(Absyn *node, int n) {
  indent(n);
  say("IfStm2(\n");
  showNode(node->u.ifStm2.test, n + 1);
  say(",\n");
  showNode(node->u.ifStm2.thenPart, n + 1);
  say(",\n");
  showNode(node->u.ifStm2.elsePart, n + 1);
  say(")");
}


static void showWhileStm(Absyn *node, int n) {
  indent(n);
  say("WhileStm(\n");
  showNode(node->u.whileStm.test, n + 1);
  say(",\n");
  showNode(node->u.whileStm.body, n + 1);
  say(")");
}


static void showDoStm(Absyn *node, int n) {
  indent(n);
  say("DoStm(\n");
  showNode(node->u.doStm.test, n + 1);
  say(",\n");
  showNode(node->u.doStm.body, n + 1);
  say(")");
}


static void showBreakStm(Absyn *node, int n) {
  indent(n);
  say("BreakStm()");
}


static void showCallStm(Absyn *node, int n) {
  indent(n);
  say("CallStm(\n");
  indent(n + 1);
  say(symToString(node->u.callStm.name));
  say(",\n");
  showNode(node->u.callStm.args, n + 1);
  say(")");
}


static void showRetStm(Absyn *node, int n) {
  indent(n);
  say("RetStm()");
}


static void showRetExpStm(Absyn *node, int n) {
  indent(n);
  say("RetExpStm(\n");
  showNode(node->u.retExpStm.exp, n + 1);
  say(")");
}


static void showBinopExp(Absyn *node, int n) {
  indent(n);
  say("BinopExp(\n");
  indent(n + 1);
  switch (node->u.binopExp.op) {
    case ABSYN_BINOP_LOR:
      say("LOR");
      break;
    case ABSYN_BINOP_LAND:
      say("LAND");
      break;
    case ABSYN_BINOP_EQ:
      say("EQ");
      break;
    case ABSYN_BINOP_NE:
      say("NE");
      break;
    case ABSYN_BINOP_LT:
      say("LT");
      break;
    case ABSYN_BINOP_LE:
      say("LE");
      break;
    case ABSYN_BINOP_GT:
      say("GT");
      break;
    case ABSYN_BINOP_GE:
      say("GE");
      break;
    case ABSYN_BINOP_ADD:
      say("ADD");
      break;
    case ABSYN_BINOP_SUB:
      say("SUB");
      break;
    case ABSYN_BINOP_MUL:
      say("MUL");
      break;
    case ABSYN_BINOP_DIV:
      say("DIV");
      break;
    case ABSYN_BINOP_MOD:
      say("MOD");
      break;
    default:
      error("unknown operator %d in showBinopExp", node->u.binopExp.op);
  }
  say(",\n");
  showNode(node->u.binopExp.left, n + 1);
  say(",\n");
  showNode(node->u.binopExp.right, n + 1);
  say(")");
}


static void showUnopExp(Absyn *node, int n) {
  indent(n);
  say("UnopExp(\n");
  indent(n + 1);
  switch (node->u.unopExp.op) {
    case ABSYN_UNOP_PLUS:
      say("PLUS");
      break;
    case ABSYN_UNOP_MINUS:
      say("MINUS");
      break;
    case ABSYN_UNOP_LNOT:
      say("LNOT");
      break;
    default:
      error("unknown operator %d in showUnopExp", node->u.unopExp.op);
  }
  say(",\n");
  showNode(node->u.unopExp.right, n + 1);
  say(")");
}


static void showNilExp(Absyn *node, int n) {
  indent(n);
  say("NilExp()");
}


static void showIntExp(Absyn *node, int n) {
  indent(n);
  say("IntExp(");
  say(node->u.intExp.val);
  say(")");
}


static void showCharExp(Absyn *node, int n) {
  indent(n);
  say("CharExp(");
  sayCharHex(node->u.charExp.val);
  say(")");
}


static void showBoolExp(Absyn *node, int n) {
  indent(n);
  say("BoolExp(");
  sayBool(node->u.boolExp.val);
  say(")");
}


static void showStringExp(Absyn *node, int n) {
  char *p, c;

  indent(n);
  say("StringExp(\"");
  p = node->u.stringExp.val;
  while (*p != '\0') {
    c = *p++;
    switch (c) {
      case '\n':
        say("\\n");
        break;
      case '\r':
        say("\\r");
        break;
      case '\t':
        say("\\t");
        break;
      case '\'':
        say("\\\'");
        break;
      case '\"':
        say("\\\"");
        break;
      case '\\':
        say("\\\\");
        break;
      default:
        sayCharLit(c);
        break;
    }
  }
  say("\")");
}


static void showNewExp(Absyn *node, int n) {
  indent(n);
  say("NewExp(\n");
  showNode(node->u.newExp.ty, n + 1);
  say(")");
}


static void showSizeofExp(Absyn *node, int n) {
  indent(n);
  say("SizeofExp(\n");
  showNode(node->u.sizeofExp.exp, n + 1);
  say(")");
}


static void showCallExp(Absyn *node, int n) {
  indent(n);
  say("CallExp(\n");
  indent(n + 1);
  say(symToString(node->u.callExp.name));
  say(",\n");
  showNode(node->u.callExp.args, n + 1);
  say(")");
}


static void showSimpleVar(Absyn *node, int n) {
  indent(n);
  say("SimpleVar(");
  say(symToString(node->u.simpleVar.name));
  say(")");
}


static void showArrayVar(Absyn *node, int n) {
  indent(n);
  say("ArrayVar(\n");
  showNode(node->u.arrayVar.array, n + 1);
  say(",\n");
  showNode(node->u.arrayVar.index, n + 1);
  say(")");
}


static void showRecordVar(Absyn *node, int n) {
  indent(n);
  say("RecordVar(\n");
  showNode(node->u.recordVar.record, n + 1);
  say(",\n");
  indent(n + 1);
  say(symToString(node->u.recordVar.member));
  say(")");
}


static void showDecList(Absyn *node, int n) {
  indent(n);
  say("DecList(");
  while (!node->u.decList.isEmpty) {
    say("\n");
    showNode(node->u.decList.head, n + 1);
    node = node->u.decList.tail;
    if (!node->u.decList.isEmpty) {
      say(",");
    }
  }
  say(")");
}


static void showStmList(Absyn *node, int n) {
  indent(n);
  say("StmList(");
  while (!node->u.stmList.isEmpty) {
    say("\n");
    showNode(node->u.stmList.head, n + 1);
    node = node->u.stmList.tail;
    if (!node->u.stmList.isEmpty) {
      say(",");
    }
  }
  say(")");
}


static void showExpList(Absyn *node, int n) {
  indent(n);
  say("ExpList(");
  while (!node->u.expList.isEmpty) {
    say("\n");
    showNode(node->u.expList.head, n + 1);
    node = node->u.expList.tail;
    if (!node->u.expList.isEmpty) {
      say(",");
    }
  }
  say(")");
}


static void showNode(Absyn *node, int indent) {
  if (node == NULL) {
    error("showNode got NULL node pointer");
  }
  switch (node->type) {
    case ABSYN_VOIDTY:
      showVoidTy(node, indent);
      break;
    case ABSYN_NAMETY:
      showNameTy(node, indent);
      break;
    case ABSYN_ARRAYTY:
      showArrayTy(node, indent);
      break;
    case ABSYN_RECORDTY:
      showRecordTy(node, indent);
      break;
    case ABSYN_MEMBDEC:
      showMembDec(node, indent);
      break;
    case ABSYN_TYPEDEC:
      showTypeDec(node, indent);
      break;
    case ABSYN_GVARDEC:
      showGvarDec(node, indent);
      break;
    case ABSYN_FUNCDEC:
      showFuncDec(node, indent);
      break;
    case ABSYN_PARDEC:
      showParDec(node, indent);
      break;
    case ABSYN_LVARDEC:
      showLvarDec(node, indent);
      break;
    case ABSYN_EMPTYSTM:
      showEmptyStm(node, indent);
      break;
    case ABSYN_COMPSTM:
      showCompStm(node, indent);
      break;
    case ABSYN_ASSIGNSTM:
      showAssignStm(node, indent);
      break;
    case ABSYN_IFSTM1:
      showIfStm1(node, indent);
      break;
    case ABSYN_IFSTM2:
      showIfStm2(node, indent);
      break;
    case ABSYN_WHILESTM:
      showWhileStm(node, indent);
      break;
    case ABSYN_DOSTM:
      showDoStm(node, indent);
      break;
    case ABSYN_BREAKSTM:
      showBreakStm(node, indent);
      break;
    case ABSYN_CALLSTM:
      showCallStm(node, indent);
      break;
    case ABSYN_RETSTM:
      showRetStm(node, indent);
      break;
    case ABSYN_RETEXPSTM:
      showRetExpStm(node, indent);
      break;
    case ABSYN_BINOPEXP:
      showBinopExp(node, indent);
      break;
    case ABSYN_UNOPEXP:
      showUnopExp(node, indent);
      break;
    case ABSYN_NILEXP:
      showNilExp(node, indent);
      break;
    case ABSYN_INTEXP:
      showIntExp(node, indent);
      break;
    case ABSYN_CHAREXP:
      showCharExp(node, indent);
      break;
    case ABSYN_BOOLEXP:
      showBoolExp(node, indent);
      break;
    case ABSYN_STRINGEXP:
      showStringExp(node, indent);
      break;
    case ABSYN_NEWEXP:
      showNewExp(node, indent);
      break;
    case ABSYN_SIZEOFEXP:
      showSizeofExp(node, indent);
      break;
    case ABSYN_CALLEXP:
      showCallExp(node, indent);
      break;
    case ABSYN_SIMPLEVAR:
      showSimpleVar(node, indent);
      break;
    case ABSYN_ARRAYVAR:
      showArrayVar(node, indent);
      break;
    case ABSYN_RECORDVAR:
      showRecordVar(node, indent);
      break;
    case ABSYN_DECLIST:
      showDecList(node, indent);
      break;
    case ABSYN_STMLIST:
      showStmList(node, indent);
      break;
    case ABSYN_EXPLIST:
      showExpList(node, indent);
      break;
    default:
      /* this should never happen */
      error("unknown node type %d in showNode", node->type);
  }
}


void showAbsyn(Absyn *node) {
  showNode(node, 0);
  printf("\n");
}
