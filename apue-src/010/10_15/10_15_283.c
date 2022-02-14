#include "apue.h"
#include <setjmp.h>
#include <time.h>

static void						sig_usr1(int);
static void						sig_alrm(int);
static sigjmp_buf				jmpbuf;
static volatile sig_atomic_t	canjump;

int
main(void)
{
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	/*
	// 屏蔽SIGINT信号。
	sigset_t curset;
	sigemptyset(&curset);
	sigaddset(&curset, SIGINT);
	if (sigprocmask(SIG_SETMASK, &curset, NULL) < 0)
		err_sys("sigprocmask error");
	*/

	pr_mask("starting main: ");			/* Figure 10.14 */

	if (sigsetjmp(jmpbuf, 1)) {
		pr_mask("ending main: ");
		exit(0);
	}
	canjump = 1;	/* now sigsetjmp() is OK */

	for ( ; ; ) {
		printf("pid=%d\n", getpid());
		pause();
	}
}

static void
sig_usr1(int signo)
{
	time_t		starttime;

	if (canjump == 0)
		return;					/* unexpected signal, ignore */

	pr_mask("starting sig_usr1: ");

	alarm(3);					/* SIGALRM in 3 seconds */
	starttime = time(NULL);
	for ( ; ; )					/* busy wait for 5 seconds */
		if (time(NULL) > starttime + 5)
			break;

	pr_mask("finishing sig_usr1: ");

	canjump = 0;
	siglongjmp(jmpbuf, 1);		/* jump back to main, don't return */
}

static void
sig_alrm(int signo)
{
	pr_mask("in sig_alrm: ");
}

/* Add by Richard Wang
   2021-12-13
   注意：信号处理函数并没有自动把该信号加入进程当前屏蔽字信号集，此时如果有相同信号发出，进程扔然可以收到，但是会异常终止。关于这一点的实际测试结果与书中描述不一致。
*/
// 按照原书作者的设想，siglongjmp返回后会恢复有之前sigsetjmp所保存的信号屏蔽字，
// 如果把他们替换成longjmp和setjmp，则longjmp返回后不会恢复信号屏蔽字。
