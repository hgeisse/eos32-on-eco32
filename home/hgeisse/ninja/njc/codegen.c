/*
 * codegen.c -- code generator
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "absyn.h"
#include "types.h"
#include "table.h"
#include "codegen.h"


static FILE *asmFile;


/**************************************************************/


static int newLabel(void) {
  static int numLabels = 0;

  return numLabels++;
}


static void shouldNotReach(char *nodeName) {
  error("code generation should not reach '%s' node", nodeName);
}


/**************************************************************/


static void genCodeNode(Absyn *node, Table *table,
                        int returnLabel, int breakLabel);


static void genCodeVoidTy(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  shouldNotReach("VoidTy");
}


static void genCodeNameTy(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  shouldNotReach("NameTy");
}


static void genCodeArrayTy(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  shouldNotReach("ArrayTy");
}


static void genCodeRecordTy(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  shouldNotReach("RecordTy");
}


static void genCodeMembDec(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  shouldNotReach("MembDec");
}


static void genCodeTypeDec(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  /* nothing to generate here */
}


static void genCodeGvarDec(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  /* nothing to generate here */
}


static void genCodeFuncDec(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Entry *funcEntry;
  Table *localTable;
  int numLocals;
  int newReturnLabel;

  /* get symbol table entry for this function */
  funcEntry = lookup(table, node->u.funcDec.name);
  if (funcEntry == NULL || funcEntry->kind != ENTRY_KIND_FUNC) {
    error("function declaration vanished from symbol table");
  }
  localTable = funcEntry->u.funcEntry.localTable;
  numLocals = funcEntry->u.funcEntry.numLocals;
  /* generate function prolog */
  newReturnLabel = newLabel();
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// ");
  showType(asmFile, funcEntry->u.funcEntry.returnType);
  fprintf(asmFile, " %s", symToString(node->u.funcDec.name));
  showParamTypes(asmFile, funcEntry->u.funcEntry.paramTypes);
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_%s:\n", symToString(node->u.funcDec.name));
  fprintf(asmFile, "\tasf\t%d\n", numLocals);
  /* generate code for function body */
  genCodeNode(node->u.funcDec.body, localTable, newReturnLabel, -1);
  /* generate function epilog */
  fprintf(asmFile, "__%d:\n", newReturnLabel);
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
}


static void genCodeParDec(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  shouldNotReach("ParDec");
}


static void genCodeLvarDec(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  shouldNotReach("LvarDec");
}


static void genCodeEmptyStm(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  /* nothing to generate here */
}


static void genCodeCompStm(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  genCodeNode(node->u.compStm.stms, table, returnLabel, breakLabel);
}


static void genCodeAssignStm(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  Absyn *var;
  Entry *varEntry;
  boolean isStatic;
  int offset;

  var = node->u.assignStm.var;
  /* generate code for LHS */
  if (var->type == ABSYN_SIMPLEVAR) {
    /* lookup variable */
    varEntry = lookup(table, var->u.simpleVar.name);
    if (varEntry == NULL || varEntry->kind != ENTRY_KIND_VAR) {
      error("variable declaration vanished from symbol table");
    }
    isStatic = varEntry->u.varEntry.isStatic;
    offset = varEntry->u.varEntry.offset;
  } else
  if (var->type == ABSYN_ARRAYVAR) {
    /* generate code to compute array object */
    genCodeNode(var->u.arrayVar.array, table, returnLabel, breakLabel);
    /* generate code to compute index */
    genCodeNode(var->u.arrayVar.index, table, returnLabel, breakLabel);
  } else
  if (var->type == ABSYN_RECORDVAR) {
    /* generate code to compute record object */
    genCodeNode(var->u.recordVar.record, table, returnLabel, breakLabel);
    offset = var->u.recordVar.offset;
  } else {
    /* this should never happen */
    error("illegal AST detected in genCodeAssignStm");
  }
  /* generate code for RHS */
  genCodeNode(node->u.assignStm.exp, table, returnLabel, breakLabel);
  /* generate code for assignment */
  if (var->type == ABSYN_SIMPLEVAR) {
    if (isStatic) {
      fprintf(asmFile, "\tpopg\t%d\n", offset);
    } else {
      fprintf(asmFile, "\tpopl\t%d\n", offset);
    }
  } else
  if (var->type == ABSYN_ARRAYVAR) {
    fprintf(asmFile, "\tputfa\n");
  } else
  if (var->type == ABSYN_RECORDVAR) {
    fprintf(asmFile, "\tputf\t%d\n", offset);
  } else {
    /* this should never happen */
    error("illegal AST detected in genCodeAssignStm");
  }
}


