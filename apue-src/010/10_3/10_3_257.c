#include "apue.h"

static void sig_usr(int);	/* one handler for both signals */

void reg_sig(int, __sighandler_t, char *);

int
main(void)
{
	reg_sig(SIGUSR1, sig_usr, "can't catch SIGUSR1");
	reg_sig(SIGUSR2, sig_usr, "can't catch SIGUSR2");
	for ( ; ; ) {
		printf("i am still alive...pid:%ld\n", (long)getpid());
		pause();
	}
}

static void
sig_usr(int signo)		/* argument is signal number */
{
	if (signo == SIGUSR1) {
		printf("received SIGUSR1\n");
		reg_sig(SIGUSR1, sig_usr, "can't catch SIGUSR1");
	}
	else if (signo == SIGUSR2)
		printf("received SIGUSR2\n");
	else
		err_dump("received signal %d\n", signo);
}

void reg_sig(int signo, __sighandler_t handler, char *errmsg) {
	if (signal(signo, handler) == SIG_ERR)
		err_sys(errmsg);
}
