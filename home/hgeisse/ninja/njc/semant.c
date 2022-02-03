/*
 * semant.c -- semantic analysis
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
#include "semant.h"


static boolean showLocalTables;

static Type *voidType;
static Type *nilType;
static Type *intType;
static Type *charType;
static Type *boolType;
static Type *stringType;

static int numGlobVars;


static void shouldNotReach(char *nodeName) {
  error("semantic check should not reach '%s' node", nodeName);
}


/**************************************************************/

/* type equality */


static boolean typeEq(Type *type1, Type *type2) {
  MemberTypes *p, *q;

  if (type1 == type2) {
    return TRUE;
  }
  if (type1 == voidType || type2 == voidType) {
    return FALSE;
  }
  if (type1 == nilType || type2 == nilType) {
    return TRUE;
  }
  if (type1->kind == TYPE_KIND_ARRAY &&
      type2->kind == TYPE_KIND_ARRAY) {
    return typeEq(type1->u.arrayType.baseType,
                  type2->u.arrayType.baseType);
  }
  if (type1->kind == TYPE_KIND_RECORD &&
      type2->kind == TYPE_KIND_RECORD) {
    p = type1->u.recordType.memberTypes;
    q = type2->u.recordType.memberTypes;
    while (!p->isEmpty && !q->isEmpty) {
      if (p->name != q->name || !typeEq(p->type, q->type)) {
        return FALSE;
      }
      p = p->next;
      q = q->next;
    }
    if (!p->isEmpty || !q->isEmpty) {
      return FALSE;
    }
    return TRUE;
  }
  if (type1->kind == TYPE_KIND_NAME) {
    if (type1->u.nameType.resolvedType == NULL) {
      /* this should never happen */
      error("unresolved name type in typeEq");
    }
    return typeEq(type1->u.nameType.resolvedType, type2);
  }
  if (type2->kind == TYPE_KIND_NAME) {
    if (type2->u.nameType.resolvedType == NULL) {
      /* this should never happen */
      error("unresolved name type in typeEq");
    }
    return typeEq(type1, type2->u.nameType.resolvedType);
  }
  return FALSE;
}


static boolean isRefType(Type *type) {
  if (type == voidType) {
    return FALSE;
  }
  if (type == nilType) {
    return TRUE;
  }
  if (type->kind == TYPE_KIND_PRIMITIVE) {
    return FALSE;
  }
  if (type->kind == TYPE_KIND_NAME) {
    if (type->u.nameType.resolvedType == NULL) {
      /* this should never happen */
      error("unresolved name type in isRefType");
    }
    return isRefType(type->u.nameType.resolvedType);
  }
  return TRUE;
}


/**************************************************************/

/* resolving named types */


void resolve(Type *type, Table *table) {
  MemberTypes *p;
  Entry *typeEntry;

  switch(type->kind) {
    case TYPE_KIND_PRIMITIVE:
      /* nothing to do here */
      break;
    case TYPE_KIND_ARRAY:
      resolve(type->u.arrayType.baseType, table);
      break;
    case TYPE_KIND_RECORD:
      p = type->u.recordType.memberTypes;
      while (!p->isEmpty) {
        resolve(p->type, table);
        p = p->next;
      }
      break;
    case TYPE_KIND_NAME:
      if (type->u.nameType.resolvedType == NULL) {
        /* not resolved yet */
        typeEntry = lookup(table, type->u.nameType.name);
        if (typeEntry == NULL) {
          error("undefined type '%s' in line %d",
                symToString(type->u.nameType.name),
                type->u.nameType.line);
        }
        if (typeEntry->kind != ENTRY_KIND_TYPE) {
          error("'%s' is not a type in line %d",
                symToString(type->u.nameType.name),
                type->u.nameType.line);
        }
        type->u.nameType.resolvedType = typeEntry->u.typeEntry.type;
      }
      break;
    default:
      error("illegal type kind %d in resolve", type->kind);
  }
}


Type *actualType(Type *type) {
  while (type->kind == TYPE_KIND_NAME) {
    if (type->u.nameType.resolvedType == NULL) {
      /* this should never happen */
      error("unresolved name type in actualType");
    }
    type = type->u.nameType.resolvedType;
  }
  return type;
}


/**************************************************************/


