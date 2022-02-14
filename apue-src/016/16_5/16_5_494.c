#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include "../16_4/16_4_488.c"

// 设置TCP数据传输时使用的缓冲区大小。
#define BUFLEN		128

// socket重连，或新建一个socket连接。
extern int connect_retry(int, int, int, const struct sockaddr *, socklen_t);

// 将从服务端获取到的远程正常运行时间打印出来。
void
print_uptime(int sockfd)
{
	int		n;
	char	buf[BUFLEN];

	// 从socket读取数据。
	while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
		// 使用系统调用将读取的数据写入到stdout。
		write(STDOUT_FILENO, buf, n);
	if (n < 0)
		err_sys("recv error");
}

int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	int					sockfd, err;

	// 输入参数为服务端地址+端口，也就是服务端hostname。
	if (argc != 2)
		err_quit("usage: ruptimed hostname");
	// 提供一个用于过滤特定地址的hint模板，初始化该模板。
	memset(&hint, 0, sizeof(hint));
	// 指定过滤协议默认为TCP。
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	// 根据SOCK_STREAM获取服务端地址信息。
	// 参数1：服务端hostname(IP+端口)。
	// 参数2：服务端服务名称。
	// 参数3：提供的过滤模板。
	// 参数4：返回的结果集。
	if ((err = getaddrinfo(argv[1], "ruptimed", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	// 如果服务器支持多重网络接口或多重网络协议，getaddrinfo会返回多个候选地址
	// 供使用，轮流尝试每个地址，当找到一个允许连接到服务器的地址时便可停止。
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		// 向服务端目标服务发起TCP连接或重连。
		if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0,
			aip->ai_addr, aip->ai_addrlen)) < 0) {
			err = errno;
		} else {
			// 连接成功，接收并打印ruptime数据。
			print_uptime(sockfd);
			// 正常退出。
			exit(0);
		}
	}
	// 跳出循环意味着TCP连接失败，报错退出。
	err_exit(err, "can't connect to %s", argv[1]);
}

// 点评：
// 该例演示的是一个面向连接的客户端，他可以从服务器端
// 获取远程正常运行时间(remote uptime，简称ruptime)。
