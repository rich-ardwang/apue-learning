#include "apue.h"

static void		sig_quit(int);

int
main(void)
{
	sigset_t newmask, oldmask, pendmask;

	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("can't catch SIGQUIT");

	/*
	 * Block SIGQUIT and save current signal mask.
	 */
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	// 进程当前信号屏蔽字与newmask取并集，返回进程旧的信号屏蔽字。
	// 也就是这里想要让进程阻塞SIGQUIT信号。
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	sleep(5);		/* SIGQUIT here will remain pending */
	// 5秒之前，所有SIGQUIT信号都被阻塞，不会发送到该进程。

	// 取得所有未决信号集。
	if (sigpending(&pendmask) < 0)
		err_sys("sigpending error");
	// 检查SIGQUIT信号是否是未决的，即检查5秒前是否有SIGQUIT信号产生，
	// 但是没有发送给该进程。
	if (sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	/*
	 * Restore signal mask which unblocks SIGQUIT.
	 */
	// 将保存的旧的进程屏蔽字恢复，即不再阻塞SIGQUIT信号。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");

	sleep(5);		/* SIGQUIT here will terminate with core file */
	exit(0);
}

static void
sig_quit(int signo)
{
	printf("caught SIGQUIT\n");
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}