static Type *checkNode(Absyn *node, Table *table, int pass,
                       Type *rtype, boolean breakAllowed);


static Type *checkVoidTy(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *type;

  type = voidType;
  return type;
}


static Type *checkNameTy(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Entry *entry;
  Type *type;

  if (pass == 1) {
    type = newNameType(node->u.nameTy.name, node->line);
  } else
  if (pass == 3 || pass == 4) {
    entry = lookup(table, node->u.nameTy.name);
    if (entry == NULL) {
      error("undefined type '%s' in line %d",
            symToString(node->u.nameTy.name), node->line);
    }
    if (entry->kind != ENTRY_KIND_TYPE) {
      error("'%s' is not a type in line %d",
            symToString(node->u.nameTy.name), node->line);
    }
    type = entry->u.typeEntry.type;
  } else {
    error("illegal pass %d in checkNameTy", pass);
  }
  return type;
}


static Type *checkArrayTy(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *sizeType;
  Type *baseType;
  Type *type;
  boolean sizeSpecified;

  if (node->u.arrayTy.nElems != NULL) {
    sizeSpecified = TRUE;
    sizeType = checkNode(node->u.arrayTy.nElems, table, pass,
                         rtype, breakAllowed);
    if (!typeEq(sizeType, intType)) {
      error("array size must be an integer in line %d", node->line);
    }
  } else {
    sizeSpecified = FALSE;
  }
  baseType = checkNode(node->u.arrayTy.baseTy, table, pass,
                       rtype, breakAllowed);
  type = newArrayType(baseType, sizeSpecified);
  return type;
}


static MemberTypes *checkMembers(Absyn *node, Table *table, int pass,
                                 Type *rtype, boolean breakAllowed) {
  Absyn *memberDecs;
  Absyn *memberDec;
  Sym *memberName;
  Type *memberType;
  MemberTypes *memberTypes;
  MemberTypes *lastType;
  MemberTypes *newType;
  MemberTypes *p;

  memberDecs = node;
  memberTypes = NULL;
  while (!memberDecs->u.decList.isEmpty) {
    memberDec = memberDecs->u.decList.head;
    memberDecs = memberDecs->u.decList.tail;
    memberName = memberDec->u.membDec.name;
    /* determine member's type */
    memberType = checkNode(memberDec, table, pass, rtype, breakAllowed);
    /* check for multiple use of member's name */
    p = memberTypes;
    while (p != NULL) {
      if (p->name == memberName) {
        error("multiple definition of member name '%s' in line %d",
              symToString(memberName), memberDec->line);
      }
      p = p->next;
    }
    /* build a new type list element and link it to end of list */
    newType = newMemberTypes(memberName, memberType, NULL);
    if (memberTypes == NULL) {
      memberTypes = newType;
    } else {
      lastType->next = newType;
    }
    lastType = newType;
  }
  /* furnish a proper end of list */
  if (memberTypes == NULL) {
    memberTypes = emptyMemberTypes();
  } else {
    lastType->next = emptyMemberTypes();
  }
  return memberTypes;
}


static Type *checkRecordTy(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  MemberTypes *memberTypes;
  Type *type;

  memberTypes = checkMembers(node->u.recordTy.members, table, pass,
                             rtype, breakAllowed);
  type = newRecordType(memberTypes);
  return type;
}


static Type *checkMembDec(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.membDec.ty, table, pass,
                   rtype, breakAllowed);
  return type;
}


static Type *checkTypeDec(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;
  Entry *typeEntry;

  if (pass == 1) {
    type = checkNode(node->u.typeDec.ty, table, pass,
                     rtype, breakAllowed);
    typeEntry = newTypeEntry(type);
    if (enter(table, node->u.typeDec.name, typeEntry) == NULL) {
      error("redeclaration of '%s' in line %d",
            symToString(node->u.typeDec.name), node->line);
    }
  } else
  if (pass == 2) {
    /* get the type declaration entered in pass 1 again */
    typeEntry = lookup(table, node->u.typeDec.name);
    if (typeEntry == NULL || typeEntry->kind != ENTRY_KIND_TYPE) {
      error("type declaration vanished from symbol table");
    }
    type = typeEntry->u.typeEntry.type;
    resolve(type, table);
  } else
  if (pass == 3 || pass == 4) {
    /* nothing to do here */
  } else {
    error("illegal pass %d in checkTypeDec", pass);
  }
  return voidType;
}


