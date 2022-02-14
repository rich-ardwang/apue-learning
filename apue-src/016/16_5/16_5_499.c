#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN		128
// Alarm信号超时设置。
#define TIMEOUT		20

void
signalrm(int signo)
{
	// Add by Richard at 2022-1-12.
	printf("Catch SIGALRM.\n");
}

void
print_uptime(int sockfd, struct addrinfo *aip)
{
	int		n;
	char	buf[BUFLEN];

	// 给服务器发送一个字节的数据，即一个空字符。
	buf[0] = 0;
	// 因为是面向无连接，需要和服务端协商好请求ruptime的方式，
	// 这里只简单给服务端发送一个字节的NULL字符，然后服务收到
	// 后会解析出客户端地址，并且将uptime的输出结果发送给客户端。
	// sendto和send很类似，区别是sendto可以在无连接的套接字上
	// 指定一个目标地址。
	if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
		err_sys("sendto error");
	// 设置alarm超时器，超时机制可以避免调用recvfrom函数后的无限期阻塞。
	alarm(TIMEOUT);
	// 如果服务端没有正常运行，则recvfrom会永久阻塞，这里加入了超时报警
	// 机制加以避免。
	// 如果最后两个参数为NULL，recvfrom与recv几乎等同，否则它可以定位
	// 发送者的源地址，即发送端套接字端点(socket entry)。
	if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
		// 接收失败。
		if (errno != EINTR)
			// 如果不是由于超时alarm信号引起的失败，
			// 清零超时器。
			alarm(0);
		// 错误输出至stderr并退出进程。
		err_sys("recv error");
	}
	// 收到服务端正常响应的结果，清零超时器。
	alarm(0);
	// 将接收到的数据输出至stdout。
	write(STDOUT_FILENO, buf, n);
}

int
main(int argc, char *argv[])
{
	struct addrinfo			*ailist, *aip;
	struct addrinfo			hint;
	int						sockfd, err;
	struct sigaction		sa;

	// 只接受2个参数，其中一个输入参数为服务器名称。
	if (argc != 2)
		err_quit("usage: ruptime hostname");
	// 注册对SIGALRM信号的自定义处理函数。
	sa.sa_handler = signalrm;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		err_sys("sigaction error");
	// 初始化地址过滤模板hint。
	memset(&hint, 0, sizeof(hint));
	// socket类型为数据报。
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	// 根据服务器名称+服务名称，获取一组符合条件的地址集。
	if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	// 使用结果集中的第一组地址进行socket数据报类型的通信配置，
	// 这个类型默认协议为UDP。
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0) {
			// 初始化套接字出错，返回错误编号。
			err = errno;
		} else {
			// 初始化套接字成功，面向服务端收发数据报。
			print_uptime(sockfd, aip);
			// 正常退出进程。
			exit(0);
		}
	}
	// 不能与服务端建立UDP通信，错误输出至stderr。
	fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(err));
	// 标记为异常退出。
	exit(1);
}

// 点评：
// 本例演示了一个面向无连接的，采用数据报套接字接口的uptime客户端
// 命令版本。
