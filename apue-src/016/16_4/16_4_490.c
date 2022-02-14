#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

// 初始化一个Socket Server。
int
initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int		fd;
	int		err = 0;

	// 参数3：协议设为0，表示为给定的域和套接字类型选择默认协议。
	// 返回套接字描述符。
	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return (-1);
	// 将套接字与服务器地址关联，这里称为地址绑定。
	// 地址中的端口号必须大于等于1024，小于1024的端口号
	// 需要有root权限。
	if (bind(fd, addr, alen) < 0)
		goto errout;
	// SOCK_STREAM的默认协议是TCP，SOCK_SEQPACKET的默认协议是SCTP。
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
		// 服务器调用listen函数来宣告它愿意接受连接请求。
		// 参数2：提示系统该进程所要入队的未完成连接请求数量。
		// 其实际值由系统决定，上限SOMAXCONN，在<sys/socket.h>
		// 中定义。
		// 一旦队列满，系统就会拒绝多余的连接请求，所以参数2的值
		// 应该基于服务器期望负载和处理量来选择，处理量是指接受连接
		// 请求与启动服务的数量。
		if (listen(fd, qlen) < 0)
			goto errout;
	}
	return (fd);

errout:
	err = errno;
	// 关闭socket套接字。
	close(fd);
	errno = err;
	return (-1);
}
