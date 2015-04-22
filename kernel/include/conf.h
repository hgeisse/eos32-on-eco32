/*
 * conf.h -- structure of the three switch tables
 */

#ifndef _CONF_H_
#define _CONF_H_

/*
 * Declaration of block device
 * switch. Each entry (row) is
 * the only link between the
 * main unix code and the driver.
 * The initialization of the
 * device switches is in the
 * file conf.c.
 */
extern struct bdevsw {
	int	(*d_size)(dev_t);
	void	(*d_open)(dev_t, int);
	void	(*d_close)(dev_t, int);
	void	(*d_strategy)(struct buf *);
	struct buf *d_tab;
} bdevsw[];

/*
 * Character device switch.
 */
extern struct cdevsw {
	void	(*d_open)(dev_t, int);
	void	(*d_close)(dev_t, int);
	void	(*d_read)(dev_t);
	void	(*d_write)(dev_t);
	void	(*d_ioctl)(dev_t, int, caddr_t, int);
	/* void	(*d_stop)(struct tty *); */
} cdevsw[];

/*
 * tty line control switch.
 */
extern struct linesw {
	void	(*l_open)(dev_t, struct tty *, caddr_t);
	void	(*l_close)(struct tty *);
	int	(*l_read)(void);
	char	*(*l_write)(void);
	void	(*l_ioctl)(int, struct tty *, caddr_t);
	int	(*l_rint)(void);
	int	(*l_rend)(void);
	int	(*l_meta)(void);
	int	(*l_start)(void);
	int	(*l_modem)(void);
} linesw[];

#endif /* _CONF_H_ */
