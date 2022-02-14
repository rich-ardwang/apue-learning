#include "apue.h"
#include <setjmp.h>

static void			sig_alrm(int);
static jmp_buf		env_alrm;

int
main(void)
{
	int		n;
	char	line[MAXLINE];

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");
	if (setjmp(env_alrm) != 0)
		err_quit("read timeout");
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
	longjmp(env_alrm, 1);
}

// 这个案例是10_10_271的改进版，不管系统调用是否是自动重启动，
// longjmp返回后都将执行err_quit，结束read系统调用。