static Type *checkGvarDec(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;
  Entry *globalEntry;

  if (pass == 1 || pass == 2) {
    /* nothing to do here */
  } else
  if (pass == 3) {
    type = checkNode(node->u.gvarDec.ty, table, pass,
                     rtype, breakAllowed);
    globalEntry = newVarEntry(type, TRUE, numGlobVars);
    if (enter(table, node->u.gvarDec.name, globalEntry) == NULL) {
      error("redeclaration of '%s' in line %d",
            symToString(node->u.gvarDec.name), node->line);
    }
    numGlobVars++;
  } else
  if (pass == 4) {
    /* nothing to do here */
  } else {
    error("illegal pass %d in checkGvarDec", pass);
  }
  return voidType;
}


static ParamTypes *checkParams(Absyn *node, Table *table, int pass,
                               Type *rtype, boolean breakAllowed) {
  Absyn *param;
  Absyn *next;
  Type *type;

  if (node->u.decList.isEmpty) {
    return emptyParamTypes();
  }
  param = node->u.decList.head;
  next = node->u.decList.tail;
  type = checkNode(param, table, pass,
                   rtype, breakAllowed);
  return newParamTypes(type,
                       checkParams(next, table, pass,
                                   rtype, breakAllowed));
}


static Type *checkFuncDec(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Table *localTable;
  Type *returnType;
  ParamTypes *paramTypes;
  Entry *funcEntry;
  /* params */
  Absyn *params;
  Absyn *param;
  Type *paramType;
  Entry *paramEntry;
  int numParams;
  int n;
  /* locals */
  Absyn *locals;
  Absyn *local;
  Type *localType;
  Entry *localEntry;
  int numLocals;

  if (pass == 1 || pass == 2) {
    /* nothing to do here */
  } else
  if (pass == 3) {
    /* build and enter func declaration into symbol table */
    returnType = checkNode(node->u.funcDec.rtype, table, pass,
                           rtype, breakAllowed);
    paramTypes = checkParams(node->u.funcDec.params, table, pass,
                             rtype, breakAllowed);
    localTable = newTable(table);
    funcEntry = newFuncEntry(returnType, paramTypes, localTable);
    if (enter(table, node->u.funcDec.name, funcEntry) == NULL) {
      error("redeclaration of '%s' in line %d",
            symToString(node->u.funcDec.name), node->line);
    }
    /* determine number of parameters */
    params = node->u.funcDec.params;
    numParams = 0;
    while (!params->u.decList.isEmpty) {
      params = params->u.decList.tail;
      numParams++;
    }
    funcEntry->u.funcEntry.numParams = numParams;
  } else
  if (pass == 4) {
    /* get the proc declaration entered in pass 1 again */
    funcEntry = lookup(table, node->u.funcDec.name);
    if (funcEntry == NULL || funcEntry->kind != ENTRY_KIND_FUNC) {
      error("procedure declaration vanished from symbol table");
    }
    localTable = funcEntry->u.funcEntry.localTable;
    numParams = funcEntry->u.funcEntry.numParams;
    /* enter parameters into local symbol table */
    params = node->u.funcDec.params;
    paramTypes = funcEntry->u.funcEntry.paramTypes;
    n = 0;
    while (!params->u.decList.isEmpty) {
      param = params->u.decList.head;
      params = params->u.decList.tail;
      paramType = paramTypes->type;
      paramTypes = paramTypes->next;
      paramEntry = newVarEntry(paramType, FALSE, n - 2 - numParams);
      if (enter(localTable, param->u.parDec.name, paramEntry) == NULL) {
        error("redeclaration of '%s' in line %d",
              symToString(param->u.parDec.name), node->line);
      }
      n++;
    }
    /* enter local declarations into local symbol table */
    locals = node->u.funcDec.locals;
    numLocals = 0;
    while (!locals->u.decList.isEmpty) {
      local = locals->u.decList.head;
      locals = locals->u.decList.tail;
      localType = checkNode(local, localTable, pass,
                            funcEntry->u.funcEntry.returnType, breakAllowed);
      localEntry = newVarEntry(localType, FALSE, numLocals);
      if (enter(localTable, local->u.lvarDec.name, localEntry) == NULL) {
        error("redeclaration of '%s' in line %d",
              symToString(param->u.lvarDec.name), node->line);
      }
      numLocals++;
    }
    funcEntry->u.funcEntry.numLocals = numLocals;
    /* do semantic check on procedure body */
    checkNode(node->u.funcDec.body, localTable, pass,
              funcEntry->u.funcEntry.returnType, breakAllowed);
    /* show symbol table if requested */
    if (showLocalTables) {
      printf("symbol table at end of procedure '%s':\n",
             symToString(node->u.funcDec.name));
      showTable(localTable);
    }
  } else {
    error("illegal pass %d in checkFuncDec", pass);
  }
  return voidType;
}


