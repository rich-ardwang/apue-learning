#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include "../16_4/16_4_490.c"

#define BUFLEN			128
#define MAXADDRLEN		256

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX	256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
	int					n;
	socklen_t			alen;
	FILE				*fp;
	char				buf[BUFLEN];
	char				abuf[MAXADDRLEN];
	struct sockaddr		*addr = (struct sockaddr *)abuf;

	// 设置sockfd运行时关闭。
	set_cloexec(sockfd);
	for (;;) {
		alen = MAXADDRLEN;
		// 阻塞等待有客户端发来UDP数据报，后两个参数非空，说明
		// 需要获取客户端的地址信息。
		if ((n = recvfrom(sockfd, buf, BUFLEN, 0, addr, &alen)) < 0) {
			// 接收数据报失败，标记异常，退出服务端进程。
			syslog(LOG_ERR, "ruptime: recvfrom error: %s", strerror(errno));
			exit(1);
		}
		// 成功接收并解析客户端发来的UDP数据报。
		// fork进程，让子进程管道接入stdout，让父进程从管道pfd[1]中
		// 接收数据，子进程调用uptime，将结果写入stdout，父进程从pfd[1]
		// 中接收数据。
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			// popen失败，将错误消息通过数据报发送给客户端。
			sprintf(buf, "error: %s\n", strerror(errno));
			sendto(sockfd, buf, strlen(buf), 0, addr, alen);
		} else {
			// popen成功，使用标准库从pfd[1]管道中读取数据，默认为全缓冲。
			if (fgets(buf, BUFLEN, fp) != NULL)
				// 读完数据后通过UDP发送给客户端。
				sendto(sockfd, buf, strlen(buf), 0, addr, alen);
			// 关闭管道，阻塞等待uptime子进程退出。
			pclose(fp);
		}
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	int					sockfd, err, n;
	char				*host;

	if (argc != 1)
		err_quit("usage: ruptime");
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;		/* best guess */
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptime");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	// 注意这里设置为数据报类型，此类型默认协议为UDP。
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	// 根据服务器主机名+服务程序名，调用getaddrinfo函数获取地址集。
	// 注意，服务端口号需要在/etc/services中配置。
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptime: getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		// 使用结果集中第一个地址进行UDP配置。
		if ((sockfd = initserver(SOCK_DGRAM, aip->ai_addr, aip->ai_addrlen, 0)) >= 0) {
			// UDP配置成功，面向客户端读写数据报，提供服务。
			serve(sockfd);
			// 服务端进程正常退出。
			exit(0);
		}
	}
	// 配置失败，标记异常，退出进程。
	exit(1);
}

// 点评：
// 本例演示的是一个面向无连接的、使用套接字数据报的服务器，
// 该服务器接受客户端查询uptime请求，将结果同样以UDP的方
// 式发送给客户端。

// 注意：
// 为了区别于面向连接的服务端程序，我们故意将这个程序编译
// 为ruptime，即名称尾字符去掉d。
// 同样需要vi /etc/services，加入ruptime 21818/udp，
// 否则getaddrinfo报错：
// getaddrinfo error: Servname not supported for ai_socktype
