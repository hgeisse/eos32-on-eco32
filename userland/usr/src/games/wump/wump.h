#ifndef __WUMP_H_INCLUDED__
#define __WUMP_H_INCLUDED__

/* defines */
#define	NBAT	3
#define	NROOM	20
#define	NTUNN	3
#define	NPIT	3

/* structs */
typedef struct {
    int	tunn[NTUNN];
    int	flag;
} room_t;

/* prototypes */
void main(void);
int tunnel(int i);
int rline(void);
int rnum(int n);
int rin(void);
int near(room_t *ap, int ahaz);
int icomp(const void *p1, const void *p2);

#endif