static Type *checkParDec(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.parDec.ty, table, pass,
                   rtype, breakAllowed);
  return type;
}


static Type *checkLvarDec(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.lvarDec.ty, table, pass,
                   rtype, breakAllowed);
  return type;
}


static Type *checkEmptyStm(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  return voidType;
}


static Type *checkCompStm(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  checkNode(node->u.compStm.stms, table, pass,
            rtype, breakAllowed);
  return voidType;
}


static Type *checkAssignStm(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Type *varType;
  Type *expType;

  varType = checkNode(node->u.assignStm.var, table, pass,
                      rtype, breakAllowed);
  expType = checkNode(node->u.assignStm.exp, table, pass,
                      rtype, breakAllowed);
  if (!typeEq(varType, expType)) {
    error("assignment has different LHS and RHS types in line %d",
          node->line);
  }
  return voidType;
}


static Type *checkIfStm1(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *testType;

  testType = checkNode(node->u.ifStm1.test, table, pass,
                       rtype, breakAllowed);
  if (!typeEq(testType, boolType)) {
    error("'if' test expression must be of type boolean in line %d",
          node->line);
  }
  checkNode(node->u.ifStm1.thenPart, table, pass,
            rtype, breakAllowed);
  return voidType;
}


static Type *checkIfStm2(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *testType;

  testType = checkNode(node->u.ifStm2.test, table, pass,
                       rtype, breakAllowed);
  if (!typeEq(testType, boolType)) {
    error("'if' test expression must be of type boolean in line %d",
          node->line);
  }
  checkNode(node->u.ifStm2.thenPart, table, pass,
            rtype, breakAllowed);
  checkNode(node->u.ifStm2.elsePart, table, pass,
            rtype, breakAllowed);
  return voidType;
}


static Type *checkWhileStm(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  Type *testType;

  testType = checkNode(node->u.whileStm.test, table, pass,
                       rtype, TRUE);
  if (!typeEq(testType, boolType)) {
    error("'while' test expression must be of type boolean in line %d",
          node->line);
  }
  checkNode(node->u.whileStm.body, table, pass,
            rtype, TRUE);
  return voidType;
}


static Type *checkDoStm(Absyn *node, Table *table, int pass,
                        Type *rtype, boolean breakAllowed) {
  Type *testType;

  testType = checkNode(node->u.doStm.test, table, pass,
                       rtype, TRUE);
  if (!typeEq(testType, boolType)) {
    error("'do' test expression must be of type boolean in line %d",
          node->line);
  }
  checkNode(node->u.doStm.body, table, pass,
            rtype, TRUE);
  return voidType;
}


static Type *checkBreakStm(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  if (!breakAllowed) {
    error("misplaced 'break' in line %d", node->line);
  }
  return voidType;
}


