/*
 * table.h -- symbol table
 */


#ifndef _TABLE_H_
#define _TABLE_H_


void enter(char *name, int value);
int lookup(char *name, int addr);
void checkUndefined(boolean map);


#endif /* _TABLE_H_ */
