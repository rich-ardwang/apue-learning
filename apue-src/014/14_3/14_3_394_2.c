#include "apue.h"
#include <fcntl.h>

// 记录锁读测试。
// 如果存在一把锁，则返回持有该锁的进程pid，
// 否则返回0，即无记录锁状态。
pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;			/* F_RDLCK or F_WRLCK */
	lock.l_start = offset;		/* byte offset, relative to l_whence */
	lock.l_whence = whence;		/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;			/* #bytes (0 means to EOF) */

	// 如果存在一把锁，则阻止再次创建锁，并将lock信息改写为持有者设置的信息。
	// 否则，将lock.l_type改写为F_UNLCK。
	if (fcntl(fd, F_GETLK, &lock) < 0)
		err_sys("fcntl error");

	if (lock.l_type == F_UNLCK)
		return (0);			/* false, region isn't locked by another proc */
	return (lock.l_pid);	/* true, return pid of lock owner */
}
