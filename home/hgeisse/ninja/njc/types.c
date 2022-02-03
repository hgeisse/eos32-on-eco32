/*
 * types.c -- representation of types
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utils.h"
#include "sym.h"
#include "types.h"


Type *newPrimitiveType(char *printName) {
  Type *type;

  type = (Type *) allocate(sizeof(Type));
  type->kind = TYPE_KIND_PRIMITIVE;
  type->u.primitiveType.printName = printName;
  return type;
}


Type *newArrayType(Type *baseType, boolean sizeSpecified) {
  Type *type;

  type = (Type *) allocate(sizeof(Type));
  type->kind = TYPE_KIND_ARRAY;
  type->u.arrayType.baseType = baseType;
  type->u.arrayType.sizeSpecified = sizeSpecified;
  return type;
}


Type *newRecordType(MemberTypes *memberTypes) {
  Type *type;
  MemberTypes *p;
  int offset;

  type = (Type *) allocate(sizeof(Type));
  type->kind = TYPE_KIND_RECORD;
  type->u.recordType.memberTypes = memberTypes;
  p = memberTypes;
  offset = 0;
  while (!p->isEmpty) {
    p->offset = offset;
    p = p->next;
    offset++;
  }
  type->u.recordType.size = offset;
  return type;
}


Type *newNameType(Sym *name, int line) {
  Type *type;

  type = (Type *) allocate(sizeof(Type));
  type->kind = TYPE_KIND_NAME;
  type->u.nameType.name = name;
  type->u.nameType.line = line;
  type->u.nameType.resolvedType = NULL;
  return type;
}


MemberTypes *emptyMemberTypes(void) {
  MemberTypes *memberTypes;

  memberTypes = (MemberTypes *) allocate(sizeof(MemberTypes));
  memberTypes->isEmpty = TRUE;
  return memberTypes;
}


MemberTypes *newMemberTypes(Sym *name, Type *type, MemberTypes *next) {
  MemberTypes *memberTypes;

  memberTypes = (MemberTypes *) allocate(sizeof(MemberTypes));
  memberTypes->isEmpty = FALSE;
  memberTypes->name = name;
  memberTypes->type = type;
  memberTypes->next = next;
  return memberTypes;
}


ParamTypes *emptyParamTypes(void) {
  ParamTypes *paramTypes;

  paramTypes = (ParamTypes *) allocate(sizeof(ParamTypes));
  paramTypes->isEmpty = TRUE;
  return paramTypes;
}


ParamTypes *newParamTypes(Type *type, ParamTypes *next) {
  ParamTypes *paramTypes;

  paramTypes = (ParamTypes *) allocate(sizeof(ParamTypes));
  paramTypes->isEmpty = FALSE;
  paramTypes->type = type;
  paramTypes->next = next;
  return paramTypes;
}


void showType(FILE *stream, Type *type) {
  switch (type->kind) {
    case TYPE_KIND_PRIMITIVE:
      fprintf(stream, "%s", type->u.primitiveType.printName);
      break;
    case TYPE_KIND_ARRAY:
      showType(stream, type->u.arrayType.baseType);
      fprintf(stream, "[]");
      break;
    case TYPE_KIND_RECORD:
      showMemberTypes(stream, type->u.recordType.memberTypes);
      break;
    case TYPE_KIND_NAME:
      fprintf(stream, "%s", symToString(type->u.nameType.name));
      if (type->u.nameType.resolvedType == NULL) {
        fprintf(stream, "(*)");
      }
      break;
    default:
      error("unknown type kind %d in showType", type->kind);
      break;
  }
}


void showMemberTypes(FILE *stream, MemberTypes *memberTypes) {
  fprintf(stream, "record { ");
  while (!memberTypes->isEmpty) {
    showType(stream, memberTypes->type);
    fprintf(stream, " %s; ", symToString(memberTypes->name));
    memberTypes = memberTypes->next;
  }
  fprintf(stream, "}");
}


void showParamTypes(FILE *stream, ParamTypes *paramTypes) {
  fprintf(stream, "(");
  while (!paramTypes->isEmpty) {
    showType(stream, paramTypes->type);
    paramTypes = paramTypes->next;
    if (!paramTypes->isEmpty) {
      fprintf(stream, ", ");
    }
  }
  fprintf(stream, ")");
}
