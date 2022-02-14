#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "../16_6/16_6_503.c"

#define QLEN	10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX	256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
	int		clfd, status;
	pid_t	pid;

	set_cloexec(sockfd);
	for (;;) {
		if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
			exit(1);
		}
		if ((pid = fork()) < 0) {
			syslog(LOG_ERR, "ruptimed: fork error: %s", strerror(errno));
			exit(1);
		} else if (pid == 0) {		/* child */
			/*
			 * The parent called daemonize (Figure 13.1), so
			 * STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO
			 * are already open to /dev/null. Thus, the call to
			 * close doesn't need to be protected by checks that
			 * clfd isn't already equal to one of these values.
			 */
			// 这里不需要使用check方式检查clfd，因为父进程已经调用了
			// daemonize，其stdin、stdout和stderr都在/dev/null打
			// 开，clfd不可能等于它们中的任何一个。
			// 因此直接将其在stdout和stderr打开即可。
			if (dup2(clfd, STDOUT_FILENO) != STDOUT_FILENO ||
					dup2(clfd, STDERR_FILENO) != STDERR_FILENO) {
				syslog(LOG_ERR, "ruptimed: unexpected error");
				exit(1);
			}
			// clfd所描述的socket连接已经在stdout打开，可以将其关闭了。
			close(clfd);
			// 子进程执行uptime，这个进程会向stdout或stderr写入数据，因
			// 为它们已经关联到客户端的socket连接，所以客户端可以读取到数据。
			execl("/usr/bin/uptime", "uptime", (char *)0);
			// 如果一切正常，execl会正常退出子进程，不会返回。
			// 如果execl返回了，说明出错，则输出错误日志。
			syslog(LOG_ERR, "ruptimed: unexpected return from exec: %s", strerror(errno));
		} else {		/* parent */
			// 父进程关闭不需要的socket描述符。
			close(clfd);
			// 父进程阻塞等待子进程退出。
			waitpid(pid, &status, 0);
		}
	}
}

// main方法与16_5_495的相同。
int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	int					sockfd, err, n;
	char				*host;

	if (argc != 1)
		err_quit("usage: ruptimed");
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;		/* best guess */
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptimed", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
				aip->ai_addrlen, QLEN)) >= 0) {
			serve(sockfd);
			exit(0);
		}
	}
	exit(1);
}

// 点评：
// 该例与16_5_495基本一致，只是serve方法的实现机制有所不同，
// 该服务端不会读取uptime然后发送给客户端，而是把与客户端取得
// 关联的socket描述符在stdout和stderr打开，这样uptime产生
// 一切输出将直接发送给客户端。
