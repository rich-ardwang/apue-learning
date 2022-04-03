#include "apue.h"
#include <fcntl.h>

void
set_f1(int fd, int flags)	/* flags are file status flags to turn on */
{
	int		val;

	// 首先取得原有的文件状态标志位。
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	// 在保证原有的文件状态标志位不变的情况下，
	// 开启某个标志位。
	val |= flags;	/* turn on flags */
	// 在保证原有的文件状态标志位不变的情况下，
	// 关闭某个标志位。
	//val &= ~flags;	/* turn flags off */

	// 将改动后的状态设置进去。
	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl f_SETFL error");
}

// 评论
// 修改文件描述符标志或文件状态标志时必须谨慎，应先取得相应
// 的文件描述符标志或文件状态标志，然后在原来的基础上打开或
// 关闭要设置的标志位。而不应直接使用F_SETFD或F_SETFL命令，
// 因为这样做会使原来的标志位丢失。
