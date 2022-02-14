#include "apue.h"
#include <sys/socket.h>

/*
 * Returns a full-duplex pipe (a UNIX domain socket) with
 * the two file descriptors returned in fd[0] and fd[1].
 */
// 创建一根全双工管道。
int
fd_pipe(int fd[2])
{
	// 使用pipe创建的普通管道是半双工的，fd[0]为管道输入端，
	// fd[1]为管道输出端，数据传递方向是从fd[1]->fd[0]单
	// 向的。
	// 利用socketpair创建的管道是全双工的，即fd[0]和fd[1]
	// 都可以收发数据，数据传输是双向的。
	// socketpair可以创建一对无名的、相互连接的UNIX域套接字。
	return (socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}
