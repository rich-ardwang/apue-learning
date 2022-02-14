#include "apue.h"

static void		sig_alrm(int);

int
main(void)
{
	int			n;
	char		line[MAXLINE];

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	alarm(10);
	if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	//alarm(0);

	write(STDOUT_FILENO, line, n);
	exit(0);
}

static void
sig_alrm(int signo)
{
	/* nothing to do, just return to interrupt the read */
}

// 中断慢速系统调用，alarm设置时间超时后，使得read函数会被系统中断。
// 但是这里存在两个问题：
// 1.alarm和read之间存在race condition，如果alarm时间足够小，可能它会提前超时，
//    这将导致read被永远阻塞。
// 2.如果系统调用是自动重启动的，当从sig_alrm返回时，read不会被其中断。
