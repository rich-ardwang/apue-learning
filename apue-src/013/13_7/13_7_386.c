#include "apue.h"
#include <fcntl.h>

// 对文件描述符设置执行时关闭。
int
set_cloexec(int fd)
{
	int		val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return (-1);

	val |= FD_CLOEXEC;		/* enable close-on-exec */

	return (fcntl(fd, F_SETFD, val));
}

// 可以对所有被执行程序不需要的文件描述符设置执行时关闭，
// 这样可以更加安全，以免未关闭的不需要的文件描述符被黑客
// 进程调用。
