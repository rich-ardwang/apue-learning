#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

int
initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int		fd, err;
	int		reuse = 1;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return (-1);
	// 设置套接字选项，将SO_REUSEADDR选项设为非0，即启用。
	// 参数1：套接字描述符。
	// 参数2：通用套接字层次(SOL_SOCKET)，针对具体的协议
	// 的套接字层次，如TCP(IPPROTO_TCP)，IP(IPPROTO_IP)
	// 等。
	// 参数3：套接字选项名称(__optname)。
	// 参数4：套接字选项值(__optval)。
	// 参数5：指定了__optval对象的大小。
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		goto errout;
	if (bind(fd, addr, alen) < 0)
		goto errout;
	// TCP协议或SCTP协议。
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
		if (listen(fd, qlen) < 0)
			goto errout;
	return (fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return (-1);
}

// 点评：
// 本例演示套接字选项接口的使用，当服务程序终止并立即
// 重新启动，默认的socket选项将使bind函数无法正常工
// 作，TCP实现不允许立即绑定重复的地址和端口，除非等
// 待几分钟，待超时后相同的地址+端口才能被复用。
// SO_REUSEADDR选项可以打破这个常规，使用套接字选项
// 接口将其设置好，这将使得相同的地址+端口被立即复用。

// 套接字选项接口有两个：setsockopt和getsockopt，用于
// 设置套接字选项和查询选项的状态，套接字选项将影响套接
// 字的很多动作和行为。