static void genCodeIfStm1(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  int label1;

  label1 = newLabel();
  genCodeNode(node->u.ifStm1.test, table, returnLabel, breakLabel);
  fprintf(asmFile, "\tbrf\t__%d\n", label1);
  genCodeNode(node->u.ifStm1.thenPart, table, returnLabel, breakLabel);
  fprintf(asmFile, "__%d:\n", label1);
}


static void genCodeIfStm2(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  int label1, label2;

  label1 = newLabel();
  label2 = newLabel();
  genCodeNode(node->u.ifStm2.test, table, returnLabel, breakLabel);
  fprintf(asmFile, "\tbrf\t__%d\n", label1);
  genCodeNode(node->u.ifStm2.thenPart, table, returnLabel, breakLabel);
  fprintf(asmFile, "\tjmp\t__%d\n", label2);
  fprintf(asmFile, "__%d:\n", label1);
  genCodeNode(node->u.ifStm2.elsePart, table, returnLabel, breakLabel);
  fprintf(asmFile, "__%d:\n", label2);
}


static void genCodeWhileStm(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  int label1, label2, newBreakLabel;

  label1 = newLabel();
  label2 = newLabel();
  newBreakLabel = newLabel();
  fprintf(asmFile, "\tjmp\t__%d\n", label2);
  fprintf(asmFile, "__%d:\n", label1);
  genCodeNode(node->u.whileStm.body, table, returnLabel, newBreakLabel);
  fprintf(asmFile, "__%d:\n", label2);
  genCodeNode(node->u.whileStm.test, table, returnLabel, newBreakLabel);
  fprintf(asmFile, "\tbrt\t__%d\n", label1);
  fprintf(asmFile, "__%d:\n", newBreakLabel);
}


static void genCodeDoStm(Absyn *node, Table *table,
                         int returnLabel, int breakLabel) {
  int label1, newBreakLabel;

  label1 = newLabel();
  newBreakLabel = newLabel();
  fprintf(asmFile, "__%d:\n", label1);
  genCodeNode(node->u.doStm.body, table, returnLabel, newBreakLabel);
  genCodeNode(node->u.doStm.test, table, returnLabel, newBreakLabel);
  fprintf(asmFile, "\tbrt\t__%d\n", label1);
  fprintf(asmFile, "__%d:\n", newBreakLabel);
}


static void genCodeBreakStm(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  if (breakLabel == -1) {
    error("no valid break label in genCodeBreakStm");
  }
  fprintf(asmFile, "\tjmp\t__%d\n", breakLabel);
}


static void genCodeCallStm(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Entry *funcEntry;
  int numParams;

  /* get symbol table entry for the callee */
  funcEntry = lookup(table, node->u.callStm.name);
  if (funcEntry == NULL || funcEntry->kind != ENTRY_KIND_FUNC) {
    error("function declaration vanished from symbol table");
  }
  numParams = funcEntry->u.funcEntry.numParams;
  /* generate code to push args */
  genCodeNode(node->u.callExp.args, table, returnLabel, breakLabel);
  /* generate code for the call */
  fprintf(asmFile, "\tcall\t_%s\n", symToString(node->u.callStm.name));
  /* generate code to drop args */
  if (numParams > 0) {
    fprintf(asmFile, "\tdrop\t%d\n", numParams);
  }
}


static void genCodeRetStm(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  if (returnLabel == -1) {
    error("no valid return label in genCodeRetStm");
  }
  fprintf(asmFile, "\tjmp\t__%d\n", returnLabel);
}


static void genCodeRetExpStm(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  genCodeNode(node->u.retExpStm.exp, table, returnLabel, breakLabel);
  fprintf(asmFile, "\tpopr\n");
  if (returnLabel == -1) {
    error("no valid return label in genCodeRetExpStm");
  }
  fprintf(asmFile, "\tjmp\t__%d\n", returnLabel);
}


