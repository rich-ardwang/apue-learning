#include "apue.h"
#include <errno.h>

void
pr_mask(const char *str)
{
	sigset_t		sigset;
	int				errno_save;

	errno_save = errno;			/* we can be called by signal handlers */
	// 不改变进程当前信号屏蔽字，取得进程当前信号屏蔽字放在sigset信号集中。
	if (sigprocmask(0, NULL, &sigset) < 0) {
		err_sys("sigprocmask error");
	} else {
		printf("%s", str);
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");

		/* remaining signals can go here */

		printf("\n");
	}

	errno = errno_save;		/* restore errno */
}

/* Add by Richard Wang
   2021-12-10
*/
int main() {
	sigset_t main_sigset;
	// 设置main_sigset包含所有信号。
	sigfillset(&main_sigset);
	// 初始化空信号集。
	//sigemptyset(&main_sigset);
	// 向main_sigset信号集中加入信号。
	//sigaddset(&main_sigset, SIGUSR1);
	//sigaddset(&main_sigset, SIGINT);
	// 设置进程新的屏蔽字设置为main_sigset集，不取得进程之前的信号屏蔽字。
	if (sigprocmask(SIG_SETMASK, &main_sigset, NULL) < 0)
		err_sys("sig block err.");
	pr_mask("hello, world.\n");
	exit(0);
}
