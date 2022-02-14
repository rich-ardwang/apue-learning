#include "apue.h"

static void		sig_int(int);

int
main(void)
{
	sigset_t	newmask, oldmask, waitmask;

	pr_mask("program start: ");

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	sigemptyset(&waitmask);
	sigaddset(&waitmask, SIGUSR1);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);

	/*
	 * Block SIGINT and save current signal mask.
	 */
	// 将进程当前信号屏蔽字与newmask取并集，即阻塞SIGINT信号，
	// 并保存旧的信号屏蔽字。
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	/*
	 * Critical region of code.
	 */
	pr_mask("in critical region: ");

	/*
	 * Pause, allowing all signals except SIGUSR1.
	 */
	// 阻塞SIGUSR1信号，放行所有其他信号，然后进入阻塞状态等待除了SIGUSR1外的其他信号被触发，
	// 一旦有信号被触发，则信号处理函数执行完毕后，sigsuspend返回。
	if (sigsuspend(&waitmask) != -1)
		err_sys("sigsuspend error");

	// 不再阻塞SIGUSR信号，并恢复信号屏蔽字为调用sigsuspend之前的状态。
	pr_mask("after return from sigsuspend: ");

	/*
	 * Reset signal mask which unblocks SIGINT.
	 */
	// 恢复最原始的进程信号屏蔽字。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	/*
	 * And continue processing ...
	 */
	pr_mask("program exit: ");

	exit(0);
}

static void
sig_int(int signo)
{
	pr_mask("\nin sig_int: ");
}
