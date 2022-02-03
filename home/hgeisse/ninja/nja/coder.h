/*
 * coder.h -- code generator
 */


#ifndef _CODE_H_
#define _CODE_H_


#define NJBF_VERSION	VERSION

#define OP_HALT		0
#define OP_PUSHC	1
#define OP_ADD		2
#define OP_SUB		3
#define OP_MUL		4
#define OP_DIV		5
#define OP_MOD		6
#define OP_RDINT	7
#define OP_WRINT	8
#define OP_RDCHR	9
#define OP_WRCHR	10
#define OP_PUSHG	11
#define OP_POPG		12
#define OP_ASF		13
#define OP_RSF		14
#define OP_PUSHL	15
#define OP_POPL		16
#define OP_EQ		17
#define OP_NE		18
#define OP_LT		19
#define OP_LE		20
#define OP_GT		21
#define OP_GE		22
#define OP_JMP		23
#define OP_BRF		24
#define OP_BRT		25
#define OP_CALL		26
#define OP_RET		27
#define OP_DROP		28
#define OP_PUSHR	29
#define OP_POPR		30
#define OP_DUP		31
#define OP_NEW		32
#define OP_GETF		33
#define OP_PUTF		34
#define OP_NEWA		35
#define OP_GETFA	36
#define OP_PUTFA	37
#define OP_GETSZ	38
#define OP_PUSHN	39
#define OP_REFEQ	40
#define OP_REFNE	41


void initCoder(void);
void exitCoder(char *codeFileName);
void code0(int opcode);
void code1(int opcode, int immed);
int getCurrAddr(void);
void patchCode(int where, int value);
void data(int addr);


#endif /* _CODE_H_ */
