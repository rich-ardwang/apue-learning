#include "apue.h"

/* Reliable version of signal(), using POSIX sigaction(). */
// 利用sigaction函数重新实现了signal函数，取名为my_signal。
Sigfunc *
my_signal(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;
	// 指定信号处理函数。
	act.sa_handler = func;
	// 取消一切信号屏蔽字。
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	// 对除了SIGALRM信号以外的所有信号中断的系统调用，尝试使用自动重启动处理。
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
		act.sa_flags |= SA_RESTART;
	}
	// 更新设置。
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	// 返回信号处理函数的函数指针。
	return (oact.sa_handler);
}

/* Add by Richard Wang
   2021-12-10
*/
void sig_handle(int signo) {
	printf("sig_handle, signo:%d.\n", signo);
	// 设置对SIGQUIT信号的处理方式为默认，即下次再收到此信号不再执行sig_handle，
	// 而是执行系统默认的处理函数。
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}

int main() {
	if (my_signal(SIGQUIT, sig_handle) == SIG_ERR)
		err_sys("my_signal(SIGQUIT) error");
	for (;;) {
		printf("i am still alive...\n");
		sleep(5);
	}
}