static void genCodeBinopExp(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  int label1;

  switch (node->u.binopExp.op) {
    case ABSYN_BINOP_LOR:
      label1 = newLabel();
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tdup\n");
      fprintf(asmFile, "\tbrt\t__%d\n", label1);
      fprintf(asmFile, "\tdrop\t1\n");
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "__%d:\n", label1);
      break;
    case ABSYN_BINOP_LAND:
      label1 = newLabel();
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tdup\n");
      fprintf(asmFile, "\tbrf\t__%d\n", label1);
      fprintf(asmFile, "\tdrop\t1\n");
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "__%d:\n", label1);
      break;
    case ABSYN_BINOP_EQ:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      if (node->u.binopExp.isRefComp) {
        fprintf(asmFile, "\trefeq\n");
      } else {
        fprintf(asmFile, "\teq\n");
      }
      break;
    case ABSYN_BINOP_NE:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      if (node->u.binopExp.isRefComp) {
        fprintf(asmFile, "\trefne\n");
      } else {
        fprintf(asmFile, "\tne\n");
      }
      break;
    case ABSYN_BINOP_LT:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tlt\n");
      break;
    case ABSYN_BINOP_LE:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tle\n");
      break;
    case ABSYN_BINOP_GT:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tgt\n");
      break;
    case ABSYN_BINOP_GE:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tge\n");
      break;
    case ABSYN_BINOP_ADD:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tadd\n");
      break;
    case ABSYN_BINOP_SUB:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tsub\n");
      break;
    case ABSYN_BINOP_MUL:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tmul\n");
      break;
    case ABSYN_BINOP_DIV:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tdiv\n");
      break;
    case ABSYN_BINOP_MOD:
      genCodeNode(node->u.binopExp.left, table, returnLabel, breakLabel);
      genCodeNode(node->u.binopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tmod\n");
      break;
    default:
      error("unknown binary operator %d in genCodeBinopExp",
            node->u.binopExp.op);
  }
}


