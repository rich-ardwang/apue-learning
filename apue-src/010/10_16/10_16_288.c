#include "apue.h"

volatile sig_atomic_t	quitflag;		/* set nonzero by signal handler */

static void
sig_int(int signo)		/* one signal handler for SIGINT and SIGQUIT */
{
	if (signo == SIGINT) {
		printf("\ninterrupt\n");
		// Add by Richard on 2021-12-13
		if (signal(SIGINT, sig_int) == SIG_ERR)
			err_sys("signal(SIGINT) error");
	}
	else if (signo == SIGQUIT)
		quitflag = 1;	/* set flag for main loop */
}

int
main(void)
{
	sigset_t	newmask, oldmask, zeromask;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	if (signal(SIGQUIT, sig_int) == SIG_ERR)
		err_sys("signal(SIGQUIT) error");

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);

	/*
	* Block SIGQUIT and save current signal mask.
	 */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	printf("quitflag:%d\n",quitflag);
	while (quitflag == 0)
		// 不阻塞任何信号，放行所有信号，阻塞等待信号发生。
		sigsuspend(&zeromask);

	/*
	 * SIGQUIT has been caught and is now blocked; do whatever.
	 */
	quitflag = 0;

	/*
	 * Reset signal mask which unblocks SIQQUIT.
	 */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	exit(0);
}
