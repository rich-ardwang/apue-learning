#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include "../16_4/16_4_490.c"

// TCP传输数据时使用的缓冲区。
#define BUFLEN		128
// 服务端期望系统支持的队列连接数。
#define QLEN		10

// 主机名最大长度如果没定义，则自定义。
// POSIX.1规定主机名最大长度为255，不
// 包含NULL，所以定义为256。
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX	256
#endif

// 执行初始化socket server的必要操作。
extern int initserver(int, const struct sockaddr *, socklen_t, int);

// 开始面向客户端连接，然后为其提供服务。
void
serve(int sockfd)
{
	int			clfd;
	FILE		*fp;
	char		buf[BUFLEN];

	// 对旧socket描述符设置执行时关闭。
	set_cloexec(sockfd);
	for (;;) {
		// 获得客户端socket连接请求并建立连接。
		// accept函数返回一个新的socket描述符，该描述符连
		// 接到调用connect的客户端。
		// 而旧的socket描述符则没有关联到这个连接，新旧socket
		// 描述符具有相同的套接字类型和地址族。
		// 参数2和参数3用于获取客户端的地址信息，如果不感兴趣客
		// 户端标识，可以将它们置NULL。
		// accept会阻塞等待一个客户端连接到来，如果sockfd处于
		// 非阻塞模式，则返回-1并设errno为EAGAIN或EWOULDBLOCK。
		if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
			// 记录系统日志，因为是守护进程，没有终端。
			syslog(LOG_ERR, "ruptimed: accept error: %s",
				strerror(errno));
			exit(1);
		}
		// 对新的socket描述符设置执行时关闭。
		set_cloexec(clfd);
		// fork进程，子进程调用excel执行uptime程序，子进程管道指向
		// stdout，父进程从管道pfd[0]只读数据。
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			// 产生失败消息，并通过socket发送给客户端。
			sprintf(buf, "error: %s\n", strerror(errno));
			send(clfd, buf, strlen(buf), 0);
		} else {
			// 读取管道成功，使用标准库获取管道数据，默认为全缓冲。
			while (fgets(buf, BUFLEN, fp) != NULL)
				// 将全缓冲数据通过socket发送至客户端。
				send(clfd, buf, strlen(buf), 0);
			// 关闭管道，等待子进程退出并收集状态。
			pclose(fp);
		}
		// 关闭socket连接。
		close(clfd);
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	int					sockfd, err, n;
	char				*host;

	// 除了服务进程名称不接受任何输入参数。
	if (argc != 1)
		err_quit("usage: ruptimed");

	// 尝试读取系统对最大主机名长度的动态定义，
	// 如果读取失败，则使用自定义值。
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;		/* best guess */
	// 在heap上申请一块内存存储主机名。
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	// 获取本地主机名称。 
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	// 以守护进程方式运行服务，服务名称为ruptimed。
	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	// 使用AI_CANONNAME和SOCK_STREAM过滤出需要的地址信息。
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	// 获取服务器本地的地址信息。
	if ((err = getaddrinfo(host, "ruptimed", &hint, &ailist)) != 0) {
		// gai_strerror函数可以获取到错误编号对应的字符串信息。
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}
	// 可能返回多个地址信息，这里只选择第一个来建立被动套接字端点(socket entry)，
	// 即一个只用于监听连接请求的地址。
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
			// 当建立socket服务端成功后，返回套接字描述符，
			// 使用它可以面向客户端，接受连接并提供服务。
			serve(sockfd);
			exit(0);
		}
	}
	// 跳出循环意味着不能成功建立socket服务端，
	// 异常退出即可。
	exit(1);
}

// 点评：
// 本例演示了一个面向连接的服务器程序，该程序接受TCP连接，
// 向客户端提供系统正常运行时间。

// 注意：
// sudo vi /etc/services，添加ruptimed   18181，为
// ruptimed服务添加端口号，否则会报错：
// ruptimed: getaddrinfo error: Servname not supported for ai_socktype。
// 还要将服务程序编译成ruptimed。