static void genCodeUnopExp(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  switch (node->u.unopExp.op) {
    case ABSYN_UNOP_PLUS:
      genCodeNode(node->u.unopExp.right, table, returnLabel, breakLabel);
      break;
    case ABSYN_UNOP_MINUS:
      fprintf(asmFile, "\tpushc\t0\n");
      genCodeNode(node->u.unopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tsub\n");
      break;
    case ABSYN_UNOP_LNOT:
      fprintf(asmFile, "\tpushc\t1\n");
      genCodeNode(node->u.unopExp.right, table, returnLabel, breakLabel);
      fprintf(asmFile, "\tsub\n");
      break;
    default:
      error("unknown unary operator %d in genCodeUnopExp",
            node->u.unopExp.op);
  }
}


static void genCodeNilExp(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  fprintf(asmFile, "\tpushn\n");
}


static void genCodeIntExp(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  char *ptr;
  unsigned int base;
  unsigned int limit;
  unsigned int scale;
  unsigned int value;
  unsigned int digit;
  boolean firstRound;

  ptr = node->u.intExp.val;
  if (ptr[0] == '0' && ptr[1] == 'x') {
    ptr += 2;
    base = 16;
    limit = 16 * 16 * 16 * 16 * 16;
  } else {
    base = 10;
    limit = 10 * 10 * 10 * 10 * 10 * 10;
  }
  firstRound = TRUE;
  while (*ptr != '\0') {
    value = 0;
    scale = 1;
    while (*ptr != '\0') {
      digit = *ptr++;
      if (digit <= '9') {
        digit -= '0';
      } else {
        digit = (digit & ~0x20) - 'A' + 10;
      }
      value = value * base + digit;
      scale = scale * base;
      if (scale == limit) {
        break;
      }
    }
    if (firstRound) {
      fprintf(asmFile, "\tpushc\t%u\n", value);
      firstRound = FALSE;
    } else {
      fprintf(asmFile, "\tpushc\t%u\n", scale);
      fprintf(asmFile, "\tmul\n");
      fprintf(asmFile, "\tpushc\t%u\n", value);
      fprintf(asmFile, "\tadd\n");
    }
  }
}


static void genCodeCharExp(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  fprintf(asmFile, "\tpushc\t%d\n", node->u.charExp.val);
}


static void genCodeBoolExp(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  fprintf(asmFile, "\tpushc\t%d\n", node->u.boolExp.val);
}


static void genCodeStringExp(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  int n, i;
  char *p;

  n = strlen(node->u.stringExp.val);
  fprintf(asmFile, "\tpushc\t%d\n", n);
  fprintf(asmFile, "\tnewa\n");
  p = node->u.stringExp.val;
  for (i = 0; i < n; i++) {
    fprintf(asmFile, "\tdup\n");
    fprintf(asmFile, "\tpushc\t%d\n", i);
    fprintf(asmFile, "\tpushc\t%d\n", *p++);
    fprintf(asmFile, "\tputfa\n");
  }
}


static void genCodeNewExp(Absyn *node, Table *table,
                          int returnLabel, int breakLabel) {
  Absyn *ty;

  if (node->u.newExp.size < 0) {
    /* array instantiation */
    ty = node->u.newExp.ty;
    if (ty->type != ABSYN_ARRAYTY ||
        ty->u.arrayTy.nElems == NULL) {
      /* this should never happen */
      error("illegal AST detected by genCodeNewExp");
    }
    genCodeNode(ty->u.arrayTy.nElems, table, returnLabel, breakLabel);
    fprintf(asmFile, "\tnewa\n");
  } else {
    /* record instantiation */
    fprintf(asmFile, "\tnew\t%d\n", node->u.newExp.size);
  }
}


static void genCodeSizeofExp(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  genCodeNode(node->u.sizeofExp.exp, table, returnLabel, breakLabel);
  fprintf(asmFile, "\tgetsz\n");
}


static void genCodeCallExp(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Entry *funcEntry;
  int numParams;

  /* get symbol table entry for the callee */
  funcEntry = lookup(table, node->u.callExp.name);
  if (funcEntry == NULL || funcEntry->kind != ENTRY_KIND_FUNC) {
    error("function declaration vanished from symbol table");
  }
  numParams = funcEntry->u.funcEntry.numParams;
  /* generate code to push args */
  genCodeNode(node->u.callExp.args, table, returnLabel, breakLabel);
  /* generate code for the call */
  fprintf(asmFile, "\tcall\t_%s\n", symToString(node->u.callExp.name));
  /* generate code to drop args */
  if (numParams > 0) {
    fprintf(asmFile, "\tdrop\t%d\n", numParams);
  }
  /* the call leaves its result on the stack */
  fprintf(asmFile, "\tpushr\n");
}


static void genCodeSimpleVar(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  Entry *varEntry;
  int varOffset;

  /* lookup variable */
  varEntry = lookup(table, node->u.simpleVar.name);
  if (varEntry == NULL || varEntry->kind != ENTRY_KIND_VAR) {
    error("variable declaration vanished from symbol table");
  }
  varOffset = varEntry->u.varEntry.offset;
  /* generate code to push the variable */
  if (varEntry->u.varEntry.isStatic) {
    fprintf(asmFile, "\tpushg\t%d\n", varOffset);
  } else {
    fprintf(asmFile, "\tpushl\t%d\n", varOffset);
  }
}


static void genCodeArrayVar(Absyn *node, Table *table,
                            int returnLabel, int breakLabel) {
  /* generate code to push the array object */
  genCodeNode(node->u.arrayVar.array, table, returnLabel, breakLabel);
  /* generate code to push the index */
  genCodeNode(node->u.arrayVar.index, table, returnLabel, breakLabel);
  /* generate code to get the field of the array */
  fprintf(asmFile, "\tgetfa\n");
}


static void genCodeRecordVar(Absyn *node, Table *table,
                             int returnLabel, int breakLabel) {
  /* generate code to push the record object */
  genCodeNode(node->u.recordVar.record, table, returnLabel, breakLabel);
  /* generate code to get the field of the record */
  fprintf(asmFile, "\tgetf\t%d\n", node->u.recordVar.offset);
}


static void genCodeDecList(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Absyn *list;

  list = node;
  while (!list->u.decList.isEmpty) {
    genCodeNode(list->u.decList.head, table, returnLabel, breakLabel);
    list = list->u.decList.tail;
  }
}


static void genCodeStmList(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Absyn *list;

  list = node;
  while (!list->u.stmList.isEmpty) {
    genCodeNode(list->u.stmList.head, table, returnLabel, breakLabel);
    list = list->u.stmList.tail;
  }
}


static void genCodeExpList(Absyn *node, Table *table,
                           int returnLabel, int breakLabel) {
  Absyn *list;

  list = node;
  while (!list->u.expList.isEmpty) {
    genCodeNode(list->u.expList.head, table, returnLabel, breakLabel);
    list = list->u.expList.tail;
  }
}


static void genCodeNode(Absyn *node, Table *table,
                        int returnLabel, int breakLabel) {
  if (node == NULL) {
    error("genCodeNode got NULL node pointer");
  }
  switch (node->type) {
    case ABSYN_VOIDTY:
      genCodeVoidTy(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_NAMETY:
      genCodeNameTy(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_ARRAYTY:
      genCodeArrayTy(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_RECORDTY:
      genCodeRecordTy(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_MEMBDEC:
      genCodeMembDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_TYPEDEC:
      genCodeTypeDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_GVARDEC:
      genCodeGvarDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_FUNCDEC:
      genCodeFuncDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_PARDEC:
      genCodeParDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_LVARDEC:
      genCodeLvarDec(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_EMPTYSTM:
      genCodeEmptyStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_COMPSTM:
      genCodeCompStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_ASSIGNSTM:
      genCodeAssignStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_IFSTM1:
      genCodeIfStm1(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_IFSTM2:
      genCodeIfStm2(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_WHILESTM:
      genCodeWhileStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_DOSTM:
      genCodeDoStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_BREAKSTM:
      genCodeBreakStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_CALLSTM:
      genCodeCallStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_RETSTM:
      genCodeRetStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_RETEXPSTM:
      genCodeRetExpStm(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_BINOPEXP:
      genCodeBinopExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_UNOPEXP:
      genCodeUnopExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_NILEXP:
      genCodeNilExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_INTEXP:
      genCodeIntExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_CHAREXP:
      genCodeCharExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_BOOLEXP:
      genCodeBoolExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_STRINGEXP:
      genCodeStringExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_NEWEXP:
      genCodeNewExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_SIZEOFEXP:
      genCodeSizeofExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_CALLEXP:
      genCodeCallExp(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_SIMPLEVAR:
      genCodeSimpleVar(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_ARRAYVAR:
      genCodeArrayVar(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_RECORDVAR:
      genCodeRecordVar(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_DECLIST:
      genCodeDecList(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_STMLIST:
      genCodeStmList(node, table, returnLabel, breakLabel);
      break;
    case ABSYN_EXPLIST:
      genCodeExpList(node, table, returnLabel, breakLabel);
      break;
    default:
      /* this should never happen */
      error("unknown node type %d in genCodeNode", node->type);
  }
}


/**************************************************************/


static void genProlog(void) {
  /* version */
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// version\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "\t.vers\t%d\n", VERSION);
  /* execution framework */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// execution framework\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "__start:\n");
  fprintf(asmFile, "\tcall\t_main\n");
  fprintf(asmFile, "\tcall\t_exit\n");
  fprintf(asmFile, "__stop:\n");
  fprintf(asmFile, "\tjmp\t__stop\n");
  /* Integer readInteger() */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// Integer readInteger()\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_readInteger:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\trdint\n");
  fprintf(asmFile, "\tpopr\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* void writeInteger(Integer) */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// void writeInteger(Integer)\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_writeInteger:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\twrint\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* Character readCharacter() */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// Character readCharacter()\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_readCharacter:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\trdchr\n");
  fprintf(asmFile, "\tpopr\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* void writeCharacter(Character) */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// void writeCharacter(Character)\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_writeCharacter:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\twrchr\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* Integer char2int(Character) */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// Integer char2int(Character)\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_char2int:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\tpopr\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* Character int2char(Integer) */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// Character int2char(Integer)\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_int2char:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\tpopr\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* void exit() */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// void exit()\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_exit:\n");
  fprintf(asmFile, "\tasf\t0\n");
  fprintf(asmFile, "\thalt\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
  /* void writeString(String) */
  fprintf(asmFile, "\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "// void writeString(String)\n");
  fprintf(asmFile, "//\n");
  fprintf(asmFile, "_writeString:\n");
  fprintf(asmFile, "\tasf\t1\n");
  fprintf(asmFile, "\tpushc\t0\n");
  fprintf(asmFile, "\tpopl\t0\n");
  fprintf(asmFile, "\tjmp\t_writeString_L2\n");
  fprintf(asmFile, "_writeString_L1:\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\tpushl\t0\n");
  fprintf(asmFile, "\tgetfa\n");
  fprintf(asmFile, "\tcall\t_writeCharacter\n");
  fprintf(asmFile, "\tdrop\t1\n");
  fprintf(asmFile, "\tpushl\t0\n");
  fprintf(asmFile, "\tpushc\t1\n");
  fprintf(asmFile, "\tadd\n");
  fprintf(asmFile, "\tpopl\t0\n");
  fprintf(asmFile, "_writeString_L2:\n");
  fprintf(asmFile, "\tpushl\t0\n");
  fprintf(asmFile, "\tpushl\t-3\n");
  fprintf(asmFile, "\tgetsz\n");
  fprintf(asmFile, "\tlt\n");
  fprintf(asmFile, "\tbrt\t_writeString_L1\n");
  fprintf(asmFile, "\trsf\n");
  fprintf(asmFile, "\tret\n");
}


void genCode(Absyn *program, Table *globalTable, FILE *outFile) {
  asmFile = outFile;
  genProlog();
  genCodeNode(program, globalTable, -1, -1);
}
