#include "apue.h"
#include <fcntl.h>

void
set_f1(int fd, int flags)	/* flags are file status flags to turn on */
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val |= flags;	/* turn on flags */
	//val &= ~flags;	/* turn flags off */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl f_SETFL error");
}