static Type *checkCallStm(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Entry *entry;
  ParamTypes *parameterTypes;
  Absyn *argumentExprs;
  Type *parameterType;
  Absyn *argumentExpr;
  Type *argumentType;
  int argNum;

  entry = lookup(table, node->u.callStm.name);
  if (entry == NULL) {
    error("undefined function '%s' in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  if (entry->kind != ENTRY_KIND_FUNC) {
    error("call of non-function '%s' in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  parameterTypes = entry->u.funcEntry.paramTypes;
  argumentExprs = node->u.callStm.args;
  argNum = 1;
  while (!parameterTypes->isEmpty && !argumentExprs->u.expList.isEmpty) {
    parameterType = parameterTypes->type;
    argumentExpr = argumentExprs->u.expList.head;
    argumentType = checkNode(argumentExpr, table, pass,
                             rtype, breakAllowed);
    if (!typeEq(parameterType, argumentType)) {
      error("procedure '%s' argument %d type mismatch in line %d",
            symToString(node->u.callStm.name), argNum, node->line);
    }
    parameterTypes = parameterTypes->next;
    argumentExprs = argumentExprs->u.expList.tail;
    argNum++;
  }
  if (!parameterTypes->isEmpty) {
    error("procedure '%s' called with too few arguments in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  if (!argumentExprs->u.expList.isEmpty) {
    error("procedure '%s' called with too many arguments in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  return voidType;
}


static Type *checkRetStm(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  if (rtype == NULL) {
    error("misplaced 'return' in line %d", node->line);
  }
  if (!typeEq(rtype, voidType)) {
    error("return in line %d must return a value",
          node->line);
  }
  return voidType;
}


static Type *checkRetExpStm(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.retExpStm.exp, table, pass,
                   rtype, breakAllowed);
  if (rtype == NULL) {
    error("misplaced 'return' in line %d", node->line);
  }
  if (typeEq(rtype, voidType)) {
    error("return in line %d must not return any value",
          node->line);
  }
  if (!typeEq(rtype, type)) {
    error("return type in line %d does not match function declaration",
          node->line);
  }
  return voidType;
}


static Type *checkBinopExp(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  Type *leftType;
  Type *rightType;
  Type *type;

  leftType = checkNode(node->u.binopExp.left, table, pass,
                       rtype, breakAllowed);
  rightType = checkNode(node->u.binopExp.right, table, pass,
                        rtype, breakAllowed);
  switch (node->u.binopExp.op) {
    case ABSYN_BINOP_LOR:
    case ABSYN_BINOP_LAND:
      if (!typeEq(leftType, boolType) || !typeEq(rightType, boolType)) {
        error("logical operation requires boolean operands in line %d",
              node->line);
      }
      type = boolType;
      break;
    case ABSYN_BINOP_EQ:
    case ABSYN_BINOP_NE:
      if (!typeEq(leftType, rightType)) {
        error("comparison operation requires same operand types in line %d",
              node->line);
      }
      if (isRefType(leftType) || isRefType(rightType)) {
        node->u.binopExp.isRefComp = TRUE;
      } else {
        node->u.binopExp.isRefComp = FALSE;
      }
      type = boolType;
      break;
    case ABSYN_BINOP_LT:
    case ABSYN_BINOP_LE:
    case ABSYN_BINOP_GT:
    case ABSYN_BINOP_GE:
      if ((!typeEq(leftType, intType) || !typeEq(rightType, intType)) &&
          (!typeEq(leftType, charType) || !typeEq(rightType, charType))) {
        error("comparison operation requires same ordered type in line %d",
              node->line);
      }
      type = boolType;
      break;
    case ABSYN_BINOP_ADD:
    case ABSYN_BINOP_SUB:
    case ABSYN_BINOP_MUL:
    case ABSYN_BINOP_DIV:
    case ABSYN_BINOP_MOD:
      if (!typeEq(leftType, intType) || !typeEq(rightType, intType)) {
        error("arithmetic operation requires integer operands in line %d",
              node->line);
      }
      type = intType;
      break;
    default:
      error("unknown binary operator %d in checkBinopExp",
            node->u.binopExp.op);
  }
  return type;
}


static Type *checkUnopExp(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *rightType;
  Type *type;

  rightType = checkNode(node->u.unopExp.right, table, pass,
                        rtype, breakAllowed);
  switch (node->u.unopExp.op) {
    case ABSYN_UNOP_PLUS:
    case ABSYN_UNOP_MINUS:
      if (!typeEq(rightType, intType)) {
        error("arithmetic operation requires integer operand in line %d",
              node->line);
      }
      type = intType;
      break;
    case ABSYN_UNOP_LNOT:
      if (!typeEq(rightType, boolType)) {
        error("logical operation requires boolean operand in line %d",
              node->line);
      }
      type = boolType;
      break;
    default:
      error("unknown unary operator %d in checkUnopExp",
            node->u.unopExp.op);
  }
  return type;
}


static Type *checkNilExp(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *type;

  type = nilType;
  return type;
}


static Type *checkIntExp(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *type;

  type = intType;
  return type;
}


static Type *checkCharExp(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;

  type = charType;
  return type;
}


static Type *checkBoolExp(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Type *type;

  type = boolType;
  return type;
}


static Type *checkStringExp(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Type *type;

  type = stringType;
  return type;
}


static Type *checkNewExp(Absyn *node, Table *table, int pass,
                         Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.newExp.ty, table, pass, rtype, breakAllowed);
  type = actualType(type);
  switch (type->kind) {
    case TYPE_KIND_PRIMITIVE:
      error("illegal object instantiation 'new(%s)' in line %d",
            type->u.primitiveType.printName, node->line);
      break;
    case TYPE_KIND_ARRAY:
      if (!type->u.arrayType.sizeSpecified) {
        error("illegal array instantiation without size in line %d",
              node->line);
      }
      node->u.newExp.size = -1;
      break;
    case TYPE_KIND_RECORD:
      node->u.newExp.size = type->u.recordType.size;
      break;
    default:
      /* this should never happen */
      error("unknown type kind %d in checkNewExp", type->kind);
  }
  return type;
}


static Type *checkSizeofExp(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Type *type;

  type = checkNode(node->u.sizeofExp.exp, table, pass,
                   rtype, breakAllowed);
  return intType;
}


static Type *checkCallExp(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Entry *entry;
  ParamTypes *parameterTypes;
  Absyn *argumentExprs;
  Type *parameterType;
  Absyn *argumentExpr;
  Type *argumentType;
  int argNum;

  entry = lookup(table, node->u.callStm.name);
  if (entry == NULL) {
    error("undefined function '%s' in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  if (entry->kind != ENTRY_KIND_FUNC) {
    error("call of non-function '%s' in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  parameterTypes = entry->u.funcEntry.paramTypes;
  argumentExprs = node->u.callStm.args;
  argNum = 1;
  while (!parameterTypes->isEmpty && !argumentExprs->u.expList.isEmpty) {
    parameterType = parameterTypes->type;
    argumentExpr = argumentExprs->u.expList.head;
    argumentType = checkNode(argumentExpr, table, pass,
                             rtype, breakAllowed);
    if (!typeEq(parameterType, argumentType)) {
      error("procedure '%s' argument %d type mismatch in line %d",
            symToString(node->u.callStm.name), argNum, node->line);
    }
    parameterTypes = parameterTypes->next;
    argumentExprs = argumentExprs->u.expList.tail;
    argNum++;
  }
  if (!parameterTypes->isEmpty) {
    error("procedure '%s' called with too few arguments in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  if (!argumentExprs->u.expList.isEmpty) {
    error("procedure '%s' called with too many arguments in line %d",
          symToString(node->u.callStm.name), node->line);
  }
  return entry->u.funcEntry.returnType;
}


static Type *checkSimpleVar(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Entry *entry;
  Type *type;

  entry = lookup(table, node->u.simpleVar.name);
  if (entry == NULL) {
    error("undefined variable '%s' in line %d",
          symToString(node->u.simpleVar.name), node->line);
  }
  if (entry->kind != ENTRY_KIND_VAR) {
    error("'%s' is not a variable in line %d",
          symToString(node->u.simpleVar.name), node->line);
  }
  type = entry->u.varEntry.type;
  return type;
}


static Type *checkArrayVar(Absyn *node, Table *table, int pass,
                           Type *rtype, boolean breakAllowed) {
  Type *arrayType;
  Type *indexType;
  Type *baseType;

  arrayType = checkNode(node->u.arrayVar.array, table, pass,
                        rtype, breakAllowed);
  arrayType = actualType(arrayType);
  if (arrayType->kind != TYPE_KIND_ARRAY) {
    error("illegal indexing of a non-array in line %d",
          node->line);
  }
  baseType = arrayType->u.arrayType.baseType;
  indexType = checkNode(node->u.arrayVar.index, table, pass,
                        rtype, breakAllowed);
  if (!typeEq(indexType, intType)) {
    error("illegal indexing with a non-integer in line %d",
          node->line);
  }
  return baseType;
}


static Type *checkRecordVar(Absyn *node, Table *table, int pass,
                            Type *rtype, boolean breakAllowed) {
  Type *recordType;
  MemberTypes *memberTypes;
  Sym *member;
  Type *type;

  recordType = checkNode(node->u.recordVar.record, table, pass,
                         rtype, breakAllowed);
  recordType = actualType(recordType);
  if (recordType->kind != TYPE_KIND_RECORD) {
    error("illegal member selection of a non-record in line %d",
          node->line);
  }
  memberTypes = recordType->u.recordType.memberTypes;
  member = node->u.recordVar.member;
  while (!memberTypes->isEmpty) {
    if (memberTypes->name == member) {
      break;
    }
    memberTypes = memberTypes->next;
  }
  if (memberTypes->isEmpty) {
    error("unknown record member '%s' in line %d",
          symToString(member), node->line);
  }
  node->u.recordVar.offset = memberTypes->offset;
  type = memberTypes->type;
  return type;
}


static Type *checkDecList(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Absyn *list;

  list = node;
  while (!list->u.decList.isEmpty) {
    checkNode(list->u.decList.head, table, pass,
              rtype, breakAllowed);
    list = list->u.decList.tail;
  }
  return voidType;
}


static Type *checkStmList(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Absyn *list;

  list = node;
  while (!list->u.stmList.isEmpty) {
    checkNode(list->u.stmList.head, table, pass,
              rtype, breakAllowed);
    list = list->u.stmList.tail;
  }
  return voidType;
}


static Type *checkExpList(Absyn *node, Table *table, int pass,
                          Type *rtype, boolean breakAllowed) {
  Absyn *list;

  list = node;
  while (!list->u.expList.isEmpty) {
    checkNode(list->u.expList.head, table, pass,
              rtype, breakAllowed);
    list = list->u.expList.tail;
  }
  return voidType;
}


static Type *checkNode(Absyn *node, Table *table, int pass,
                       Type *rtype, boolean breakAllowed) {
  if (node == NULL) {
    error("checkNode got NULL node pointer");
  }
  switch (node->type) {
    case ABSYN_VOIDTY:
      return checkVoidTy(node, table, pass, rtype, breakAllowed);
    case ABSYN_NAMETY:
      return checkNameTy(node, table, pass, rtype, breakAllowed);
    case ABSYN_ARRAYTY:
      return checkArrayTy(node, table, pass, rtype, breakAllowed);
    case ABSYN_RECORDTY:
      return checkRecordTy(node, table, pass, rtype, breakAllowed);
    case ABSYN_MEMBDEC:
      return checkMembDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_TYPEDEC:
      return checkTypeDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_GVARDEC:
      return checkGvarDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_FUNCDEC:
      return checkFuncDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_PARDEC:
      return checkParDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_LVARDEC:
      return checkLvarDec(node, table, pass, rtype, breakAllowed);
    case ABSYN_EMPTYSTM:
      return checkEmptyStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_COMPSTM:
      return checkCompStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_ASSIGNSTM:
      return checkAssignStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_IFSTM1:
      return checkIfStm1(node, table, pass, rtype, breakAllowed);
    case ABSYN_IFSTM2:
      return checkIfStm2(node, table, pass, rtype, breakAllowed);
    case ABSYN_WHILESTM:
      return checkWhileStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_DOSTM:
      return checkDoStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_BREAKSTM:
      return checkBreakStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_CALLSTM:
      return checkCallStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_RETSTM:
      return checkRetStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_RETEXPSTM:
      return checkRetExpStm(node, table, pass, rtype, breakAllowed);
    case ABSYN_BINOPEXP:
      return checkBinopExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_UNOPEXP:
      return checkUnopExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_NILEXP:
      return checkNilExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_INTEXP:
      return checkIntExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_CHAREXP:
      return checkCharExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_BOOLEXP:
      return checkBoolExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_STRINGEXP:
      return checkStringExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_NEWEXP:
      return checkNewExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_SIZEOFEXP:
      return checkSizeofExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_CALLEXP:
      return checkCallExp(node, table, pass, rtype, breakAllowed);
    case ABSYN_SIMPLEVAR:
      return checkSimpleVar(node, table, pass, rtype, breakAllowed);
    case ABSYN_ARRAYVAR:
      return checkArrayVar(node, table, pass, rtype, breakAllowed);
    case ABSYN_RECORDVAR:
      return checkRecordVar(node, table, pass, rtype, breakAllowed);
    case ABSYN_DECLIST:
      return checkDecList(node, table, pass, rtype, breakAllowed);
    case ABSYN_STMLIST:
      return checkStmList(node, table, pass, rtype, breakAllowed);
    case ABSYN_EXPLIST:
      return checkExpList(node, table, pass, rtype, breakAllowed);
    default:
      /* this should never happen */
      error("unknown node type %d in checkNode", node->type);
      /* not reached */
      return voidType;
  }
}


/**************************************************************/


static void enterBuiltinTypes(Table *table) {
  Entry *typeEntry;

  typeEntry = newTypeEntry(intType);
  enter(table, newSym("Integer"), typeEntry);
  typeEntry = newTypeEntry(charType);
  enter(table, newSym("Character"), typeEntry);
  typeEntry = newTypeEntry(boolType);
  enter(table, newSym("Boolean"), typeEntry);
  typeEntry = newTypeEntry(stringType);
  enter(table, newSym("String"), typeEntry);
}


static void enterLibraryFuncs(Table *table) {
  ParamTypes *paramTypes;
  Entry *funcEntry;

  /* Integer readInteger() */
  paramTypes = emptyParamTypes();
  funcEntry = newFuncEntry(intType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 0;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("readInteger"), funcEntry);

  /* void writeInteger(Integer) */
  paramTypes = newParamTypes(intType, emptyParamTypes());
  funcEntry = newFuncEntry(voidType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 1;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("writeInteger"), funcEntry);

  /* Character readCharacter() */
  paramTypes = emptyParamTypes();
  funcEntry = newFuncEntry(charType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 0;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("readCharacter"), funcEntry);

  /* void writeCharacter(Character) */
  paramTypes = newParamTypes(charType, emptyParamTypes());
  funcEntry = newFuncEntry(voidType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 1;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("writeCharacter"), funcEntry);

  /* Integer char2int(Character) */
  paramTypes = newParamTypes(charType, emptyParamTypes());
  funcEntry = newFuncEntry(intType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 1;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("char2int"), funcEntry);

  /* Character int2char(Integer) */
  paramTypes = newParamTypes(intType, emptyParamTypes());
  funcEntry = newFuncEntry(charType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 1;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("int2char"), funcEntry);

  /* void exit() */
  paramTypes = emptyParamTypes();
  funcEntry = newFuncEntry(voidType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 0;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("exit"), funcEntry);

  /* void writeString(String) */
  paramTypes = newParamTypes(stringType, emptyParamTypes());
  funcEntry = newFuncEntry(voidType, paramTypes, NULL);
  funcEntry->u.funcEntry.numParams = 1;
  funcEntry->u.funcEntry.numLocals = 0;
  enter(table, newSym("writeString"), funcEntry);
}


static void checkMain(Table *globalTable) {
  Entry *mainEntry;

  mainEntry = lookup(globalTable, newSym("main"));
  if (mainEntry == NULL) {
    error("procedure 'main' is missing");
  }
  if (mainEntry->kind != ENTRY_KIND_FUNC) {
    error("'main' is not a procedure");
  }
  if (!typeEq(mainEntry->u.funcEntry.returnType, voidType)) {
    error("procedure 'main' must not return a value");
  }
  if (!mainEntry->u.funcEntry.paramTypes->isEmpty) {
    error("procedure 'main' must not have any parameters");
  }
}


Table *check(Absyn *program, boolean showSymbolTables) {
  Table *globalTable;

  /* store global flag to show local symbol tables */
  showLocalTables = showSymbolTables;
  /* generate built-in types */
  voidType = newPrimitiveType("void");
  nilType = newPrimitiveType("nil");
  intType = newPrimitiveType("Integer");
  charType = newPrimitiveType("Character");
  boolType = newPrimitiveType("Boolean");
  stringType = newArrayType(charType, FALSE);
  /* setup global symbol table */
  globalTable = newTable(NULL);
  numGlobVars = 0;
  enterBuiltinTypes(globalTable);
  enterLibraryFuncs(globalTable);
  /* do semantic checks in 4 passes */
  checkNode(program, globalTable, 1, NULL, FALSE);
  checkNode(program, globalTable, 2, NULL, FALSE);
  checkNode(program, globalTable, 3, NULL, FALSE);
  checkNode(program, globalTable, 4, NULL, FALSE);
  /* check if "main()" is present */
  checkMain(globalTable);
  /* return global symbol table */
  return globalTable;
}
