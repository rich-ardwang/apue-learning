#include "apue.h"

#define BUFFSIZE	1024

static void
sig_tstp(int signo)		/* signal handler for SIGTSTP */
{
	printf("Reveived SIGTSTP.\n");

	sigset_t		mask;

	/* ... move cursor to lower left corner, reset tty mode ... */

	/*
	 * Unblock SIGTSTP, since it's blocked while we're handling it.
	 */
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	// 将mask信号集的补集与进程当前信号屏蔽字取交集，即解除对mask中所含信号的阻塞。
	sigprocmask(SIG_UNBLOCK, &mask, NULL);

	// 设置信号SIGTSTP为默认处理方式。
	signal(SIGTSTP, SIG_DFL);		/* reset disposition to default */

	// 进程给自己发SIGTSTP信号。
	printf("Before send SIGTSTP to myself.\n");
	kill(getpid(), SIGTSTP);		/* and send the signal to ourself */

	// 进程调用kill发送SIGTSTP给自己，这次直接使用默认方式结束进程，
	// 因此kill函数不会返回，它后面的代码不会得到执行。
	printf("After send SIGTSTP to myself.\n");

	/* we don't return from the kill until we're continued */

	// 重新注册该信号处理函数。
	signal(SIGTSTP, sig_tstp);		/* reestablish signal handler */

	/* ... reset tty mode, redraw screen ... */
}

int
main(void)
{
	int			n;
	char		buf[BUFFSIZE];

	/*
	 * Only catch SIGTSTP if we're running with a job-control shell.
	 */
	if (signal(SIGTSTP, SIG_IGN) == SIG_DFL)
		signal(SIGTSTP, sig_tstp);

	while ((n= read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");

	if (n < 0)
		err_sys("read error");

	exit(0);
}
