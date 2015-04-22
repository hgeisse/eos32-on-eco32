/*
 * sys.c -- indirect driver for controlling tty
 */

#include "../include/param.h"
#include "../include/dev.h"
#include "../include/blk.h"
#include "../include/off.h"
#include "../include/tim.h"
#include "../include/ino.h"
#include "../include/conf.h"
#include "../include/dir.h"
#include "../include/user.h"
#include "../include/tty.h"
#include "../include/proc.h"

#include "sys.h"

void syopen(dev_t dev, int flag)
{

	if(u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return;
	}
	(*cdevsw[major(u.u_ttyd)].d_open)(u.u_ttyd, flag);
}

void syclose(dev_t dev, int flag)
{
}

void syread(dev_t dev)
{

	(*cdevsw[major(u.u_ttyd)].d_read)(u.u_ttyd);
}

void sywrite(dev_t dev)
{

	(*cdevsw[major(u.u_ttyd)].d_write)(u.u_ttyd);
}

void syioctl(dev_t dev, int cmd, caddr_t addr, int flag)
{

	(*cdevsw[major(u.u_ttyd)].d_ioctl)(u.u_ttyd, cmd, addr, flag);
}
