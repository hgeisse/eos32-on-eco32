/*
 * reg.h -- layout of stored registers
 */


#ifndef _REG_H_
#define _REG_H_


/*
 * Location of the users' stored
 * registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
#define	R8	8
#define	R9	9
#define	R10	10
#define	R11	11
#define	R12	12
#define	R13	13
#define	R14	14
#define	R15	15
#define	R16	16
#define	R17	17
#define	R18	18
#define	R19	19
#define	R20	20
#define	R21	21
#define	R22	22
#define	R23	23
#define	R24	24
#define	R25	25
#define	R26	26
#define	R27	27
#define	R28	28
#define	R29	29
#define	R30	30
#define	R31	31

#define SP	29
#define	PC	30
#define	RPS	32

#define	TBIT	020		/* PS trace bit */


#endif /* _REG_H_ */
